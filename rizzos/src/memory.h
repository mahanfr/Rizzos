#ifndef _MEMORY_
#define _MEMORY_

#include <stdint.h>
#include "uefi_memory.h"

#define MEM_FRAME_SIZE 0x1000

uint64_t MEM_GetTotalSize(EFIMemoryDescriptor* mmap, uint64_t entries, uint64_t desc_size);

void MEM_Set(void* start, uint8_t value, uint64_t num);
void MEM_Set32(void* start, uint32_t value, uint64_t num);
void MEM_Set64(void* start, uint64_t value, uint64_t num);

#endif
