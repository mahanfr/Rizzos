ASM = nasm
CC = gcc

BOOTLOADER_SRC = ./bootloader/bios/boot.asm
BUILD_DIR = build
BOOTLOADER = boot.bin

.PHONY: run always clean
default: run

$(BUILD_DIR)/$(BOOTLOADER): $(BOOTLOADER_SRC) | always
	$(ASM) -fbin $^ -o $@ 

$(BUILD_DIR)/floppy.img: $(BUILD_DIR)/$(BOOTLOADER)
	dd if=/dev/zero of=$@ bs=1024 count=1440
	dd if=$^ of=$@ seek=0 count=1 conv=notrunc

run: $(BUILD_DIR)/floppy.img
	qemu-system-i386 -fda $^

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/* 
