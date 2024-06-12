#!/bin/sh

set -xe

qemu-system-x86_64 -cpu qemu64 \
    -drive if=pflash,format=raw,unit=0,file=./OVMFbin/OVMF_CODE-pure-efi.fd,readonly=on \
    -drive if=pflash,format=raw,unit=1,file=./OVMFbin/OVMF_VARS-pure-efi.fd \
    -net none
