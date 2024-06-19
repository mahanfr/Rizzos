#ifndef _PAGE_TABLE_MANAGER
#define _PAGE_TABLE_MANAGER

#include "paging.h"
typedef struct {
    PageTable* PML4;
} PageTableManager;

PageTableManager* pageTableManager_Create(PageTable* PLM4Address);

void pageTableManager_MapMemory(PageTableManager* ptm, void* virtualMemory, void* physicalMemory);

#endif

