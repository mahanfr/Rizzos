#include "gnu-efi/inc/efi.h"
#include "gnu-efi/inc/efidef.h"
#include "gnu-efi/inc/efilib.h"
#include "gnu-efi/inc/x86_64/efibind.h"
#include <elf.h>

#ifndef LOG
#define LOG(fmt, ...) AsciiPrint(fmt, __VA_ARGS__)
#endif

#ifndef TRACE
#define TRACE(status)   LOG("Status: '%r', Function: '%a', File: '%a', Line: '%d'\r\n", status, __FUNCTION__, __FILE__, __LINE__)
#endif

typedef unsigned long long Size_t;

extern EFI_BOOT_SERVICES *gBS;

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_FILE* LoadedFile;
    EFI_STATUS Status;

    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    // Find the pointer to the image protocol
    Status = uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &gEfiLoadedImageProtocolGuid ,(void**) &LoadedImage);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }

    Print(L"Loading Volume to Directory \n\r");
    Directory = LibOpenRoot(LoadedImage->DeviceHandle);
    if (Directory == NULL) {
        Print(L"Cannot Open Root Directory! \n\r");
    }

    // Open selected file path from Directory and return the status
    Status = uefi_call_wrapper(Directory->Open, 5, Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    return LoadedFile;
}

int MemCmp(const void* aptr, const void* bptr, Size_t n) {
    const unsigned char *a = aptr, *b = bptr;
    for(Size_t i=0; i<n; i++) {
        if (a[i] < b[i]) return -1;
        else if (a[i] > b[i]) return 1;
    }
    return 0;
}

BOOLEAN IsElfFHeader(Elf64_Ehdr header) {
    return MemCmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) == 0
        && header.e_ident[EI_CLASS] == ELFCLASS64
        && header.e_ident[EI_DATA] == ELFDATA2LSB
        && header.e_type == ET_EXEC
        && header.e_machine == EM_X86_64
        && header.e_version == EV_CURRENT;
}

Elf64_Ehdr* ReadElfHeader(EFI_FILE* file) {
    Elf64_Ehdr* header;
    EFI_STATUS Status;

    EFI_FILE_INFO* FileInfo = LibFileInfo(file);
    if (FileInfo == NULL) {
        Print(L"Failed to fetch file info!\n\r");
        return NULL;
    }

    UINTN size = sizeof(*header);
    Status = uefi_call_wrapper(file->Read, 3, file, &size, header);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    return header;
}

Elf64_Phdr* ReadElfProgramHeader(EFI_FILE *kernel, Elf64_Ehdr* header) {
    Elf64_Phdr* phdrs;
    EFI_STATUS Status;
    Status = uefi_call_wrapper(kernel->SetPosition, 2, kernel, header->e_phoff);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    UINTN size = header->e_phnum * header->e_phentsize;
    Status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, size, (void**)&phdrs);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    Status = uefi_call_wrapper(kernel->Read, 3, kernel, &size, phdrs);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    return phdrs;

}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Bootloader Started!\n\r");

    EFI_FILE* kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);

    if(kernel == NULL) {
        Print(L"Could not load kernel \n\r");
    }else {
        Print(L"Kernel File Loaded Successfuly \n\r");
    }

    // Elf64_Ehdr* header = ReadElfHeader(kernel);
    Elf64_Ehdr* header;
    EFI_STATUS Status;

    EFI_FILE_INFO* FileInfo = LibFileInfo(kernel);
    if (FileInfo == NULL) {
        Print(L"Failed to fetch file info!\n\r");
    }

    UINTN size = sizeof(*header);
    Status = uefi_call_wrapper(kernel->Read, 3, kernel, &size, header);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
    }
    if(header == NULL) {
        Print(L"Can not read the elf headers!\n\r");
    }
    if (IsElfFHeader(*header)) {
        Print(L"kernel header successfully verified\r\n");
    } else {
        Print(L"kernel format is bad\r\n");
    }
    // Status = uefi_call_wrapper(kernel->SetPosition, 2, kernel, header->e_phoff);
    // if (EFI_ERROR(Status)) {
    //     TRACE(Status);
    //     return NULL;
    // }
    Elf64_Phdr* phdrs = ReadElfProgramHeader(kernel, header);
    if (phdrs == NULL) {
        Print(L"Can not read the elf program headers!\n\r");
    }
    for(Elf64_Phdr* phdr = phdrs;
            (char*)phdr < (char*)phdrs + header->e_phnum * header->e_phentsize;
            phdr = (Elf64_Phdr*)((char*)phdr + header->e_phentsize)) {
        switch (phdr->p_type) {
            case PT_LOAD:
                {
                    int pages = (phdr->p_memsz + 8 - 1)/ 8;
                    Elf64_Addr segment = phdr->p_paddr;
                    uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, pages, &segment);
                    uefi_call_wrapper(kernel->SetPosition, 2, kernel, phdr->p_offset);
                    UINTN size = phdr->p_filesz;
                    uefi_call_wrapper(kernel->Read, 3, kernel, &size, (void*)segment);
                    break;
                }
        }
    }
    Print(L"Kernel Executables Loaded into memory!\n\r");

    int (*KernelStart)() = ((__attribute__((sysv_abi)) int (*)() ) header->e_entry);

    Print(L"%d\r\n", KernelStart());
    return EFI_SUCCESS;
}
