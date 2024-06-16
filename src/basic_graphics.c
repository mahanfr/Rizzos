#include "../common/graphics.h"
#include "../common/uefi_data.h"

Point CursorPosition = {.x= 8, .y= 16};
FrameBuffer frameBuffer;
PSF1_FONT font;
#define WHITE 0xFFFFFFFF

void initBasicGraphics(UEFIData* uefiData) {
    frameBuffer = *uefiData->frameBuffer;
    font = *uefiData->consoleFont;
}

void putChar(unsigned int color, char chr,
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

void print(char* str) {
    char* chr = str;
    while (*chr != 0) {
        putChar(WHITE, *chr, CursorPosition.x, CursorPosition.y);
        CursorPosition.x += 8;
        if (CursorPosition.x > frameBuffer.Width) {
            CursorPosition.x = 8;
            CursorPosition.y += 16;
        }
        chr++;
    }
}

