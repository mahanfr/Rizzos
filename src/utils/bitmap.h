#ifndef _UTILS_BITMAP_
#define _UTILS_BITMAP_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    size_t size;
    uint8_t* buffer;
} Bitmap;

bool bitmapGet(Bitmap* bitmap,uint64_t index);
bool bitmapSet(Bitmap* bitmap,uint64_t index, bool value);

#endif
