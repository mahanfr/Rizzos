#ifndef _MEMORY_
#define _MEMORY_

#include <stdint.h>
#include "uefi_memory.h"

#define MEM_FRAME_SIZE 0x1000

uint64_t getMemorySize(EFIMemoryDescriptor* mmap, uint64_t entries, uint64_t desc_size);

void memSet(void* start, uint8_t value, uint64_t num);

#endif
