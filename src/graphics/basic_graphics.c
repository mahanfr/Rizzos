#include "../../common/graphics.h"
#include "../../common/uefi_data.h"
#define STB_SPRINTF_IMPLEMENTATION
#include "../../lib/stb_sprintf.h"
#include "../memory.h"

#include <stdarg.h>
#define WHITE 0xFFFFFFFF
#define DEFAULT_CUR_X 8
#define DEFAULT_CUR_Y 16

static Point CursorPosition = {.x= DEFAULT_CUR_X, .y= DEFAULT_CUR_Y};
static FrameBuffer frameBuffer;
static PSF1_FONT font;
static int Color = WHITE;

void initBasicGraphics(UEFIBootData* uefiData) {
    frameBuffer = *uefiData->frameBuffer;
    font = *uefiData->consoleFont;
}

void clearBuffer() {
    memSet(frameBuffer.BaseAddress, 0, frameBuffer.BufferSize);
    CursorPosition.x= DEFAULT_CUR_X; 
    CursorPosition.y= DEFAULT_CUR_Y;
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

void print(const char* fmt, ...) {
    va_list va;
    char str[512];
    va_start(va, fmt);
    stbsp_vsprintf(str, fmt, va);
    va_end(va);
    char* chr = str;
    while (*chr != 0) {
        if (CursorPosition.y > frameBuffer.Height / 2) {
            clearBuffer();
        }
        switch (*chr) {
            case '\n':
                CursorPosition.x = DEFAULT_CUR_X;
                CursorPosition.y += DEFAULT_CUR_Y;
                break;
            case '\t':
                CursorPosition.x += DEFAULT_CUR_X * 4;
                break;
            case '\0':
                break;
            default:
                putChar(Color, *chr, CursorPosition.x, CursorPosition.y);
                CursorPosition.x += DEFAULT_CUR_X;
                break;
        }
        if (CursorPosition.x > frameBuffer.Width) {
            CursorPosition.x = DEFAULT_CUR_X;
            CursorPosition.y += DEFAULT_CUR_Y;
        }
        chr++;
    }
}

