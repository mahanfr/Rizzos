#include "basic_graphics.h"
#include "../common/graphics.h"
#include "../common/uefi_data.h"
#include <stdint.h>
#define STB_SPRINTF_IMPLEMENTATION
#include "../lib/stb_sprintf.h"
#include "../memory.h"

#include <stdarg.h>
#define WHITE 0xFFFFFFFF
#define DEFAULT_CUR_X 8
#define DEFAULT_CUR_Y 16

static Point g_cursorPosition = {.x= DEFAULT_CUR_X, .y= DEFAULT_CUR_Y};
static FrameBuffer g_frameBuffer;
static PSF1_FONT g_font;
static uint32_t g_color = WHITE;
static uint32_t g_bgColor = 0;

static void clearBuffer(void) {
    BG_ClearBg();
    g_cursorPosition.x= DEFAULT_CUR_X;
    g_cursorPosition.y= DEFAULT_CUR_Y;
}

static void putChar(uint32_t color, char chr,
        uint64_t x_offset, uint64_t y_offset) {
    uint32_t* pixPtr = (uint32_t*) g_frameBuffer.baseAddress;
    char* fontPtr = (char*) g_font.glyphBuffer + (chr * g_font.psfHeader->charsize);
    for (uint64_t y = y_offset; y < y_offset + 16; y++) {
        for (uint64_t x = x_offset; x < x_offset + 8; x++) {
            if ((*fontPtr & (128 >> (x - x_offset))) > 0){
                *(uint32_t*)(pixPtr + x + (y * g_frameBuffer.pixelPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void BG_Init(UEFIBootData* uefiData) {
    g_frameBuffer = *uefiData->frameBuffer;
    g_font = *uefiData->consoleFont;
}

void BG_SetBgColor(int bg_color) {
    g_bgColor = bg_color;
}

void BG_SetColor(int fg_color) {
    g_color = fg_color;
}

void BG_ClearBg(void) {
    MEM_Set32(g_frameBuffer.baseAddress, g_bgColor, g_frameBuffer.bufferSize);
}

void BG_SetCursor(uint32_t x_offset, uint32_t y_offset) {
    g_cursorPosition.x = x_offset;
    g_cursorPosition.y = y_offset;
}

void print(const char* fmt, ...) {
    va_list va;
    char str[512];
    va_start(va, fmt);
    stbsp_vsprintf(str, fmt, va);
    va_end(va);
    char* chr = str;
    while (*chr != 0) {
        if (g_cursorPosition.y > g_frameBuffer.height / 2) {
            clearBuffer();
        }
        switch (*chr) {
            case '\n':
                g_cursorPosition.x = DEFAULT_CUR_X;
                g_cursorPosition.y += DEFAULT_CUR_Y;
                break;
            case '\t':
                g_cursorPosition.x += DEFAULT_CUR_X * 4;
                break;
            case '\0':
                break;
            default:
                putChar(g_color, *chr, g_cursorPosition.x, g_cursorPosition.y);
                g_cursorPosition.x += DEFAULT_CUR_X;
                break;
        }
        if (g_cursorPosition.x > g_frameBuffer.width) {
            g_cursorPosition.x = DEFAULT_CUR_X;
            g_cursorPosition.y += DEFAULT_CUR_Y;
        }
        chr++;
    }
}

