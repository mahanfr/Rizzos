#include "page_table_manager.h"
#include "page_frame_allocator.h"
#include "page_map_indexer.h"
#include "paging.h"
#include "../memory.h"
#include <stdint.h>
#include <stdbool.h>

PageTableManager* pageTableManager_Create(PageTable* PLM4Address) {
    PageTableManager* ptm = {0};
    ptm->PML4 = PLM4Address;
    return ptm;
}

void pageTableManager_MapMemory(PageTableManager* ptm, void* virtualMemory, void* physicalMemory) {
    PageMapIndexer indexer = *pageMapIndexer_Create((uint64_t) virtualMemory);
    PageDirEntry pde;

    pde = ptm->PML4->entries[indexer.PDP_i];
    PageTable* pdp;
    if(!pde.Present) {
        pdp = (PageTable*) pageFrameRequestPage();
        memSet(pdp, 0, 0x1000);
        pde.Address = (uint64_t) pdp >> 12;
        pde.Present = true;
        pde.ReadWrite = true;
        ptm->PML4->entries[indexer.PDP_i] = pde;
    } else {
        pdp = (PageTable*)((uint64_t) pde.Address << 12);
    }

    pde = pdp->entries[indexer.PD_i];
    PageTable* pd;
    if (!pde.Present){
        pd = (PageTable*) pageFrameRequestPage();
        memSet(pd, 0, 0x1000);
        pde.Address = (uint64_t)pd >> 12;
        pde.Present = true;
        pde.ReadWrite = true;
        pdp->entries[indexer.PD_i] = pde;
    } else {
        pd = (PageTable*)((uint64_t)pde.Address << 12);
    }

    pde = pd->entries[indexer.PT_i];
    PageTable* pt;
    if (!pde.Present){
        pt = (PageTable*) pageFrameRequestPage();
        memSet(pt, 0, 0x1000);
        pde.Address = (uint64_t)pt >> 12;
        pde.Present = true;
        pde.ReadWrite = true;
        pd->entries[indexer.PT_i] = pde;
    } else {
        pt = (PageTable*)((uint64_t)pde.Address << 12);
    }

    pde = pt->entries[indexer.P_i];
    pde.Address = (uint64_t)physicalMemory >> 12;
    pde.Present = true;
    pde.ReadWrite = true;
    pt->entries[indexer.P_i] = pde;
}
