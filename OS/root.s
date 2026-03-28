@@ -1,300 +1,281 @@
.section .text
.syntax unified
.code 32

.global _start
.global jump_to_process
.global PUT32
.global GET32
.global enable_irq
.global disable_irq
.global yield

.extern main
.extern timer_irq_handler
.extern CurrProcess

.extern _stack_top
.extern _irq_stack_top
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

_start:
    // Configurar VBAR
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0

    mrs r0, cpsr
    bic r1, r0, #0x1F
    orr r1, r1, #0x12      @ modo IRQ
    orr r1, r1, #0x80      @ IRQ disable
    msr cpsr_c, r1

    ldr sp, =_irq_stack_top

    bic r1, r0, #0x1F
    orr r1, r1, #0x13      @ modo SVC
    orr r1, r1, #0x80      @ IRQ disable
    msr cpsr_c, r1

    ldr sp, =_stack_top

    // Llamar main del OS
    bl main

hang:
    b hang

jump_to_process:
    mov sp, r1

    bx  r0

irq_handler:

    // Etapa 1: Salvar registros de trabajo en el stack IRQ
    sub  sp, sp, #56
    stmia sp, {r0-r12, lr}

    // Etapa 2: Cargar puntero al proceso actual
    ldr  r4, =CurrProcess
    ldr  r4, [r4]

    cmp  r4, #0
    beq  irq_no_current_process

    // Etapa 3: Guardar r0-r12 en el PCB
    ldr  r5, [sp, #0]
    str  r5, [r4, #PROC_R0]
    ldr  r5, [sp, #4]
    str  r5, [r4, #PROC_R1]
    ldr  r5, [sp, #8]
    str  r5, [r4, #PROC_R2]
    ldr  r5, [sp, #12]
    str  r5, [r4, #PROC_R3]
    ldr  r5, [sp, #16]
    str  r5, [r4, #PROC_R4]
    ldr  r5, [sp, #20]
    str  r5, [r4, #PROC_R5]
    ldr  r5, [sp, #24]
    str  r5, [r4, #PROC_R6]
    ldr  r5, [sp, #28]
    str  r5, [r4, #PROC_R7]
    ldr  r5, [sp, #32]
    str  r5, [r4, #PROC_R8]
    ldr  r5, [sp, #36]
    str  r5, [r4, #PROC_R9]
    ldr  r5, [sp, #40]
    str  r5, [r4, #PROC_R10]
    ldr  r5, [sp, #44]
    str  r5, [r4, #PROC_R11]
    ldr  r5, [sp, #48]
    str  r5, [r4, #PROC_R12]

    // Etapa 4: Guardar SPSR (= CPSR del proceso interrumpido)
    mrs  r5, spsr
    str  r5, [r4, #PROC_SPSR]

    // Etapa 5: Guardar PC del proceso
    ldr  r5, [sp, #52]
    sub  r5, r5, #4
    str  r5, [r4, #PROC_PC]

    // Etapa 6: Guardar SP y LR del proceso (registros SVC)
    mrs  r6, cpsr

    bic  r7, r6, #0x1F
    orr  r7, r7, #0x13
    orr  r7, r7, #0x80
    msr  cpsr_c, r7

    str  sp, [r4, #PROC_SP]
    str  lr, [r4, #PROC_LR]

    msr  cpsr_c, r6

    // Etapa 7: Limpiar interrupción del timer
    bl   timer_irq_handler

    // Etapa 8: Scheduler — encola proceso actual, desencola
    bl   schedule

    // Etapa 9: Cargar el NUEVO proceso actual
    ldr  r4, =CurrProcess
    ldr  r4, [r4]

    // Etapa 10: Preparar el retorno al nuevo proceso
    ldr  lr, [r4, #PROC_PC]

    ldr  r5, [r4, #PROC_SPSR]
    msr  spsr_cxsf, r5
    mrs  r6, cpsr
    bic  r7, r6, #0x1F
    orr  r7, r7, #0x13
    orr  r7, r7, #0x80
    msr  cpsr_c, r7

    ldr  sp, [r4, #PROC_SP]        @ restaurar sp_svc
    ldr  lr, [r4, #PROC_LR]        @ restaurar lr_svc

    // Volver a IRQ — lr_irq sigue con el PC del nuevo proceso
    msr  cpsr_c, r6

    // Etapa 11: Restaurar r0-r12 del PCB con ldmia
    add  r4, r4, #PROC_R0
    ldmia r4, {r0-r12}


    // Etapa 12: Saltar al nuevo proceso
    movs pc, lr

yield:
    push {r0-r12, lr}

    ldr  r4, =CurrProcess
    ldr  r4, [r4]

    cmp  r4, #0
    beq  yield_no_process

    ldr  r5, [sp, #0]
    str  r5, [r4, #PROC_R0]
    ldr  r5, [sp, #4]
    str  r5, [r4, #PROC_R1]
    ldr  r5, [sp, #8]
    str  r5, [r4, #PROC_R2]
    ldr  r5, [sp, #12]
    str  r5, [r4, #PROC_R3]
    ldr  r5, [sp, #16]
    str  r5, [r4, #PROC_R4]
    ldr  r5, [sp, #20]
    str  r5, [r4, #PROC_R5]
    ldr  r5, [sp, #24]
    str  r5, [r4, #PROC_R6]
    ldr  r5, [sp, #28]
    str  r5, [r4, #PROC_R7]
    ldr  r5, [sp, #32]
    str  r5, [r4, #PROC_R8]
    ldr  r5, [sp, #36]
    str  r5, [r4, #PROC_R9]
    ldr  r5, [sp, #40]
    str  r5, [r4, #PROC_R10]
    ldr  r5, [sp, #44]
    str  r5, [r4, #PROC_R11]
    ldr  r5, [sp, #48]
    str  r5, [r4, #PROC_R12]

    ldr  r5, [sp, #52]
    str  r5, [r4, #PROC_PC]
    str  r5, [r4, #PROC_LR]

    add  r5, sp, #56
    str  r5, [r4, #PROC_SP]

    mrs  r5, cpsr
    str  r5, [r4, #PROC_SPSR]

yield_no_process:
    bl   schedule

    ldr  r4, =CurrProcess
    ldr  r4, [r4]

    ldr  lr, [r4, #PROC_PC]

    ldr  r5, [r4, #PROC_SPSR]
    msr  spsr_cxsf, r5

    ldr  sp, [r4, #PROC_SP]

    add  r4, r4, #PROC_R0
    ldmia r4, {r0-r12}

    movs pc, lr

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

disable_irq:
    mrs r0, cpsr
    orr r0, r0, #0x80
    msr cpsr_c, r0
    bx lr

.section .bss
.align 8

irq_stack:
    .space 4096
_irq_stack_top:
