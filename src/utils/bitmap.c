#include <stdint.h>
#include <stdbool.h>
#include "bitmap.h"

bool bitmapGet(Bitmap* bitmap, uint64_t index) {
    uint64_t byte_index;
    uint8_t bit_index;
    uint8_t bit_mask;
    byte_index = index / 8;
    bit_index = index % 8;
    bit_mask = (uint8_t) (1 << bit_index);
    if ((bitmap->buffer[byte_index] & (uint8_t) bit_mask) > 0) {
        return true;
    }
    return false;
}

bool bitmapSet(Bitmap* bitmap,uint64_t index, bool value) {
    if (index > bitmap->size * 8) return false;
    uint64_t byte_index;
    uint8_t bit_index;
    uint8_t bit_mask;
    byte_index = index / 8;
    bit_index = index % 8;
    bit_mask = (uint8_t) (1 << bit_index);
    bitmap->buffer[byte_index] &= (uint8_t) ~bit_mask;
    if (value) {
        bitmap->buffer[byte_index] |= (uint8_t) bit_mask;
    }
    return true;
}
