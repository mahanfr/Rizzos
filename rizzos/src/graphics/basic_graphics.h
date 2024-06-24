#ifndef _BASIC_GRAPHICS_
#define _BASIC_GRAPHICS_

#include "../../common/uefi_data.h"

void BG_Init(UEFIBootData* uefiData);
void BG_ClearBg(void);
void BG_SetBgColor(int bgColor);
void BG_SetColor(int color);
void BG_SetCursor(uint32_t x_offset, uint32_t y_offset);
void BG_ResetCursor(void);
void BG_PutChar(char chr);
void BG_NextLine(void);
void BG_PutTab(void);
void BG_ClearChar(void);
void print(const char* fmt, ...);

#endif
