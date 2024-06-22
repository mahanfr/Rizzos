CC = gcc
SRC = bootloader.c
BUILD_DIR = build
OBJFILE = $(BUILD_DIR)/main.o
SOFILE = $(BUILD_DIR)/bootx64.so
TARGET = $(BUILD_DIR)/bootx64.efi

LDINC = -Lgnu-efi/x86_64/lib -Lgnu-efi/x86_64/gnuefi
ELF_LDS = gnu-efi/gnuefi/elf_x86_64_efi.lds
CRT_OBJ = gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o 

ifeq (,$(wildcard ./gnu-efi))
	$(error please run: git submodules init & git submodule update)
endif

ifeq (,$(wildcard $(ELF_LDS)))
	$(info building gnu-efi module)
	$(MAKE) -C gnu-efi
endif

all: $(TARGET)

$(OBJFILE): always
	$(CC) -Ignu-efi/inc -fpic -ffreestanding -fno-stack-protector \
		-fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args \
		-c $(SRC) -o $@

$(SOFILE): $(OBJFILE)
	ld -shared -Bsymbolic \
		$(LDINC) \
		-T$(ELF_LDS) \
		$(CRT_OBJ) \
		$^ -o $@ -lgnuefi -lefi

$(TARGET): $(SOFILE)
	objcopy -j .text -j .sdata -j .data -j .rodata \
		-j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* \
		-j .rela.* -j .reloc --target efi-app-x86_64 \
		--subsystem=10 $^ $@

always:
	mkdir -p $(BUILD_DIR)

