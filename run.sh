#!/bin/sh
set -xe

# qemu-system-i386 -fda ./build/rizzos.img
qemu-system-x86_64 -cpu qemu64 \
    -drive if=pflash,format=raw,unit=0,file=./bootloader/uefi/OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on \
    -drive if=pflash,format=raw,unit=1,file=./bootloader/uefi/OVMFbin/OVMF_VARS-pure-efi.fd \
    -drive file=./build/uefi.img \
    -net none
