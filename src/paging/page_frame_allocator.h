#ifndef _PAGE_FRAME_ALLOCATOR_
#define _PAGE_FRAME_ALLOCATOR_

#include "../uefi_memory.h"
#include <stdint.h>
#include <stddef.h>

void PFA_InitEfiMemoryMap(EFIMemoryDescriptor* mmap, size_t mmap_entries, size_t mmap_desc_size);
void* PFA_RequestPage(void);
void PFA_LockPages(void* address, uint64_t pageCount);
void PFA_LockPage(void* address);
void PFA_FreePages(void* address, uint64_t pageCount);
void PFA_FreePage(void* address);
uint64_t PFA_GetFreeMemorySize(void);
uint64_t PFA_GetUsedMemorySize(void);
uint64_t PFA_GetReservedMemorySize(void);

#endif
