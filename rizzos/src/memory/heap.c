#include "heap.h"
#include "../paging/page_frame_allocator.h"
#include "../paging/page_table_manager.h"
#include <stdbool.h>
#include <stdint.h>
#include "../memory.h"

static void* g_HeapStart;
static void* g_HeapEnd;
static HeapSegHdr* g_LastHdr;

void MEM_InitializeHeap(void *heapAddress, size_t pageCount) {
    void* pos = heapAddress;

    for(size_t i = 0; i < pageCount; i++){
        PTM_MapMemory(pos, PFA_RequestPage());
        pos = (void*) ((size_t) pos + MEM_FRAME_SIZE);
    }

    size_t heapLenght = pageCount * MEM_FRAME_SIZE;
    g_HeapStart = heapAddress; 
    g_HeapEnd = (void*)((size_t)heapAddress + heapLenght);
    HeapSegHdr* startSeg = (HeapSegHdr*)heapAddress;
    startSeg->lenght = heapLenght - sizeof(HeapSegHdr);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    g_LastHdr = startSeg; 
}

void MEM_Free(void* addr) {
    HeapSegHdr* segment = (HeapSegHdr*) addr - 1;
    segment->free = true;
    MEM_HeapSegHdrCombineForward(segment);
    MEM_HeapSegHdrCombineBackward(segment);
}

void* MEM_Malloc(size_t size) {
    if(size % 0x10 > 0) {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return NULL;
    
    HeapSegHdr* currentSeg = (HeapSegHdr*) g_HeapStart;
    while (true) {
        if(currentSeg->free) {
            if(currentSeg->lenght > size) {
                MEM_HeapSegHdrSplit(currentSeg, size);
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegHdr));
            }
            if (currentSeg->lenght == size) {
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegHdr));
            }
        }
        if(currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    MEM_ExpandHeap(size);
    return MEM_Malloc(size);
}

void MEM_ExpandHeap(size_t lenght) {
    if(lenght % MEM_FRAME_SIZE) {
        lenght -= lenght % MEM_FRAME_SIZE;
        lenght += MEM_FRAME_SIZE;
    }

    size_t pageCount = lenght / MEM_FRAME_SIZE;
    HeapSegHdr* newSegment = (HeapSegHdr*) g_HeapEnd;
    for(size_t i=0; i< pageCount; i++) {
        PTM_MapMemory(g_HeapEnd, PFA_RequestPage());
        g_HeapEnd = (void*)((size_t) g_HeapEnd + MEM_FRAME_SIZE);
    }

    newSegment->free = true;
    newSegment->last = g_LastHdr;
    g_LastHdr->next = newSegment;
    g_LastHdr = newSegment;
    newSegment->next = NULL;
    newSegment->lenght = lenght - sizeof(HeapSegHdr);
    MEM_HeapSegHdrCombineBackward(newSegment);
}

HeapSegHdr* MEM_HeapSegHdrSplit(HeapSegHdr* hshdr, size_t splitLenght) {
    if(splitLenght < 0x10) return NULL;
    int64_t splitSeqLenght = hshdr->lenght - splitLenght - sizeof(HeapSegHdr);
    if (splitSeqLenght < 0x10) return NULL;

    HeapSegHdr* newSplitHdr = (HeapSegHdr*)((size_t) hshdr + splitLenght + sizeof(HeapSegHdr));
    hshdr->next->last = newSplitHdr;
    newSplitHdr->next = hshdr->next;
    hshdr->next = newSplitHdr;
    newSplitHdr->last = hshdr;
    newSplitHdr->lenght = splitLenght;
    newSplitHdr->free = hshdr->free;
    hshdr->lenght = splitLenght;

    if(g_LastHdr == hshdr) g_LastHdr = newSplitHdr;
    return newSplitHdr;
}

void MEM_HeapSegHdrCombineForward(HeapSegHdr* hshdr) {
    if (hshdr->next == NULL) return;
    if (!hshdr->free) return;

    if (hshdr->next == g_LastHdr) g_LastHdr = hshdr;

    if (hshdr->next->next != NULL) {
        hshdr->next->next->last = hshdr;
    }
    hshdr->lenght += hshdr->next->lenght + sizeof(HeapSegHdr);
    hshdr->next = hshdr->next->next;
}

void MEM_HeapSegHdrCombineBackward(HeapSegHdr *hshdr) {
    if (hshdr->last != NULL && hshdr->last->free)
        MEM_HeapSegHdrCombineForward(hshdr->last);
}
