#ifndef _PAGING_
#define _PAGING_

#include <stdbool.h>
#include <stdint.h>
typedef struct {
    bool Present : 1;
    bool ReadWrite: 1;
    bool UserSuper: 1;
    bool WriteThrough: 1;
    bool CacheDisabled: 1;
    bool Accessed: 1;
    bool Dirty: 1;
    bool LargePages: 1;
    bool Global: 1;
    uint8_t Available: 3;
    uint64_t Address: 52;
} PageDirEntry;

typedef struct {
    PageDirEntry entries[512];
} PageTable __attribute__((aligned(0x1000)));

#endif
