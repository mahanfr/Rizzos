#include "../common/uefi_data.h"
#include "basic_graphics.h"

void _start(UEFIBootData* uefiBootData) {
    initBasicGraphics(uefiBootData);
    print("Hello World From Kernel!\n");

    return;
}
