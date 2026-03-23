CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
LD      = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

PROGRAM_DIR = User
OS_DIR      = OS
LIB_DIR     = Lib
BIN_DIR     = bin

name   = program
TARGET ?= beagle

ELF = $(BIN_DIR)/$(name).elf
BIN = $(BIN_DIR)/$(name).bin

ASM_SRC = OS/root.s
C_SRC   = $(wildcard $(PROGRAM_DIR)/**/*.c) \
           $(wildcard $(OS_DIR)/*.c) \
           $(wildcard $(LIB_DIR)/*.c)

ASM_OBJ = $(BIN_DIR)/root.o
C_OBJ   = $(C_SRC:%.c=$(BIN_DIR)/%.o)

# Cargar el .venv según el target
ifeq ($(TARGET), qemu)
    include $(OS_DIR)/.venv.qemu
else
    include $(OS_DIR)/.venv.beagle
endif
PLATFORM_FLAGS = -DPLATFORM_TARGET=$(PLATFORM_TARGET) \
                 -DPLATFORM_UART0_BASE=$(UART0_BASE) \
                 -DPLATFORM_TIMER_BASE=$(TIMER_BASE) \
                 -DPLATFORM_INTC_BASE=$(INTC_BASE) \
                 -DPLATFORM_CM_PER_BASE=$(CM_PER_BASE) \
                 -DPLATFORM_OS_BASE=$(OS_BASE) \
                 -DPLATFORM_OS_STACK=$(OS_STACK) \
				 -DP1_BASE=$(P1_BASE) \
				 -DP1_STACK=$(P1_STACK) 

BASE_CFLAGS = -mcpu=cortex-a8 \
              -mfpu=neon \
              -mfloat-abi=hard \
              -Wall \
              -nostdlib -nostartfiles -ffreestanding \
              -DOS -DLIBRARY \
              -I$(OS_DIR) -I$(LIB_DIR)

BASE_ASFLAGS = --warn --fatal-warnings

ifeq ($(TARGET), qemu)
    CFLAGS   = $(BASE_CFLAGS) -O0 -g3 $(PLATFORM_FLAGS)
    ASFLAGS  = $(BASE_ASFLAGS) -g
    LDFLAGS  = -T linker_qemu.ld
else
    CFLAGS   = $(BASE_CFLAGS) -O2 $(PLATFORM_FLAGS)
    ASFLAGS  = $(BASE_ASFLAGS)
    LDFLAGS  = -T linker_beagle.ld
endif

all: $(BIN)

$(ASM_OBJ): $(ASM_SRC)
	@mkdir -p $(BIN_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(ASM_OBJ) $(C_OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

$(BIN): $(ELF)
	$(OBJCOPY) $< -O binary $@

beagle: $(BIN)

qemu: TARGET = qemu
qemu: $(BIN)
	@echo ""
	@echo ">>> Lanzando QEMU con GDB server en puerto 1234..."
	@echo ">>> En otra terminal corre:"
	@echo ">>> gdb-multiarch $(ELF)"
	@echo ">>> set architecture arm"
	@echo ">>> target remote :1234"
	@echo ">>> file bin/program.elf"
	@echo ">>> break main"
	@echo ">>> continue"
	@echo ""
	qemu-system-arm \
	    -M versatilepb \
	    -cpu cortex-a8 \
	    -kernel $(ELF) \
	    -nographic \
	    -S -s

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean beagle qemu