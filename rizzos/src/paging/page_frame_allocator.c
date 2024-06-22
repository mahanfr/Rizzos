#include "page_frame_allocator.h"
#include "../memory.h"
#include "../uefi_memory.h"
#include "../utils/bitmap.h"
#include <stdint.h>
#include <stdbool.h>

static uint64_t g_freeMemory;
static uint64_t g_reservedMemory;
static uint64_t g_usedMemory;
static bool g_initialized = false;
static Bitmap g_pageBitmap;
static uint64_t g_lastReservedPage = 0;

static void InitBitmap(size_t bitmap_size, void* bufferAddress);
static void ReservePage(void* address);
static void ReservePages(void* address, uint64_t pageCount);
static void UnreservePage(void* address);
static void UnreservePages(void* address, uint64_t pageCount);

void PFA_InitEfiMemoryMap(EFIMemoryDescriptor *mmap, size_t desc_entries, size_t desc_size) {
    if (g_initialized) return;
    g_initialized = true;

    void* largest_free_mem = NULL;
    size_t largest_free_mem_size = 0;

    for(size_t i = 0; i < desc_entries; i++) {
        EFIMemoryDescriptor* desc = (EFIMemoryDescriptor*)((uint64_t)mmap + (i * desc_size));
        if (desc->type == 7) {
            if(desc->numPages * MEM_FRAME_SIZE > largest_free_mem_size) {
                largest_free_mem = desc->physAddr;
                largest_free_mem_size = desc->numPages * MEM_FRAME_SIZE;
            }
        }
    }

    uint64_t total_memory_size = MEM_GetTotalSize(mmap, desc_entries, desc_size);
    g_freeMemory = total_memory_size;
    uint64_t bitmap_size = (total_memory_size / (MEM_FRAME_SIZE * 8)) + 1;

    InitBitmap(bitmap_size, largest_free_mem);

    PFA_LockPages(&g_pageBitmap.buffer, g_pageBitmap.size / 4096 + 1);

    for(size_t i = 0; i < desc_entries; i++) {
        EFIMemoryDescriptor* desc = (EFIMemoryDescriptor*)
            ((uint64_t)mmap + (i * desc_size));
        if(desc->type != 7) {
            ReservePages(desc->physAddr, desc->numPages);
        }
    }
}

void* PFA_RequestPage(void) {
    for(uint64_t index = g_lastReservedPage; index < g_pageBitmap.size * 8; index++) {
        if (Bitmap_Get(&g_pageBitmap, index) == true) continue;
        PFA_LockPage((void*) (index * MEM_FRAME_SIZE));
        g_lastReservedPage = index;
        return (void*)(index * MEM_FRAME_SIZE);
    }
    return NULL;
}

void PFA_LockPage(void* address) {
    uint64_t index = (uint64_t) address / MEM_FRAME_SIZE;
    if(Bitmap_Get(&g_pageBitmap, index)) return;
    if(Bitmap_Set(&g_pageBitmap, index, true)) {
        g_freeMemory -= MEM_FRAME_SIZE;
        g_usedMemory += MEM_FRAME_SIZE;
    }
}

void PFA_LockPages(void* address, uint64_t pageCount) {
    for (uint64_t i=0; i < pageCount; i++) {
        PFA_LockPage((void*)((uint64_t)address + (i * MEM_FRAME_SIZE)));
    }
}

void PFA_FreePage(void* address) {
    uint64_t index = (uint64_t) address / MEM_FRAME_SIZE;
    if(!Bitmap_Get(&g_pageBitmap, index)) return;
    if(Bitmap_Set(&g_pageBitmap, index, false)){
        g_freeMemory += MEM_FRAME_SIZE;
        g_usedMemory -= MEM_FRAME_SIZE;
        if (g_lastReservedPage > index) g_lastReservedPage = index;
    }
}

void PFA_FreePages(void* address, uint64_t pageCount) {
    for(uint64_t i=0;i < pageCount; i++) {
        PFA_FreePage((void*)((uint64_t)address + (i * MEM_FRAME_SIZE)));
    }
}

uint64_t PFA_GetFreeMemorySize(void) {
    return g_freeMemory;
}

uint64_t PFA_GetUsedMemorySize(void) {
    return g_usedMemory;
}

uint64_t PFA_GetReservedMemorySize(void) {
    return g_reservedMemory;
}

static void InitBitmap(size_t bitmap_size, void* bufferAddress) {
    g_pageBitmap.size = bitmap_size;
    g_pageBitmap.buffer = (uint8_t*) bufferAddress;

    MEM_Set64(g_pageBitmap.buffer, 0 , g_pageBitmap.size);
}

static void UnreservePage(void* address) {
    uint64_t index = (uint64_t) address / MEM_FRAME_SIZE;
    if(!Bitmap_Get(&g_pageBitmap, index)) return;
    if(Bitmap_Set(&g_pageBitmap, index, false)) {
        g_freeMemory += MEM_FRAME_SIZE;
        g_reservedMemory -= MEM_FRAME_SIZE;
        if (g_lastReservedPage > index) g_lastReservedPage = index;
    }
}

static void UnreservePages(void* address, uint64_t pageCount) {
    for(uint64_t i=0;i < pageCount; i++) {
        UnreservePage((void*)((uint64_t)address + (i * MEM_FRAME_SIZE)));
    }
}

static void ReservePage(void* address) {
    uint64_t index = (uint64_t) address / MEM_FRAME_SIZE;
    if(Bitmap_Get(&g_pageBitmap, index)) return;
    if(Bitmap_Set(&g_pageBitmap, index, true)) {
        g_freeMemory -= MEM_FRAME_SIZE;
        g_reservedMemory += MEM_FRAME_SIZE;
    }
}

static void ReservePages(void* address, uint64_t pageCount) {
    for(uint64_t i = 0; i < pageCount; i++) {
         ReservePage((void*)((uint64_t) address + (i * MEM_FRAME_SIZE)));
    }
}

