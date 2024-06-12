
PARTED = /usr/sbin/parted
BUILD_DIR = ./build
TEMP_PART = $(BUILD_DIR)/part.img
TARGET = $(BUILD_DIR)/uefi.img

bootloader: image

partitions: disk-image
	$(PARTED) $(TARGET) -s -a minimal mklabel gpt
	$(PARTED) $(TARGET) -s -a minimal mkpart EFI FAT16 2048s 93716s
	$(PARTED) $(TARGET) -s -a minimal toggle 1 boot

temp-partitions: partitions
	dd if=/dev/zero of=$(TEMP_PART) bs=512 count=91669
	mformat -i $(TEMP_PART) -h 32 -t 32 -n 64 -c 1
	mmd -i $(TEMP_PART) ::/EFI
	mmd -i $(TEMP_PART) ::/EFI/BOOT

CP-bootlaoder: temp-partitions
	$(MAKE) -C ./bootloader
	mcopy -i $(TEMP_PART) ./bootloader/build/main.efi ::/EFI/BOOT
	mcopy -i $(TEMP_PART) ./bootloader/startup.nsh ::

image: CP-bootlaoder
	dd if=$(TEMP_PART) of=$(TARGET) bs=512 count=91669 seek=2048 conv=notrunc

disk-image:
	dd if=/dev/zero of=$(TARGET) bs=512 count=93750

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/* 
