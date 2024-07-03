#ifndef _MEMORY_HEAP_H_
#define _MEMORY_HEAP_H_

#include <stddef.h>
#include <stdbool.h>
typedef struct HeapSegHdr HeapSegHdr;

struct HeapSegHdr{
    size_t lenght;
    HeapSegHdr* next;
    HeapSegHdr* last;
    bool free;
};
void MEM_HeapSegHdrCombineForward(HeapSegHdr* hshdr);
void MEM_HeapSegHdrCombineBackward(HeapSegHdr* hshdr);
HeapSegHdr* MEM_HeapSegHdrSplit(HeapSegHdr* hshdr, size_t splitLenght);

void MEM_InitializeHeap(void* heapAddress, size_t pageCount);
void* MEM_Malloc(size_t size);
void MEM_Free(void* addr);
void MEM_ExpandHeap(size_t lenght);

#endif
