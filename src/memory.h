#ifndef __MEMORY__
#define __MEMORY__

#include <stdint.h>
#include "uefi_memory.h"

uint64_t getMemorySize(EFIMemoryDescriptor* mmap, uint64_t entries, uint64_t desc_size);

#endif
