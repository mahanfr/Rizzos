#ifndef _PAGE_FRAME_ALLOCATOR_
#define _PAGE_FRAME_ALLOCATOR_

#include "uefi_memory.h"
#include <stdint.h>
#include <stddef.h>

void pageFrameInitEfiMemoryMap(EFIMemoryDescriptor* mmap, size_t mmap_entries, size_t mmap_desc_size);
void* pageFrameRequestPage();
void pageFrameLockPages(void* address, uint64_t pageCount);
void pageFrameLockPage(void* address);
void pageFrameFreePages(void* address, uint64_t pageCount);
void pageFrameFreePage(void* address);
uint64_t getFreeMemorySize();
uint64_t getUsedMemorySize();
uint64_t getReservedMemorySize();

#endif
