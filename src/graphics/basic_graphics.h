#ifndef _BASIC_GRAPHICS_
#define _BASIC_GRAPHICS_

#include "../../common/uefi_data.h"

void initBasicGraphics(UEFIBootData* uefiData);
void clearBackground();
void setBgColor(int bgColor);
void setColor(int color);
void print(const char* fmt, ...);

#endif
