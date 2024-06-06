org 0x7c00
bits 16

start:
    jmp main

puts:
    push si
    push ax
.loop:
    lodsb
    or al, al
    jz .done

    mov ah, 0x0e
    mov bh, 0
    int 0x10

    jmp .loop
.done:
    pop ax
    pop si
    ret

main:
    mov ax, 0
    mov ds, ax
    mov es, ax

    mov ss, ax
    mov sp, 0x7c00

    mov si, hello
    call puts
    hlt

.halt:
    jmp .halt

hello: 
    db "hello world!!", 0x0D, 0x0A, 0

times 510 - ($-$$) db 0

dw 0xAA55
