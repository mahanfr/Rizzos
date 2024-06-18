#include "../common/uefi_data.h"
#include "basic_graphics.h"
#include "uefi_memory.h"
#include <stdint.h>
#include "memory.h"

void _start(UEFIBootData* uefiBootData) {
    initBasicGraphics(uefiBootData);
    print("Hello World From Kernel!\n");

    print("Memory size in bytes: %d", 
            getMemorySize(uefiBootData->mMap,
                uefiBootData->mMapEntries,
                uefiBootData->mMapDescSize));

   // for(int i = 0; i < uefiBootData->mMapEntries; i++) {
   //     EFIMemoryDescriptor* desc = (EFIMemoryDescriptor*)
   //         ((uint64_t) uefiBootData->mMap + (i * uefiBootData->mMapDescSize));
   //     print("%s %dKB\n", uefiMemTypeName(desc->type), (desc->numPages * 4096 / 1024));
   // }

   return;
}
