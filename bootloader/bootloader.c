#include "gnu-efi/inc/efi.h"
#include "gnu-efi/inc/efiapi.h"
#include "gnu-efi/inc/efidef.h"
#include "gnu-efi/inc/efierr.h"
#include "gnu-efi/inc/efilib.h"
#include "gnu-efi/inc/efiprot.h"
#include "gnu-efi/inc/x86_64/efibind.h"

#ifndef LOG
#define LOG(fmt, ...) AsciiPrint(fmt, __VA_ARGS__)
#endif

#ifndef TRACE
#define TRACE(status)   LOG("Status: '%r', Function: '%a', File: '%a', Line: '%d'\r\n", status, __FUNCTION__, __FILE__, __LINE__)
#endif

extern EFI_BOOT_SERVICES *gBS;

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    Print(L"Loading File... \n\r");
    EFI_FILE* LoadedFile;
    EFI_STATUS Status;

    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    // Find the pointer to the image protocol
    Status = uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &gEfiLoadedImageProtocolGuid ,(void**) &LoadedImage);
    // Status = SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);
    // EFI_STATUS li_status = SystemTable->BootServices->OpenProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage,
    //         ImageHandle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    Print(L"Located the Image Protocol \n\r");

    // EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    // // Find the pointer to the file system
    // Status = SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);
    // if (EFI_ERROR(Status)) {
    //     TRACE(Status);
    //     return NULL;
    // }
    // Print(L"Located the File System \n\r");

    // if (Directory == NULL) {
    //     // Open Volume in selected directory
    //     Print(L"Loading Volume to Directory \n\r");
    //     FileSystem->OpenVolume(FileSystem, &Directory);
    //     Print(L"Volume Loaded to Directory \n\r");
    // }

    Print(L"Loading Volume to Directory \n\r");
    Directory = LibOpenRoot(LoadedImage->DeviceHandle);
    if (Directory == NULL) {
        Print(L"WILL CRASH!!! \n\r");
    }

    Print(L"Opening File... \n\r");
    // Open selected file path from Directory and return the status
    // Status = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    Status = uefi_call_wrapper(Directory->Open, 5, Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (EFI_ERROR(Status)) {
        TRACE(Status);
        return NULL;
    }
    return LoadedFile;
}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  InitializeLib(ImageHandle, SystemTable);
  Print(L"Bootloader Started!\n\r");

  EFI_FILE* kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);

  if(kernel == NULL) {
      Print(L"Could not load kernel \n\r");
  }else {
      Print(L"Kernel Loaded Successfuly \n\r");
  }

  return EFI_SUCCESS;
}
