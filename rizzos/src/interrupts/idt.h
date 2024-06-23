#ifndef _IDTABLE_
#define _IDTABLE_

#include <stdint.h>

typedef enum {
    IDT_TDPLP_INTERRUPT_GATE = 0x8E, // 0b10001110 
    IDT_TDPLP_CALL_GATE      = 0x8B, // 0b10001100 
    IDT_TDPLP_TRAP_GATE      = 0x8F, // 0b10001111 
} IDTTypeDplP;

typedef struct {
    uint16_t offset0;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_dpl_p;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t reserved;
} IDTDescEntry;

typedef struct {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) IDTR;

void IDT_InitInterruptTable(void);
IDTR IDT_GetInterruptTable(void);
void IDT_SetOffset(IDTDescEntry* ide, uint64_t offset);
uint64_t IDT_GetOffset(IDTDescEntry* ide);

#endif
