#ifndef __BASIC_GRAPHICS__
#define __BASIC_GRAPHICS__

#include "../common/uefi_data.h"

void initBasicGraphics(UEFIBootData* uefiData);
void print(char* str, ...);

#endif
