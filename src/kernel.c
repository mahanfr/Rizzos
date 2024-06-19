#include "../common/uefi_data.h"
#include "basic_graphics.h"
#include <stdint.h>
#include <stdbool.h>
#include "page_frame_allocator.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

uint64_t bloat[6000];
void _start(UEFIBootData* uefiBootData) {
    (void) initBasicGraphics(uefiBootData);
    print("Hello World From Kernel!\n");

    (void) pageFrameInitEfiMemoryMap(
            uefiBootData->mMap,
            uefiBootData->mMapEntries,
            uefiBootData->mMapDescSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)(kernelSize / 4096) + 1;

    pageFrameLockPages(&_KernelStart, kernelPages);

    print("Free RAM: %dKB\n",getFreeMemorySize() / 1024);
    print("Used RAM: %dKB\n",getUsedMemorySize() / 1024);
    print("Reserved RAM: %dKB\n",getReservedMemorySize() / 1024);

    for(int i = 0; i < 10; i++) {
        void* address = pageFrameRequestPage();
        print("address%d: 0x%X\n",i, address);
    }

    return;
}
