# Proyecto 1 — CC7

[Link a la documentación oficial.](https://www.mintlify.com/Martin-ing/Proyecto1cc7)
Sistema operativo mínimo de bare-metal para procesadores **ARM Cortex-A8**, desarrollado en C y ensamblador ARM. El proyecto puede ejecutarse tanto en hardware real (**BeagleBone**) como en emulación mediante **QEMU**.

---

## Descripción

Este proyecto implementa un sistema operativo mínimo que corre directamente sobre el hardware, sin ningún sistema operativo anfitrión. Incluye:

- **Kernel** escrito en C y ensamblador ARM
- **Gestión de procesos** con múltiples procesos de usuario
- **Biblioteca estándar mínima** (`stdio`, `string`) sin dependencia de libc
- **Soporte dual de plataforma**: BeagleBone Black y QEMU (versatilepb)
- **Configuración de memoria** mediante linker scripts personalizados

---

## Estructura del proyecto

```
Proyecto1cc7/
├── OS/                     # Núcleo del sistema operativo
│   ├── root.s              # Punto de entrada en ensamblador ARM
│   ├── os.c                # Lógica principal del OS
│   ├── process.c           # Gestión de procesos
│   ├── .venv.beagle        # Variables de memoria para BeagleBone
│   └── .venv.qemu          # Variables de memoria para QEMU
├── Lib/                    # Biblioteca estándar mínima
│   ├── stdio.c             # Funciones de I/O (UART)
│   └── string.c            # Funciones de cadenas
├── User/                   # Programas de usuario
│   └── P1/
│       └── main.c          # Proceso de usuario P1
├── linker/                 # Scripts de enlazado
│   ├── linker_beagle.ld    # Mapa de memoria para BeagleBone
│   └── linker_qemu.ld      # Mapa de memoria para QEMU
├── Makefile                # Sistema de build principal
├── build_and_run.sh        # Script automatizado de build y ejecución
└── .gitignore
```

---

## Requisitos

Antes de compilar, asegúrate de tener instaladas las siguientes herramientas:

| Herramienta | Propósito |
|---|---|
| `arm-none-eabi-gcc` | Compilador cruzado ARM |
| `arm-none-eabi-as` | Ensamblador ARM |
| `arm-none-eabi-ld` | Linker ARM |
| `arm-none-eabi-objcopy` | Conversión de formatos binarios |
| `qemu-system-arm` | Emulación ARM (solo para target QEMU) |
| `gdb-multiarch` | Depuración remota (opcional) |

### Instalación en Debian/Ubuntu

```bash
sudo apt update
sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi \
                 qemu-system-arm gdb-multiarch
```

### Instalación en macOS (Homebrew)

```bash
brew install arm-none-eabi-gcc qemu
```

---

## Compilación y ejecución

### Usando el script automatizado (recomendado)

```bash
# Para QEMU
./build_and_run.sh qemu

# Para BeagleBone
./build_and_run.sh beagle
```

### Usando Make

```bash
# Compilar para QEMU
make qemu

# Compilar para BeagleBone
make beagle

# Limpiar archivos generados
make clean
```

Los archivos generados se colocan en el directorio `bin/`:

| Archivo | Descripción |
|---|---|
| `bin/program.elf` | Ejecutable ELF con símbolos de debug |
| `bin/program.bin` | Imagen binaria plana para hardware |
| `bin/program.map` | Mapa de memoria del enlazado |

---

## Plataformas soportadas

### QEMU (`versatilepb`)

El target QEMU compila con flags de depuración (`-O0 -g3`) y lanza automáticamente el emulador. También expone un servidor GDB en el puerto `1234`.

**Depuración con GDB:**

```bash
# En terminal 1: iniciar QEMU con servidor GDB
make qemu

# En terminal 2: conectar GDB
gdb-multiarch bin/program.elf
(gdb) set architecture arm
(gdb) target remote :1234
(gdb) break main
(gdb) continue
```

### BeagleBone Black (`cortex-a8`)

El target BeagleBone compila con optimizaciones (`-O2`) y genera `bin/program.bin`. Para flashear en la placa, usar U-Boot:

```
# En la consola serie de U-Boot
fatload mmc 0 0x82000000 program.bin
go 0x82000000
```

---

## Arquitectura del sistema

```
┌─────────────────────────────────────┐
│           User Space                │
│    P1 (User/P1/main.c)              │
│    P2 (futuras extensiones)         │
├─────────────────────────────────────┤
│           Kernel / OS               │
│  root.s  →  os.c  →  process.c     │
├─────────────────────────────────────┤
│         Biblioteca mínima           │
│       stdio.c   string.c            │
├─────────────────────────────────────┤
│           Hardware ARM              │
│  UART0 · Timer · INTC · CM_PER      │
└─────────────────────────────────────┘
```

El sistema arranca desde `root.s`, que inicializa los registros ARM y el stack, luego transfiere el control al kernel en `os.c`. Los procesos de usuario corren en espacios de memoria separados definidos en los linker scripts.

---

## Configuración de memoria

La configuración de direcciones base se define en los archivos `.venv.*` dentro de `OS/`:

| Variable | Descripción |
|---|---|
| `OS_BASE` | Dirección base del kernel |
| `OS_STACK` | Pila del kernel |
| `P1_BASE` / `P1_STACK` | Memoria del proceso 1 |
| `P2_BASE` / `P2_STACK` | Memoria del proceso 2 |
| `UART0_BASE` | Dirección del periférico UART |
| `TIMER_BASE` | Dirección del temporizador |
| `INTC_BASE` | Controlador de interrupciones |
| `CM_PER_BASE` | Clock Manager Peripheral |
