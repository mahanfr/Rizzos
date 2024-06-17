#ifndef __COMMON_UEFI_DATA_
#define __COMMON_UEFI_DATA_

#include "fonts.h"
#include "graphics.h"
#include <stdint.h>

typedef struct {
    FrameBuffer* frameBuffer;
    PSF1_FONT* consoleFont;
    void* mMap;
    uint64_t mMapSize;
    uint64_t mMapDescSize;
} UEFIBootData;

#endif
