#include "page_table_manager.h"
#include "page_frame_allocator.h"
#include "page_map_indexer.h"
#include "paging.h"
#include "../memory.h"
#include <stdint.h>
#include <stdbool.h>

static PageTableManager g_pageTableManager;

void PTM_Create(PageTable* PLM4Address) {
    g_pageTableManager.PML4 = PLM4Address;
}

void PTM_MapMemory(void* virtualMemory, void* physicalMemory) {
    PageMapIndexer* indexer = PMI_Create((uint64_t) virtualMemory);
    PageDirEntry pde;

    pde = g_pageTableManager.PML4->entries[indexer->PDP_i];
    PageTable* pdp;
    if(!PDE_GetFlag(&pde, PT_PRESENT)) {
        pdp = (PageTable*) PFA_RequestPage();
        MEM_Set64(pdp, 0, MEM_FRAME_SIZE);
        PDE_SetAddress(&pde, (uint64_t) pdp >> 12);
        PDE_SetFlag(&pde, PT_PRESENT, true);
        PDE_SetFlag(&pde, PT_READWRITE, true);
        g_pageTableManager.PML4->entries[indexer->PDP_i] = pde;
    } else {
        pdp = (PageTable*)((uint64_t)PDE_GetAddress(&pde) << 12);
    }

    pde = pdp->entries[indexer->PD_i];
    PageTable* pd;
    if(!PDE_GetFlag(&pde, PT_PRESENT)) {
        pd = (PageTable*) PFA_RequestPage();
        MEM_Set64(pd, 0, MEM_FRAME_SIZE);
        PDE_SetAddress(&pde, (uint64_t) pd >> 12);
        PDE_SetFlag(&pde, PT_PRESENT, true);
        PDE_SetFlag(&pde, PT_READWRITE, true);
        pdp->entries[indexer->PD_i] = pde;
    } else {
        pd = (PageTable*)((uint64_t)PDE_GetAddress(&pde) << 12);
    }

    pde = pd->entries[indexer->PT_i];
    PageTable* pt;
    if(!PDE_GetFlag(&pde, PT_PRESENT)) {
        pt = (PageTable*) PFA_RequestPage();
        MEM_Set64(pt, 0, MEM_FRAME_SIZE);
        PDE_SetAddress(&pde, (uint64_t) pt >> 12);
        PDE_SetFlag(&pde, PT_PRESENT, true);
        PDE_SetFlag(&pde, PT_READWRITE, true);
        pd->entries[indexer->PT_i] = pde;
    } else {
        pt = (PageTable*)((uint64_t)PDE_GetAddress(&pde) << 12);
    }

    pde = pt->entries[indexer->P_i];
    PDE_SetAddress(&pde, (uint64_t) physicalMemory >> 12);
    PDE_SetFlag(&pde, PT_PRESENT, true);
    PDE_SetFlag(&pde, PT_READWRITE, true);
    pt->entries[indexer->P_i] = pde;
}
