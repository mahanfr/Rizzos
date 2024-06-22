#ifndef _PAGING_
#define _PAGING_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PT_PRESENT = 0,
    PT_READWRITE = 1,
    PT_USERSUPER = 2,
    PT_WRITETHROUGH = 3,
    PT_CACHEDISABLED = 4,
    PT_ACCESSED = 5,
    PT_DIRTY = 6,
    PT_LARGEPAGES = 7,
    PT_GLOBAL = 8,
    PT_ACCESS0 = 9,
    PT_ACCESS1 = 10,
    PT_ACCESS2 = 11,
    PT_NX = 63,
} PT_Flag;

typedef struct {
    uint64_t value;
} PageDirEntry;

void PDE_SetFlag(PageDirEntry* pde, PT_Flag flag, bool enabled);
bool PDE_GetFlag(PageDirEntry* pde, PT_Flag flag);
void PDE_SetAddress(PageDirEntry* pde, uint64_t address);
uint64_t PDE_GetAddress(PageDirEntry* pde);

typedef struct {
    PageDirEntry entries[512];
} PageTable __attribute__((aligned(0x1000)));

#endif
