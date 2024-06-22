#include "paging.h"
#include <stdbool.h>
#include <stdint.h>

void PDE_SetFlag(PageDirEntry* pde, PT_Flag flag, bool enabled) {
    uint64_t bit_mask = (uint64_t) 1 << flag;
    pde->value &= ~bit_mask;
    if (enabled) {
        pde->value |= bit_mask;
    }
}

bool PDE_GetFlag(PageDirEntry* pde, PT_Flag flag) {
    uint64_t bit_mask = (uint64_t) 1 << flag;
    return (pde->value & bit_mask) > 0 ? true : false;
}

void PDE_SetAddress(PageDirEntry* pde, uint64_t address) {
    address &= 0x000000ffffffffff;
    pde->value &= 0xfff0000000000fff;
    pde->value |= (address << 12);
}

uint64_t PDE_GetAddress(PageDirEntry* pde) {
    return (pde->value & 0x000ffffffffff000) >> 12;
}
