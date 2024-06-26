#include "basic_graphics.h"
#include "../../common/graphics.h"
#include "../../common/uefi_data.h"
#include <stdint.h>
#define STB_SPRINTF_IMPLEMENTATION
#include "../../lib/stb_sprintf.h"
#include "../memory.h"
#include "../userinput/mouse.h"

#include <stdarg.h>
#define WHITE 0xFFFFFFFF
#define DEFAULT_CUR_X 8
#define DEFAULT_CUR_Y 16

static Point g_cursorPosition = {.x= DEFAULT_CUR_X, .y= DEFAULT_CUR_Y};
static FrameBuffer g_frameBuffer;
static PSF1_FONT g_font;
static uint32_t g_color = WHITE;
static uint32_t g_bgColor = 0;
static Point g_last_mouse_cur_pos = {0};

static void clearBuffer(void) {
    BG_ClearBg();
    BG_ResetCursor();
}

void BG_PutChar(char chr) {
    uint32_t* pixPtr = (uint32_t*) g_frameBuffer.baseAddress;
    char* fontPtr = (char*) g_font.glyphBuffer + (chr * g_font.psfHeader->charsize);
    for (uint64_t y = g_cursorPosition.y; y < g_cursorPosition.y + 16; y++) {
        for (uint64_t x = g_cursorPosition.x; x < g_cursorPosition.x + 8; x++) {
            if ((*fontPtr & (128 >> (x - g_cursorPosition.x))) > 0){
                *(uint32_t*)(pixPtr + x + (y * g_frameBuffer.pixelPerScanLine)) = g_color;
            }
        }
        fontPtr++;
    }
    g_cursorPosition.x += DEFAULT_CUR_X;
    if (g_cursorPosition.x > g_frameBuffer.width) {
        g_cursorPosition.x = DEFAULT_CUR_X;
        g_cursorPosition.y += DEFAULT_CUR_Y;
    }
    if (g_cursorPosition.y > g_frameBuffer.height - 16) {
        clearBuffer();
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

uint32_t BG_GetScreenWidth(void) {
    return g_frameBuffer.width;
}

uint32_t BG_GetScreenHeight(void) {
    return g_frameBuffer.height;
}

void BG_SetCursor(uint32_t x_offset, uint32_t y_offset) {
    g_cursorPosition.x = x_offset;
    g_cursorPosition.y = y_offset;
}

void BG_ResetCursor(void) {
    g_cursorPosition.x = DEFAULT_CUR_X;
    g_cursorPosition.y = DEFAULT_CUR_Y;
}

void BG_NextLine(void) {
    g_cursorPosition.x = DEFAULT_CUR_X;
    g_cursorPosition.y += DEFAULT_CUR_Y;
    if (g_cursorPosition.y > g_frameBuffer.height - 16) {
        clearBuffer();
    }
}

void BG_PutTab(void) {
    g_cursorPosition.x += DEFAULT_CUR_X * 4;
    if (g_cursorPosition.x > g_frameBuffer.width) {
        BG_NextLine();
    }
}

void BG_ClearChar(void) {
    if (g_cursorPosition.x > DEFAULT_CUR_X) {
        g_cursorPosition.x -= DEFAULT_CUR_X;
    } else {
        return;
    }
    uint32_t* pixPtr = (uint32_t*) g_frameBuffer.baseAddress;
    for (uint64_t y = g_cursorPosition.y; y < g_cursorPosition.y + 16; y++) {
        for (uint64_t x = g_cursorPosition.x; x < g_cursorPosition.x + 8; x++) {
            *(uint32_t*)(pixPtr + x + (y * g_frameBuffer.pixelPerScanLine)) = g_bgColor;
        }
    }
}

static void ClearLastMouseCurPos() {
    uint32_t* pixPtr = (uint32_t*) g_frameBuffer.baseAddress;
    for (uint64_t y = 0; y < MOUSE_POINTER_HEIGHT; y++) {
        for (uint64_t x = 0; x < MOUSE_POINTER_WIDTH; x++) {
            *(uint32_t*)(pixPtr
                    + (x + g_last_mouse_cur_pos.x)
                    + ((y + g_last_mouse_cur_pos.y) * g_frameBuffer.pixelPerScanLine))
                = g_bgColor;
        }
    }
}

void BG_DrawCursor(uint32_t x_offset, uint32_t y_offset) {
    ClearLastMouseCurPos();
    uint32_t* pixPtr = (uint32_t*) g_frameBuffer.baseAddress;
    for (uint64_t y = 0; y < MOUSE_POINTER_HEIGHT; y++) {
        for (uint64_t x = 0; x < MOUSE_POINTER_WIDTH; x++) {
            uint32_t pix_color;
            uint32_t index = x + (y * MOUSE_POINTER_WIDTH);
            if (UI_MousePointer[index] == 0)
                continue;
            else if (UI_MousePointer[index] == 1) {
                pix_color = 0xFFFFFFFF;
            } else {
                pix_color = 0;
            }
            *(uint32_t*)(pixPtr 
                    + (x + x_offset) 
                    + ((y + y_offset) * g_frameBuffer.pixelPerScanLine))
                = pix_color;
        }
    }
    g_last_mouse_cur_pos.x = x_offset;
    g_last_mouse_cur_pos.y = y_offset;
}

void print(const char* fmt, ...) {
    va_list va;
    char str[512];
    va_start(va, fmt);
    stbsp_vsprintf(str, fmt, va);
    va_end(va);
    char* chr = str;
    while (*chr != 0) {
        switch (*chr) {
            case '\n':
                BG_NextLine();
                break;
            case '\t':
                BG_PutTab();
                break;
            case '\0':
                break;
            default:
                BG_PutChar(*chr);
                break;
        }
        chr++;
    }
}

