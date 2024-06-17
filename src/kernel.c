#include "../common/uefi_data.h"
#include "basic_graphics.h"
#include "uefi_memory.h"
#include <stdint.h>

void _start(UEFIBootData* uefiBootData) {
    initBasicGraphics(uefiBootData);
    print("Hello World From Kernel!\n");

   uint64_t mMapEntries = uefiBootData->mMapSize / uefiBootData->mMapDescSize;

   for(int i = 0; i < mMapEntries; i++) {
       EFIMemoryDescriptor* desc = (EFIMemoryDescriptor*)
           ((uint64_t) uefiBootData->mMap + (i * uefiBootData->mMapDescSize));
       print("%s %dKB\n", uefiMemTypeName(desc->type), (desc->numPages * 4096 / 1024));
   }

    return;
}
