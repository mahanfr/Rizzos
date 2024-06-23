#include "../common/uefi_data.h"
#include "gdt/gdt.h"
#include "graphics/basic_graphics.h"
#include <stdint.h>
#include <stdbool.h>
#include "interrupts/idt.h"
#include "interrupts/interrupts.h"
#include "memory.h"
#include "paging/page_frame_allocator.h"
#include "paging/page_table_manager.h"
#include "paging/paging.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;
static IDTR idtr;

static void InitializeMemory(UEFIBootData* uefiBootData) {
    // Init PageFrame Memory Mapping
    (void) PFA_InitEfiMemoryMap(
            uefiBootData->mMap,
            uefiBootData->mMapEntries,
            uefiBootData->mMapDescSize);

    // Locking Kernels location in memory
    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)(kernelSize / MEM_FRAME_SIZE) + 1;
    PFA_LockPages(&_KernelStart, kernelPages);
    PFA_LockPages(uefiBootData->frameBuffer->baseAddress, uefiBootData->frameBuffer->bufferSize / MEM_FRAME_SIZE + 1);

    // Create a PageTableManager
    PageTable* PML4 = (PageTable*) PFA_RequestPage();
    MEM_Set64(PML4, 0, MEM_FRAME_SIZE);
    PageTableManager pageTableManager = PTM_Create(PML4);

    // Map All the Pages of Memory to thire Virtual Memory
    uint64_t memorySize = MEM_GetTotalSize(uefiBootData->mMap, uefiBootData->mMapEntries, uefiBootData->mMapDescSize);
    for(uint64_t i = 0; i < memorySize; i+= MEM_FRAME_SIZE) {
        PTM_MapMemory(&pageTableManager, (void*)i, (void*)i);
    }
    // Map All the Pages of Memory to thire Virtual Memory
    for(uint64_t i = (uint64_t) uefiBootData->frameBuffer->baseAddress;
            i < (uint64_t)uefiBootData->frameBuffer->baseAddress + uefiBootData->frameBuffer->bufferSize;
            i+= MEM_FRAME_SIZE) {
        PTM_MapMemory(&pageTableManager, (void*)i, (void*)i);
    }

    asm("mov %0, %%cr3" :: "r" (PML4));
}

static void InitializeInterrupts(void) {
    idtr.size = 0x0FFF;
    idtr.offset = (uint64_t) PFA_RequestPage();

    IDTDescEntry* int_pagefault = (IDTDescEntry*)(idtr.offset + 0xE * sizeof(IDTDescEntry));
    IDT_SetOffset(int_pagefault, (uint64_t) INT_PageFaultHandler);
    int_pagefault->type_dpl_p = IDT_TDPLP_INTERRUPT_GATE;
    int_pagefault->selector = 0x08;

    asm("lidt %0" :: "m" (idtr));
}

void _start(UEFIBootData* uefiBootData) {
    (void) BG_Init(uefiBootData);
    BG_SetBgColor(0xFF02242b);
    BG_ClearBg();
    print("Hello World From Kernel!\n");

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.size = sizeof(GDT) - 1;
    gdtDescriptor.offset = (uint64_t)&g_DefaultGDT;
    LoadGDT(&gdtDescriptor);

    InitializeMemory(uefiBootData);

    InitializeInterrupts();

    // asm("int $0x0e");

    print("Kernel Initialized.\n");
    while(true);
}
