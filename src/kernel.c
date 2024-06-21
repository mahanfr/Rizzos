#include "../common/uefi_data.h"
#include "graphics/basic_graphics.h"
#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "paging/page_frame_allocator.h"
#include "paging/page_table_manager.h"
#include "paging/paging.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void _start(UEFIBootData* uefiBootData) {
    (void) initBasicGraphics(uefiBootData);
    setBgColor(0xFF02242b);
    clearBackground();
    print("Hello World From Kernel!\n");

    // Init PageFrame Memory Mapping
    (void) pageFrameInitEfiMemoryMap(
            uefiBootData->mMap,
            uefiBootData->mMapEntries,
            uefiBootData->mMapDescSize);

    // Locking Kernels location in memory
    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)(kernelSize / MEM_FRAME_SIZE) + 1;
    pageFrameLockPages(&_KernelStart, kernelPages);
    pageFrameLockPages(uefiBootData->frameBuffer->BaseAddress, uefiBootData->frameBuffer->BufferSize / MEM_FRAME_SIZE + 1);

    // Create a PageTableManager
    PageTable* PML4 = (PageTable*) pageFrameRequestPage();
    memSet(PML4, 0, MEM_FRAME_SIZE);
    PageTableManager pageTableManager = pageTableManager_Create(PML4);

    // Map All the Pages of Memory to thire Virtual Memory
    uint64_t memorySize = getMemorySize(uefiBootData->mMap, uefiBootData->mMapEntries, uefiBootData->mMapDescSize);
    for(uint64_t i = 0; i < memorySize; i+= MEM_FRAME_SIZE) {
        pageTableManager_MapMemory(&pageTableManager, (void*)i, (void*)i);
    }
    // Map All the Pages of Memory to thire Virtual Memory
    for(uint64_t i = (uint64_t) uefiBootData->frameBuffer->BaseAddress;
            i < (uint64_t)uefiBootData->frameBuffer->BaseAddress + uefiBootData->frameBuffer->BufferSize;
            i+= MEM_FRAME_SIZE) {
        pageTableManager_MapMemory(&pageTableManager, (void*)i, (void*)i);
    }

    asm("mov %0, %%cr3" :: "r" (PML4));

    print("Kernel Initialized.\n");
    while(true);
}
