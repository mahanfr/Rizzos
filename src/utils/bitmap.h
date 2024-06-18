#ifndef __UTILS_BITMAP__
#define __UTILS_BITMAP__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    size_t size;
    uint8_t* buffer;
} Bitmap;

bool bitmapGet(Bitmap* bitmap,uint64_t index);
void bitmapSet(Bitmap* bitmap,uint64_t index, bool value);

#endif
