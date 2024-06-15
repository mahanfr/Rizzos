#include "../common/graphics.h"
#include "../common/fonts.h"

#define WHITE 0xFFFFFFFF

Point CursorPosition = {.x= 8, .y= 16};
FrameBuffer frameBuffer;
PSF1_FONT font;

void PutChar(unsigned int color, char chr,
        unsigned int x_offset, unsigned int y_offset) {
    unsigned int* pixPtr = (unsigned int*)frameBuffer.BaseAddress;
    char* fontPtr = font.glyphBuffer + (chr * font.psfHeader->charsize);
    for (unsigned long y = y_offset; y < y_offset+16; y++) {
        for (unsigned long x = x_offset; x< x_offset + 8; x++) {
            if ((*fontPtr & (0b10000000 >> (x - x_offset))) > 0){
                *(unsigned int*)(pixPtr + x + (y * frameBuffer.PixelPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void Print(char* str, unsigned int color) {
    char* chr = str;
    while (*chr != 0) {
        PutChar(color, *chr, CursorPosition.x, CursorPosition.y);
        CursorPosition.x += 8;
        if (CursorPosition.x > frameBuffer.Width) {
            CursorPosition.x = 8;
            CursorPosition.y += 16;
        }
        chr++;
    }
}

void _start(FrameBuffer* framebuffer, PSF1_FONT* psf_font) {
    frameBuffer = *framebuffer;
    font = *psf_font;

    Print("Hello World From Kernel!\0", WHITE);
    return;
}
