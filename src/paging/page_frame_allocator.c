#include "page_frame_allocator.h"
#include "../memory.h"
#include "../uefi_memory.h"
#include "../utils/bitmap.h"
#include <stdint.h>
#include <stdbool.h>

static uint64_t freeMemory;
static uint64_t reservedMemory;
static uint64_t usedMemory;
static bool initialized = false;
static Bitmap pageBitmap;

void initBitmap(size_t bitmap_size, void* bufferAddress);
void reservePage(void* address);
void reservePages(void* address, uint64_t pageCount);
void unreservePage(void* address);
void unreservePages(void* address, uint64_t pageCount);

void pageFrameInitEfiMemoryMap(EFIMemoryDescriptor *mmap, size_t desc_entries, size_t desc_size) {
    if (initialized) return;
    initialized = true;

    void* largest_free_mem = NULL;
    size_t largest_free_mem_size = 0;

    for(int i = 0; i < desc_entries; i++) {
        EFIMemoryDescriptor* desc = (EFIMemoryDescriptor*)((uint64_t)mmap + (i * desc_size));
        if (desc->type == 7) {
            if(desc->numPages * MEM_FRAME_SIZE > largest_free_mem_size) {
                largest_free_mem = desc->physAddr;
                largest_free_mem_size = desc->numPages * MEM_FRAME_SIZE;
            }
        }
    }

    uint64_t total_memory_size = getMemorySize(mmap, desc_entries, desc_size);
    freeMemory = total_memory_size;
    uint64_t bitmap_size = (total_memory_size / (MEM_FRAME_SIZE * 8)) + 1;

    initBitmap(bitmap_size, largest_free_mem);

    pageFrameLockPages(&pageBitmap.buffer, pageBitmap.size / 4096 + 1);

    for(int i = 0; i < desc_entries; i++) {
        EFIMemoryDescriptor* desc = (EFIMemoryDescriptor*)
            ((uint64_t)mmap + (i * desc_size));
        if(desc->type != 7) {
            reservePages(desc->physAddr, desc->numPages);
        }
    }
}

void* pageFrameRequestPage() {
    for(uint64_t index = 0; index < pageBitmap.size * 8; index++) {
        if (bitmapGet(&pageBitmap, index)) continue;
        pageFrameLockPage((void*) (index * MEM_FRAME_SIZE));
        return (void*)(index * MEM_FRAME_SIZE);
    }
    return NULL;
}

void initBitmap(size_t bitmap_size, void* bufferAddress) {
    if (pageBitmap.size > 0) return;
    pageBitmap.size = bitmap_size;
    pageBitmap.buffer = (uint8_t*) bufferAddress;

    for(int i=0; i< bitmap_size; i++) {
        pageBitmap.buffer[i] = 0;
    }
}

void pageFrameLockPage(void* address) {
    uint64_t index = (uint64_t) address / MEM_FRAME_SIZE;
    if(bitmapGet(&pageBitmap, index)) return;
    bitmapSet(&pageBitmap, index, true);
    freeMemory -= MEM_FRAME_SIZE;
    usedMemory += MEM_FRAME_SIZE;
}

void pageFrameLockPages(void* address, uint64_t pageCount) {
    for (uint64_t i=0; i < pageCount; i++) {
        pageFrameLockPage((void*)((uint64_t)address + (i * MEM_FRAME_SIZE)));
    }
}

void pageFrameFreePage(void* address) {
    uint64_t index = (uint64_t) address / MEM_FRAME_SIZE;
    if(!bitmapGet(&pageBitmap, index)) return;
    bitmapSet(&pageBitmap, index, false);
    freeMemory += MEM_FRAME_SIZE;
    usedMemory -= MEM_FRAME_SIZE;
}

void pageFrameFreePages(void* address, uint64_t pageCount) {
    for(int i=0;i< pageCount; i++) {
        pageFrameFreePage((void*)((uint64_t)address + (i * MEM_FRAME_SIZE)));
    }
}

void unreservePage(void* address) {
    uint64_t index = (uint64_t) address / MEM_FRAME_SIZE;
    if(!bitmapGet(&pageBitmap, index)) return;
    bitmapSet(&pageBitmap, index, false);
    freeMemory += MEM_FRAME_SIZE;
    reservedMemory -= MEM_FRAME_SIZE;
}

void unreservePages(void* address, uint64_t pageCount) {
    for(int i=0;i< pageCount; i++) {
        unreservePage((void*)((uint64_t)address + (i * MEM_FRAME_SIZE)));
    }
}

void reservePage(void* address) {
    uint64_t index = (uint64_t) address / MEM_FRAME_SIZE;
    if(bitmapGet(&pageBitmap, index)) return;
    bitmapSet(&pageBitmap, index, true);
    freeMemory -= MEM_FRAME_SIZE;
    reservedMemory += MEM_FRAME_SIZE;
}

void reservePages(void* address, uint64_t pageCount) {
    for(uint64_t i = 0; i < pageCount; i++) {
         reservePage((void*)((uint64_t) address + (i * MEM_FRAME_SIZE)));
    }
}

uint64_t getFreeMemorySize() {
    return freeMemory;
}

uint64_t getUsedMemorySize() {
    return usedMemory;
}

uint64_t getReservedMemorySize() {
    return reservedMemory;
}
