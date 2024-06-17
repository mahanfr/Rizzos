#ifndef __COMMON_UEFI_DATA_
#define __COMMON_UEFI_DATA_

#include "fonts.h"
#include "graphics.h"

typedef struct {
    FrameBuffer* frameBuffer;
    PSF1_FONT* consoleFont;
} UEFIBootData;

#endif
