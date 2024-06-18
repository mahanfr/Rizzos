#include "../common/uefi_data.h"
#include "basic_graphics.h"
#include "memory.h"
#include <stdint.h>
#include <stdbool.h>
#include "utils/bitmap.h"

uint8_t buffer_test[16];
void _start(UEFIBootData* uefiBootData) {
    initBasicGraphics(uefiBootData);
    print("Hello World From Kernel!\n");

    print("Memory size in bytes: %d\n",
            getMemorySize(uefiBootData->mMap,
                uefiBootData->mMapEntries,
                uefiBootData->mMapDescSize));

    Bitmap *bitmap = {0};
    bitmap->buffer = buffer_test;
    bitmap->size = 16;

    bitmapSet(bitmap, 0, true);
    bitmapSet(bitmap, 8, true);
    bitmapSet(bitmap, 13, true);
    bitmapSet(bitmap, 13, false);
    bitmapSet(bitmap, 14, true);

    for(int i=0; i < 16; i++) {
        print("bit[%d]: %d\n",i ,bitmapGet(bitmap, i));
    }

   return;
}
