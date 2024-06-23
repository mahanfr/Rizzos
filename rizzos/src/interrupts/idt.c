#include "idt.h"
#include <stdint.h>

void IDT_SetOffset(IDTDescEntry* ide, uint64_t offset) {
    ide->offset0 = (uint16_t)  (offset & 0x000000000000ffff);
    ide->offset1 = (uint16_t) ((offset & 0x00000000ffff0000) >> 16);
    ide->offset2 = (uint32_t) ((offset & 0xffffffff00000000) >> 32);
}

uint64_t IDT_GetOffset(IDTDescEntry *ide) {
    uint64_t offset = 0;
    offset |= (uint64_t) ide->offset0;
    offset |= (uint64_t) ide->offset1 << 16;
    offset |= (uint64_t) ide->offset2 << 32;
    return offset;
}
