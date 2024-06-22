#include "memory.h"
#include <stdint.h>
#include "uefi_memory.h"

uint64_t MEM_GetTotalSize(EFIMemoryDescriptor* mmap,
        uint64_t entries, uint64_t desc_size) {
    static uint64_t memory_size = 0;
    if (memory_size > 0) return memory_size;
    for (uint64_t i=0; i < entries; i++) {
        EFIMemoryDescriptor* desc = (EFIMemoryDescriptor*)
            ((uint64_t) mmap + (i * desc_size));
        memory_size += desc->numPages * 4096;
    }
    return memory_size;
}

void MEM_Set64(void* start, uint64_t value, uint64_t num) {
    for(uint64_t i=0; i < num; i+=8) {
        *(uint64_t*)((uint64_t) start + i) = value;
    }
}

void MEM_Set32(void* start, uint32_t value, uint64_t num) {
    for(uint64_t i=0; i < num; i+=4) {
        *(uint32_t*)((uint64_t) start + i) = value;
    }
}

void MEM_Set(void* start, uint8_t value, uint64_t num) {
    for(uint64_t i=0; i < num; i++) {
        *(uint8_t*)((uint64_t) start + i) = value;
    }
}
