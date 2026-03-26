.section .text
.syntax unified
.code 32

.global _start
.global jump_to_process
.global PUT32
.global GET32
.global enable_irq
.global _irq_stack_top

.extern main
.extern timer_irq_handler
.extern CurrProcess

.extern _stack_top
.extern schedule

.equ PROC_PID,    0
.equ PROC_R0,     4
.equ PROC_R1,     8
.equ PROC_R2,     12
.equ PROC_R3,     16
.equ PROC_R4,     20
.equ PROC_R5,     24
.equ PROC_R6,     28
.equ PROC_R7,     32
.equ PROC_R8,     36
.equ PROC_R9,     40
.equ PROC_R10,    44
.equ PROC_R11,    48
.equ PROC_R12,    52
.equ PROC_SP,     56
.equ PROC_LR,     60
.equ PROC_PC,     64
.equ PROC_SPSR,   68
.equ PROC_STATE,  72

// ============================================================
// Exception Vector Table
// ============================================================

.align 5
vector_table:
    b _start              @ 0x00 Reset
    b undefined_handler   @ 0x04 Undefined
    b swi_handler         @ 0x08 SWI
    b prefetch_handler    @ 0x0C Prefetch abort
    b data_handler        @ 0x10 Data abort
    b .                   @ 0x14 Reserved
    b irq_handler         @ 0x18 IRQ
    b fiq_handler         @ 0x1C FIQ

// ============================================================
// Reset
// ============================================================

_start:
    // Configurar VBAR
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0

    // --------------------------------------------------------
    // Inicializar stack de IRQ
    // --------------------------------------------------------
    mrs r0, cpsr
    bic r1, r0, #0x1F
    orr r1, r1, #0x12      @ modo IRQ
    orr r1, r1, #0x80      @ IRQ disable
    msr cpsr_c, r1

    ldr sp, =_irq_stack_top

    mov r2, #0x00000013    @ SVC mode, IRQs habilitadas
    msr spsr_cxsf, r2      @ ← agregar esta línea

    // --------------------------------------------------------
    // Volver a SVC e inicializar stack del OS
    // --------------------------------------------------------
    bic r1, r0, #0x1F
    orr r1, r1, #0x13      @ modo SVC
    orr r1, r1, #0x80      @ IRQ disable
    msr cpsr_c, r1

    ldr sp, =_stack_top

    // Llamar main del OS
    bl main

hang:
    b hang

// ============================================================
// Saltar a un proceso
// r0 = pc
// r1 = sp
// ============================================================

jump_to_process:
    mov sp, r1
    mov lr, #0
    bx  r0

// ============================================================
// IRQ handler
// ============================================================

