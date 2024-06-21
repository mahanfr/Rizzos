#ifndef _COMMON_GRAPHICS_
#define _COMMON_GRAPHICS_
#include <stdint.h>

typedef struct {
    void* baseAddress;
    uint64_t bufferSize;
    uint32_t width;
    uint32_t height;
    uint32_t pixelPerScanLine;
} FrameBuffer;

typedef struct {
    uint32_t x;
    uint32_t y;
} Point;

#endif
