#ifndef _COMMON_UEFI_DATA_
#define _COMMON_UEFI_DATA_

#include "fonts.h"
#include "graphics.h"
#include <stdint.h>
#include "../src/uefi_memory.h"

typedef struct {
    FrameBuffer* frameBuffer;
    PSF1_FONT* consoleFont;
    EFIMemoryDescriptor* mMap;
    uint64_t mMapSize;
    uint64_t mMapDescSize;
    uint64_t mMapEntries;
    void* rsdp;
} UEFIBootData;

#endif
