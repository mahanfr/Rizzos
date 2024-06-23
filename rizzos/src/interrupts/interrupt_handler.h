#ifndef _INTERRUPT_HANDLER_
#define _INTERRUPT_HANDLER_

#include <stdint.h>

void INTH_SetInterruptHandler(uint8_t table_entry, uint64_t pointer_func);

#endif
