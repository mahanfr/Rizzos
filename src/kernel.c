#include "../common/uefi_data.h"
#include "basic_graphics.h"

void _start(UEFIData* uefiData) {
    initBasicGraphics(uefiData);
    print("Hello World From Kernel!\0");
    return;
}
