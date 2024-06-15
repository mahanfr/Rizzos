#include "../common/graphics.h"

void _start(FrameBuffer* framebuffer) {
    for (unsigned int y = 0; y< framebuffer->Height; y += 20){
        for (unsigned int x = 0; x < framebuffer->Width; x++) {
            *(unsigned int*)((x * 4) + (y * framebuffer->PixelPerScanLine * 4)
                    + framebuffer->BaseAddress) = 0xFFFF00FF;
        }
    }
    return;
}
