#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../common/uefi_data.h"
#include "gdt/gdt.h"
#include "graphics/basic_graphics.h"
#include "interrupts/idt.h"
#include "interrupts/interrupts.h"
#include "interrupts/interrupt_handler.h"
#include "io.h"
#include "memory.h"
#include "memory/heap.h"
#include "paging/page_frame_allocator.h"
#include "paging/page_table_manager.h"
#include "paging/paging.h"
#include "pci.h"
#include "userinput/mouse.h"
#include "acpi.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

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
    (void) PTM_Create(PML4);

    // Map All the Pages of Memory to thire Virtual Memory
    uint64_t memorySize = MEM_GetTotalSize(uefiBootData->mMap, uefiBootData->mMapEntries, uefiBootData->mMapDescSize);
    for(uint64_t i = 0; i < memorySize; i+= MEM_FRAME_SIZE) {
        PTM_MapMemory((void*)i, (void*)i);
    }
    // Map All the Pages of Memory to thire Virtual Memory
    for(uint64_t i = (uint64_t) uefiBootData->frameBuffer->baseAddress;
            i < (uint64_t)uefiBootData->frameBuffer->baseAddress + uefiBootData->frameBuffer->bufferSize;
            i+= MEM_FRAME_SIZE) {
        PTM_MapMemory((void*)i, (void*)i);
    }

    asm("mov %0, %%cr3" :: "r" (PML4));
}

static void InitializeInterrupts(void) {
    IDT_InitInterruptTable();

    INTH_SetInterruptHandler(0xE, (uint64_t) INT_PageFaultHandler);
    INTH_SetInterruptHandler(0x8, (uint64_t) INT_DoubleFaultHandler);
    INTH_SetInterruptHandler(0xD, (uint64_t) INT_GPFaultHandler);
    INTH_SetInterruptHandler(0x21, (uint64_t) INT_KeyboardIntHandler);
    INTH_SetInterruptHandler(0x2C, (uint64_t) INT_MouseIntHandler);

    IDTR idtr = IDT_GetInterruptTable();
    asm("lidt %0" :: "m" (idtr));

    INT_PIC_Remap(0x20, 0x28);
}

void InitializeACPI(UEFIBootData* uefiBootData) {
    SDTHeader* xsdt = (SDTHeader*)(uefiBootData->rsdp->XSDTAddress);
    MCFGHeader* mcfg = (MCFGHeader*) ACPI_FindTable(xsdt, (char*) "MCFG");

    PCI_EnumeratePCI(mcfg);
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

    (void) MEM_InitializeHeap((void*) 0x0000100000000000, 0x10);

    InitializeInterrupts();

    UI_PS2Mouse_Init();

    InitializeACPI(uefiBootData);

    IO_OutByte(INT_PIC1_DATA, 0b11111001);
    IO_OutByte(INT_PIC2_DATA, 0b11101111);
    asm("sti");

    print("Kernel Initialized.\n");

    while(true);
}
