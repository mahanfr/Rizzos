#ifndef _PAGE_TABLE_MANAGER
#define _PAGE_TABLE_MANAGER

#include "paging.h"
typedef struct {
    PageTable* PML4;
} PageTableManager;

void PTM_Create(PageTable* PLM4Address);

void PTM_MapMemory(void* virtualMemory, void* physicalMemory);

#endif

