CC = gcc
ASM = nasm
LD = ld
MKFS = /sbin/mkfs.vfat

LDS = rizzos/kernel.ld

CFLAGS = -ffreestanding -fshort-wchar -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
ASMFLAGS =
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRC_DIR = ./rizzos/src
BUILD_DIR = ./build
TARGET = $(BUILD_DIR)/rizzos.img
KERNEL = $(BUILD_DIR)/kernel.elf
ASSETS = ./assets/zap-light16.psf

# finding files by extention
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRC_DIR),*.c)
ASMSRC = $(call rwildcard,$(SRC_DIR),*.asm)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRC_DIR)/*)

all: target-img

bootloader: always
	$(MAKE) -C ./bootloader

kernel: always $(OBJS) link

$(BUILD_DIR)/interrupts/interrupts.o: $(SRC_DIR)/interrupts/interrupts.c
	@ echo CC $^
	mkdir -p $(@D)
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@

$(BUILD_DIR)/%_asm.o: $(SRC_DIR)/%.asm
	@ echo ASM $^
	mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $^ -felf64 -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@ echo CC $^
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

link:
	@ echo Linking object files..
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

base-img:
ifeq (,$(wildcard $(TARGET)))
	dd if=/dev/zero of=$^ bs=512 count=93750
endif

target-img: base-img bootloader kernel
	$(MKFS) $(TARGET)
	mmd -i $(TARGET) ::/EFI
	mmd -i $(TARGET) ::/EFI/BOOT
	mcopy -i $(TARGET) ./bootloader/build/bootx64.efi ::/EFI/BOOT
	mcopy -i $(TARGET) ./bootloader/startup.nsh ::
	mcopy -i $(TARGET) $(KERNEL) ::
	mcopy -i $(TARGET) $(ASSETS) ::
	@ echo DONE. CREATED $(TARGET)

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
