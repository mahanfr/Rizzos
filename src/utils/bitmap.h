#ifndef _UTILS_BITMAP_
#define _UTILS_BITMAP_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    size_t size;
    uint8_t* buffer;
} Bitmap;

bool Bitmap_Get(Bitmap* bitmap,uint64_t index);
bool Bitmap_Set(Bitmap* bitmap,uint64_t index, bool value);

#endif
