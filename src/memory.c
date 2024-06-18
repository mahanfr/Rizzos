#include "memory.h"
#include <stdint.h>
#include "uefi_memory.h"

uint64_t getMemorySize(EFIMemoryDescriptor* mmap,
        uint64_t entries, uint64_t desc_size) {
    static uint64_t memory_size = 0;
    if (memory_size > 0) return memory_size;
    for (int i=0; i < entries; i++) {
        EFIMemoryDescriptor* desc = (EFIMemoryDescriptor*)
            ((uint64_t) mmap + (i * desc_size));
        memory_size += desc->numPages * 4096;
    }
    return memory_size;
}
