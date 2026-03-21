#!/bin/bash
set -e

# ============================================================
# Build and Run Script
# Uso:
#   ./build_and_run.sh beagle
#   ./build_and_run.sh qemu
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

TARGET="${1:-qemu}"

# ============================================================
# Cargar configuración según plataforma
# ============================================================
case "$TARGET" in
    beagle)
        ENV_FILE="OS/.venv.beagle"
        PLATFORM_DEFINE="-DPLATFORM_BEAGLE"
        LINKER_SCRIPT="linker_beagle.ld"
        ;;
    qemu)
        ENV_FILE="OS/.venv.qemu"
        PLATFORM_DEFINE="-DPLATFORM_QEMU"
        LINKER_SCRIPT="linker_qemu.ld"
        ;;
esac

if [ ! -f "$ENV_FILE" ]; then
    echo "Error: no se encontró $ENV_FILE"
    exit 1
fi

echo "Cargando configuración desde $ENV_FILE ..."
set -a
source "$ENV_FILE"
set +a

# ============================================================
# Verificación mínima de variables
# ============================================================
: "${OS_BASE:?Falta OS_BASE en $ENV_FILE}"
: "${P1_BASE:?Falta P1_BASE en $ENV_FILE}"
: "${P1_STACK:?Falta P1_STACK en $ENV_FILE}"
: "${P2_BASE:?Falta P2_BASE en $ENV_FILE}"
: "${P2_STACK:?Falta P2_STACK en $ENV_FILE}"
: "${UART0_BASE:?Falta UART0_BASE en $ENV_FILE}"

TIMER_BASE="${TIMER_BASE:-0x0}"
INTC_BASE="${INTC_BASE:-0x0}"
CM_PER_BASE="${CM_PER_BASE:-0x0}"
QEMU_MACHINE="${QEMU_MACHINE:-beagle}"

# ============================================================
# Herramientas
# ============================================================
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

# ============================================================
# Flags
# ============================================================
COMMON_FLAGS=(
    -mcpu=cortex-a8
    -marm
    -mfpu=neon
    -mfloat-abi=hard
    -ffreestanding
    -fno-builtin
    -fno-stack-protector
    -nostdlib
    -nostartfiles
    -Wall
    -O2
    -I OS
    -I Lib
    -I User
    "$PLATFORM_DEFINE"
    -DOS_BASE="$OS_BASE"
    -DP1_BASE="$P1_BASE"
    -DP1_STACK="$P1_STACK"
    -DP2_BASE="$P2_BASE"
    -DP2_STACK="$P2_STACK"
    -DPLATFORM_UART0_BASE="$UART0_BASE"
    -DPLATFORM_TIMER_BASE="$TIMER_BASE"
    -DPLATFORM_INTC_BASE="$INTC_BASE"
    -DPLATFORM_CM_PER_BASE="$CM_PER_BASE"
)

# ============================================================
# Limpieza
# ============================================================
echo "Cleaning previous build..."
mkdir -p bin
rm -f bin/*.o bin/*.elf bin/*.bin bin/*.map

# ============================================================
# Compilación
# ============================================================
echo "Assembling OS/root.s..."
$CC -x assembler-with-cpp -c "${COMMON_FLAGS[@]}" \
    -o bin/root.o OS/root.s

echo "Compiling OS/os.c..."
$CC -c "${COMMON_FLAGS[@]}" \
    -o bin/os.o OS/os.c

echo "Compiling OS/process.c..."
$CC -c "${COMMON_FLAGS[@]}" \
    -o bin/process.o OS/process.c

echo "Compiling Lib/stdio.c..."
$CC -c "${COMMON_FLAGS[@]}" \
    -o bin/stdio.o Lib/stdio.c

echo "Compiling Lib/string.c..."
$CC -c "${COMMON_FLAGS[@]}" \
    -o bin/string.o Lib/string.c

echo "Compiling User/main.c..."
$CC -c "${COMMON_FLAGS[@]}" \
    -o bin/main.o User/main.c


# ============================================================
# Link
# ============================================================
if [ ! -f "$LINKER_SCRIPT" ]; then
    echo "Error: no se encontró $LINKER_SCRIPT"
    exit 1
fi

echo "Linking..."
$CC "${COMMON_FLAGS[@]}" -T "$LINKER_SCRIPT" \
    -Wl,-Map,bin/program.map \
    -o bin/program.elf \
    bin/root.o \
    bin/os.o \
    bin/process.o \
    bin/stdio.o \
    bin/string.o \
    bin/main.o

# ============================================================
# BIN
# ============================================================
echo "Generating binary..."
$OBJCOPY -O binary bin/program.elf bin/program.bin

echo "Build complete."
echo "  ELF : bin/program.elf"
echo "  BIN : bin/program.bin"
echo "  MAP : bin/program.map"

# ============================================================
# Run
# ============================================================
if [ "$TARGET" = "qemu" ]; then
    echo "Running in QEMU..."
    qemu-system-arm -M versatilepb -nographic -kernel bin/program.elf
else
    echo "Target = beagle"
    echo "No se ejecuta automáticamente."
    echo "Carga bin/program.bin o bin/program.elf en la Beagle con U-Boot."
fi