irq_handler:

    // --------------------------------------------------------
    // Etapa 1: Salvar registros de trabajo en el stack IRQ
    // 14 registros × 4 bytes = 56 bytes
    // Disposición: r0@sp+0, r1@sp+4, ... r12@sp+48, lr@sp+52
    // --------------------------------------------------------
    sub  sp, sp, #56
    stmia sp, {r0-r12, lr}

    // --------------------------------------------------------
    // Etapa 2: Cargar puntero al proceso actual
    // --------------------------------------------------------
    ldr  r4, =CurrProcess
    ldr  r4, [r4]

    cmp  r4, #0
    beq  irq_no_current_process     // safety: no debería ocurrir

    // --------------------------------------------------------
    // Etapa 3: Guardar r0-r12 en el PCB
    // (los valores reales están en el stack IRQ, no en los regs)
    // --------------------------------------------------------
    sub  sp, sp, #56
    stmia sp, {r0-r12, lr} 

    // --------------------------------------------------------
    // Etapa 4: Guardar SPSR (= CPSR del proceso interrumpido)
    // --------------------------------------------------------
    mrs  r5, spsr
    str  r5, [r4, #PROC_SPSR]

    // --------------------------------------------------------
    // Etapa 5: Guardar PC del proceso
    // En IRQ, lr_irq = PC_interrumpido + 4  →  restamos 4
    // --------------------------------------------------------
    ldr  r5, [sp, #52]             @ lr_irq guardado en stack
    sub  r5, r5, #4
    str  r5, [r4, #PROC_PC]

    // --------------------------------------------------------
    // Etapa 6: Guardar SP y LR del proceso (registros SVC)
    // sp_svc y lr_svc son banked → hay que cambiar a SVC mode
    // para leerlos; r0-r12 son COMPARTIDOS entre modos
    // --------------------------------------------------------
    mrs  r6, cpsr                  @ guardar CPSR (modo IRQ actual)

    bic  r7, r6, #0x1F
    orr  r7, r7, #0x13             @ modo SVC
    orr  r7, r7, #0x80             @ IRQ disable (precaución)
    msr  cpsr_c, r7

    str  sp, [r4, #PROC_SP]        @ sp_svc del proceso
    str  lr, [r4, #PROC_LR]        @ lr_svc del proceso

    msr  cpsr_c, r6                @ volver a modo IRQ

    // --------------------------------------------------------
    // Etapa 7: Limpiar interrupción del timer
    // r4 es callee-saved → sobrevive el bl sin problema
    // --------------------------------------------------------
irq_no_current_process:
    bl   timer_irq_handler

    // --------------------------------------------------------
    // Etapa 8: Scheduler — encola proceso actual, desencola
    //          el siguiente y actualiza CurrProcess
    // --------------------------------------------------------
    bl   schedule

    // --------------------------------------------------------
    // Etapa 9: Cargar el NUEVO proceso actual
    // (schedule() actualizó CurrProcess)
    // --------------------------------------------------------
    ldr  r4, =CurrProcess
    ldr  r4, [r4]

    // --------------------------------------------------------
    // Etapa 10: Preparar el retorno al nuevo proceso
    //
    // El truco del ARM:
    //   - lr_irq   = dirección a la que saltaremos (PC del proceso)
    //   - spsr_irq = CPSR que se restaurará al ejecutar movs pc, lr
    //   - movs pc, lr  hace ambas cosas en un solo ciclo
    // --------------------------------------------------------

    // Cargar PC del nuevo proceso en lr_irq (banco IRQ)
    ldr  lr, [r4, #PROC_PC]

    // Restaurar SPSR con el CPSR guardado del nuevo proceso
    ldr  r5, [r4, #PROC_SPSR]
    msr  spsr_cxsf, r5

    // Cambiar a SVC para restaurar sp_svc y lr_svc del nuevo proceso
    // lr_irq queda intacto porque es un registro banked
    mrs  r6, cpsr
    bic  r7, r6, #0x1F
    orr  r7, r7, #0x13
    orr  r7, r7, #0x80
    msr  cpsr_c, r7

    ldr  sp, [r4, #PROC_SP]        @ restaurar sp_svc
    ldr  lr, [r4, #PROC_LR]        @ restaurar lr_svc

    // Volver a IRQ — lr_irq sigue con el PC del nuevo proceso
    msr  cpsr_c, r6

    // --------------------------------------------------------
    // Etapa 11: Restaurar r0-r12 del PCB con ldmia
    //
    // r4 apunta al PCB; sumamos PROC_R0 (offset 4) para apuntar
    // directo al arreglo r[0..12]. ldmia carga 13 registros
    // consecutivos: r0←r[0], r1←r[1], ..., r4←r[4], ..., r12←r[12]
    // r4 queda sobreescrito con su valor correcto del PCB. ✓
    // --------------------------------------------------------
    add  r4, r4, #PROC_R0
    ldmia r4, {r0-r3, r5-r12}   @ salta r4
    ldr  r4, [r4, #16]     

    // --------------------------------------------------------
    // Etapa 12: Saltar al nuevo proceso
    // movs pc, lr  (con S-bit en modo privilegiado):
    //   → PC = lr_irq  (= nuevo proceso PC)
    //   → CPSR = SPSR_irq (= CPSR guardado del nuevo proceso)
    // --------------------------------------------------------
    movs pc, lr

// ============================================================
// Otros handlers
// ============================================================

undefined_handler:
    b hang

swi_handler:
    b hang

prefetch_handler:
    b hang

data_handler:
    b hang

fiq_handler:
    b hang

// ============================================================
// Acceso de memoria
// ============================================================

PUT32:
    str r1, [r0]
    bx lr

GET32:
    ldr r0, [r0]
    bx lr

enable_irq:
    mrs r0, cpsr
    bic r0, r0, #0x80
    msr cpsr_c, r0
    bx lr

// ============================================================
// Stacks
// ============================================================

.section .bss
.align 8

irq_stack:
    .space 4096
_irq_stack_top:
