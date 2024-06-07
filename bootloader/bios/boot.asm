org 0x00
bits 16

;--------------------------------------------------
; FAT32 Init SECTION
;--------------------------------------------------
jmp short start
nop
OEM_ID:             db "mkfs.fat"
BytesPerSector:     dw 0x0200
SectorsPerCluster:  db 0x01
ReservedSectors:    dw 0x0020
TotalFATs:          db 0x02
MaxRootEntries:     dw 0x0000
NumberOfSectors:    dw 0x0000
MediaDescriptor:    db 0xF8
SectorsPerFAT:      dw 0x0000
SectorsPerTrack:    dw 0x0020
SectorsPerHead:     dw 0x0008
HiddenSectors:      dd 0x00000000
TotalSectors:       dd 0x00013880
BigSectorsPerFAT:   dd 0x00000268
Flags:              dw 0x0000
FSVersion:          dw 0x0000
RootDirectoryStart: dd 0x00000002
FSInfoSector:       dw 0x0001
BackupBootSector:   dw 0x0006

TIMES 12 db 0 ;jumping to next offset

DriveNumber:        db 0x80
ReservedByte:       db 0x00
Signature:          db 0x29
VolumeID:           dd 0xFFFFFFFF
VolumeLabel:        db "OSMIUM BOOT"
SystemID:           db "FAT32   "
;--------------------------------------------------
; FAT32 Init SECTION
;--------------------------------------------------

start: ;Entry Point
    cli
    mov ax, 0x07C0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ax, 0x00
    mov ss, ax
    mov sp, 0xFFFF
    sti

    mov si, msgHello
    call puts

    ; Size of a cluser
    mov cx, word [SectorsPerCluster]
    ; location of data begin
    mov al, byte [TotalFATs]
    mul word[BigSectorsPerFAT]
    add ax, word [ReservedSectors]
    mov word[datasector], ax
    ; Read first cluster
    mov ax, word [RootDirectoryStart]
    call ClusterLBA
    mov bx, 0x200
    call ReadSectors
    ; point Index register to first file entry
    mov di, 0x0200 + 0x20
    
    mov si, msgCRLF
    call puts
    ; point to location info offset
    mov dx, word [di + 0x001A]
    mov word [cluster], dx

    ; set up kernel loading segments
    mov ax, 0100h
    mov es, ax
    mov bx, 0
    ; read the kernel cluster
    mov cx, 0x0008
    mov ax, word [cluster]
    call ClusterLBA 
    call ReadSectors
   
    mov si, msgCRLF
    call puts

    ; jump to kernel location
    push word 0x0100
    push word 0x0000
    retf

    ; in case of an error
    mov si, msgFailure
    call puts
    mov ah, 0
    int 0x16
    int 0x19

puts: ;print si register characters to BIOS TTY
    push si
    push ax
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x07
    int 0x10
    jmp .loop
.done:
    pop ax
    pop si
    ret

ReadSectors: ;Read Sectors
.main:
    mov di, 0x05
.sector_loop:
    push ax
    push bx
    push cx
    call LBACHS
    mov ah, 0x2
    mov al, 0x1
    mov ch, byte [absoluteTrack]
    mov cl, byte [absoluteSector]
    mov dh, byte [absoluteHead]
    mov dl, byte [DriveNumber]
    int 0x13
    jnc .success
    xor ax, ax
    int 0x13
    dec di
    pop cx
    pop bx
    pop ax
    jnz .sector_loop
    int 0x18
.success:
    mov si, msgProgress
    call puts
    pop cx
    pop bx
    pop ax
    add bx, word [BytesPerSector]
    inc ax
    loop .main
    ret

ClusterLBA: ;convert fat cluster to lba addressing scheme
    sub ax, 0x0002
    xor cx, cx
    mov cl, byte [SectorsPerCluster]
    mul cx
    add ax, word [datasector]
    ret

LBACHS: ;convert LBA to CHS addressing scheme
    xor dx, dx
    div word [SectorsPerTrack]
    inc dl
    mov byte [absoluteSector], dl
    xor dx, dx
    div word [SectorsPerHead]
    mov byte [absoluteHead], dl
    mov byte [absoluteTrack], al
    ret

; variables
absoluteSector: db 0x00
absoluteHead:   db 0x00
absoluteTrack:  db 0x00
datasector:     dw 0x00
cluster:        dw 0x00

; readonly data
msgHello:    db "hello world!!", 0x0D, 0x0A, 0
msgProgress: db ".", 0x00
msgFailure:  db 0x0D, 0x0A, "Kernel loading failed...", 0x0D, 0x0A, 0x00
msgCRLF:     db 0x0D, 0x0A, 0x00

; bootloader signeture
times 510 - ($-$$) db 0
dw 0xAA55
