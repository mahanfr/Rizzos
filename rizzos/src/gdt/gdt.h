#ifndef _GDTable_
#define _GDTable_

#include <stdint.h>
typedef struct {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) GDTDescriptor;

typedef struct {
    uint16_t limit0;
    uint16_t base0;
    uint8_t base1;
    uint8_t accessByte;
    uint8_t limit1_flag;
    uint8_t base2;
} __attribute__((packed)) GDTEntry;

typedef struct {
    GDTEntry null;
    GDTEntry kernelCode;
    GDTEntry kernelData;
    GDTEntry userNull;
    GDTEntry userCode;
    GDTEntry userData;
} __attribute__((packed)) GDT
__attribute__((aligned(0x1000)));

extern GDT g_DefaultGDT;
extern void LoadGDT(GDTDescriptor* gdtDescriptor);

#endif
