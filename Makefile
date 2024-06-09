ASM = nasm
CC = gcc

BOOTLOADER_SRC = ./bootloader/bios/boot.asm
BUILD_DIR = build
BOOTLOADER = boot.bin
SRC_DIR = src
KERNEL = kernel.bin
IMG= rizzos.img

.PHONY: bootloader kernel image always clean
default: image
#
# Bootloader
#
bootloader: $(BUILD_DIR)/$(BOOTLOADER)

$(BUILD_DIR)/$(BOOTLOADER): always
	$(ASM) -fbin $(BOOTLOADER_SRC) -o $@ 

#
# Kernel
#
kernal: $(BUILD_DIR)/$(KERNEL)

$(BUILD_DIR)/$(KERNEL): always
	$(ASM) -fbin $(SRC_DIR)/kernel.asm -o $@

#
# Bootable Image
#
image: $(BUILD_DIR)/$(IMG)

$(BUILD_DIR)/$(IMG): bootloader kernal
	dd if=/dev/zero of=$@ bs=4096 count=10000
	/sbin/mkfs.fat -F 32 -n "NBOS" $@
	dd if=$(BUILD_DIR)/$(BOOTLOADER) of=$@ conv=notrunc
	mcopy -i $@ $(BUILD_DIR)/$(KERNEL) "::kernel.bin"

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/* 
