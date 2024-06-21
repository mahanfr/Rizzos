#include "page_map_indexer.h"

PageMapIndexer* PMI_Create(uint64_t virtualAddress) {
    PageMapIndexer* pmi = {0};
    virtualAddress >>= 12;
    pmi->P_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pmi->PT_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pmi->PD_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pmi->PDP_i = virtualAddress & 0x1ff;

    return pmi;
}
