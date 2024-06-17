#ifndef __COMMON_UEFI_MEMORY__
#define __COMMON_UEFI_MEMORY__

#include <stdint.h>

typedef struct {
    uint32_t type;
    void* physAddr;
    void* virtAddr;
    uint64_t numPages;
    uint64_t attribs;
} EFIMemoryDescriptor;

#endif
