CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
LD      = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

PROGRAM_DIR = program
OS_DIR      = OS
LIB_DIR     = library
BIN_DIR     = bin

name   ?= hello
TARGET ?= beagle

ELF = $(BIN_DIR)/$(name).elf
BIN = $(BIN_DIR)/$(name).bin

ASM_SRC = OS/root.s
C_SRC   = $(wildcard $(PROGRAM_DIR)/*.c) \
           $(wildcard $(OS_DIR)/*.c) \
           $(wildcard $(LIB_DIR)/*.c)

ASM_OBJ = $(BIN_DIR)/root.o
C_OBJ   = $(C_SRC:%.c=$(BIN_DIR)/%.o)

BASE_CFLAGS = -mcpu=cortex-a8 \
              -mfpu=neon \
              -mfloat-abi=hard \
              -Wall \
              -nostdlib -nostartfiles -ffreestanding \
              -DOS -DLIBRARY \
              -I$(OS_DIR) -I$(LIB_DIR)

BASE_ASFLAGS = --warn --fatal-warnings

ifeq ($(TARGET), qemu)
    CFLAGS   = $(BASE_CFLAGS) -O0 -g3 -DQEMU
    ASFLAGS  = $(BASE_ASFLAGS) -g
    LDFLAGS  = -T linker.ld
else
    CFLAGS   = $(BASE_CFLAGS) -O2
    ASFLAGS  = $(BASE_ASFLAGS)
    LDFLAGS  = -T linker.ld
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
	@echo ">>>   arm-none-eabi-gdb $(ELF)"
	@echo ">>>   (gdb) target remote :1234"
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
