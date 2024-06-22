#ifndef _PAGE_TABLE_MANAGER
#define _PAGE_TABLE_MANAGER

#include "paging.h"
typedef struct {
    PageTable* PML4;
} PageTableManager;

PageTableManager PTM_Create(PageTable* PLM4Address);

void PTM_MapMemory(PageTableManager* ptm, void* virtualMemory, void* physicalMemory);

#endif

