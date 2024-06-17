#include "gnu-efi/inc/efi.h"
#include "gnu-efi/inc/efidef.h"
#include "gnu-efi/inc/efilib.h"
#include <elf.h>
#include "../common/graphics.h"
#include "../common/fonts.h"
#include "../common/uefi_data.h"
#include "gnu-efi/inc/x86_64/efibind.h"

#ifndef LOG
#define LOG(fmt, ...) AsciiPrint(fmt, __VA_ARGS__)
#endif

#ifndef TRACE
#define TRACE(status)   LOG("Status: '%r', Function: '%a', File: '%a', Line: '%d'\r\n", status, __FUNCTION__, __FILE__, __LINE__)
#endif

FrameBuffer frameBuffer;
FrameBuffer* InitializeGOP() {
    EFI_STATUS Status;
    EFI_GUID gopGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

    Status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGUID, NULL, (void**) &gop);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    } else {
        Print(L"GOP Located sucessfully!\n\r");
    }
    frameBuffer.BaseAddress = (void*) gop->Mode->FrameBufferBase;
    frameBuffer.BufferSize = gop->Mode->FrameBufferSize;
    frameBuffer.Width = gop->Mode->Info->HorizontalResolution;
    frameBuffer.Height = gop->Mode->Info->VerticalResolution;
    frameBuffer.PixelPerScanLine = gop->Mode->Info->PixelsPerScanLine;

    return &frameBuffer;
}

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

PSF1_FONT* LoadPSF1Font(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS Status;
    EFI_FILE* fontFile = LoadFile(Directory, Path, ImageHandle, SystemTable);
    if (fontFile == NULL) return NULL;

    PSF1_HEADER* fontHeader;
    Status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    UINTN size = sizeof(PSF1_HEADER);
    Status = uefi_call_wrapper(fontFile->Read, 3, fontFile, &size, fontHeader);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }

    if(fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1) {
        return NULL;
    }
    UINTN glyphBufferSize = fontHeader->charsize * 256;
    if (fontHeader->mode == 1) {
        glyphBufferSize = fontHeader->charsize * 512;
    }

    void* glyphBuffer;
    {
        Status = uefi_call_wrapper(fontFile->SetPosition, 2, fontFile, sizeof(PSF1_HEADER));
        if (EFI_ERROR(Status)) {
            TRACE(Status);
            return NULL;
        }
        Status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, glyphBufferSize, (void**) &glyphBuffer);
        if (EFI_ERROR(Status)) {
            TRACE(Status);
            return NULL;
        }
        Status = uefi_call_wrapper(fontFile->Read, 3, fontFile, &glyphBufferSize, glyphBuffer);
        if (EFI_ERROR(Status)) {
            TRACE(Status);
            return NULL;
        }
    }

    PSF1_FONT* font;
    Status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(PSF1_FONT), (void**) &font);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    font->psfHeader = fontHeader;
    font->glyphBuffer = glyphBuffer;
    return font;
}

int MemCmp(const void* aptr, const void* bptr, size_t n) {
    const unsigned char *a = aptr, *b = bptr;
    for(size_t i=0; i<n; i++) {
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

    FrameBuffer *new_FrameBuf = InitializeGOP();
    if (new_FrameBuf == NULL) return EFI_ERROR(24);
    PSF1_FONT *newFont = LoadPSF1Font(NULL, L"zap-light16.psf", ImageHandle, SystemTable);
    if (newFont == NULL) {
        Print(L"Font not Found!\n\r");
    } else {
        Print(L"Font Found: char size = %d\n\r", newFont->psfHeader->charsize);
    }

    EFI_MEMORY_DESCRIPTOR* map = NULL;
    UINTN mapSize, mapKey;
    UINTN descriptorSize;
    UINT32 descriptorVersion;

    (void) uefi_call_wrapper(BS->GetMemoryMap, 5, &mapSize, map, &mapKey, &descriptorSize, &descriptorVersion);
    Status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, mapSize, (void**)&map);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
    }
    Status = uefi_call_wrapper(BS->GetMemoryMap, 5, &mapSize, map, &mapKey, &descriptorSize, &descriptorVersion);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
    }

    void (*KernelStart)(UEFIBootData*) = ((__attribute__((sysv_abi)) void (*)(UEFIBootData*) ) header->e_entry);

    UEFIBootData* uefiBootData;
    uefiBootData->frameBuffer = new_FrameBuf;
    uefiBootData->consoleFont = newFont;
    uefiBootData->mMap = map;
    uefiBootData->mMapSize = mapSize;
    uefiBootData->mMapDescSize = descriptorSize;

    Status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, mapKey);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
    }

    KernelStart(uefiBootData);
    return EFI_SUCCESS;
}
