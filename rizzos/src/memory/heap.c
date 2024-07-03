#include "heap.h"
#include "../paging/page_frame_allocator.h"
#include "../paging/page_table_manager.h"

static void* g_HeapStart;
static void* g_HeapEnd;
static HeapSegHdr* g_LastHdr;

void MEM_InitializeHeap(void *heapAddress, size_t pageCount) {
    void* pos = heapAddress;

    for(size_t i = 0; i < pageCount; i++){
        PTM_MapMemory(pos, PFA_RequestPage());
        pos = (void*) ((size_t) pos + 0x1000);
    }

    size_t heapLenght = pageCount * 0x1000;
    g_HeapStart = heapAddress; 
    g_HeapEnd = (void*)((size_t)heapAddress + heapLenght);
    HeapSegHdr* startSeg = (HeapSegHdr*)heapAddress;
    startSeg->lenght = heapLenght - sizeof(HeapSegHdr);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    g_LastHdr = startSeg; 
}
