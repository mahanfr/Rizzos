#ifndef _PAGE_MAP_INDEXER_
#define _PAGE_MAP_INDEXER_

#include <stdint.h>
typedef struct {
    uint64_t PDP_i;
    uint64_t PD_i;
    uint64_t PT_i;
    uint64_t P_i;
} PageMapIndexer;

PageMapIndexer* PMI_Create(uint64_t virtualAddress);

#endif
