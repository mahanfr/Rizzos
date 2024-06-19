#ifndef _COMMON_GRAPHICS_
#define _COMMON_GRAPHICS_
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
