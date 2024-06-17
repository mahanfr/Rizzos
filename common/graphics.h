#ifndef __COMMON_GRAPHICS__
#define __COMMON_GRAPHICS__
#include <stddef.h>

typedef struct {
    void* BaseAddress;
    size_t BufferSize;
    unsigned int Width;
    unsigned int Height;
    unsigned int PixelPerScanLine;
} FrameBuffer;

typedef struct {
    unsigned int x;
    unsigned int y;
} Point;

#endif
