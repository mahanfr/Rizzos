#include "../common/graphics.h"
#include "../common/fonts.h"

void PutChar(FrameBuffer* framebuffer,
        PSF1_FONT* font, unsigned int color, char chr,
        unsigned int x_offset, unsigned int y_offset) {
    unsigned int* pixPtr = (unsigned int*)framebuffer->BaseAddress;
    char* fontPtr = font->glyphBuffer + (chr * font->psfHeader->charsize);
    for (unsigned long y = y_offset; y < y_offset+16; y++) {
        for (unsigned long x = x_offset; x< x_offset + 8; x++) {
            if ((*fontPtr & (0b10000000 >> (x - x_offset))) > 0){
                *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void _start(FrameBuffer* framebuffer, PSF1_FONT* font) {
    PutChar(framebuffer, font, 0xFFFFFFFF, 'G', 10, 10);
    PutChar(framebuffer, font, 0xFFFFFFFF, 'A', 20, 10);
    PutChar(framebuffer, font, 0xFFFFFFFF, 'Y', 30, 10);
    return;
}
