#ifndef _UEFI_MEMORY_
#define _UEFI_MEMORY_

#include <stdint.h>

const char* uefiMemTypeName(uint32_t type);

typedef struct {
    uint32_t type;
    uint32_t pad;
    void* physAddr;
    void* virtAddr;
    uint64_t numPages;
    uint64_t attribs;
} EFIMemoryDescriptor;

#endif
