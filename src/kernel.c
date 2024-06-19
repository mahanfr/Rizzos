#include "../common/uefi_data.h"
#include "graphics/basic_graphics.h"
#include <stdint.h>
#include <stdbool.h>
#include "paging/page_frame_allocator.h"
#include "paging/page_map_indexer.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

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

    PageMapIndexer* pageIndexer = pageMapIndexer(0x1000);
    print("%X - %X - %X - %X\n", pageIndexer->P_i, pageIndexer->PT_i, pageIndexer->PD_i, pageIndexer->PDP_i);

    return;
}
