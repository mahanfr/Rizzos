#include "interrupt_handler.h"
#include "idt.h"

void INTH_SetInterruptHandler(uint8_t table_entry, uint64_t pointer_func) {
    IDTR idtr = IDT_GetInterruptTable();
    IDTDescEntry* idte = (IDTDescEntry*)(idtr.offset + table_entry * sizeof(IDTDescEntry));
    IDT_SetOffset(idte, pointer_func);
    idte->type_dpl_p = IDT_TDPLP_INTERRUPT_GATE;
    idte->selector = 0x08;
}
