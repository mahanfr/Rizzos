#ifndef _IO_
#define _IO_

#include <stdint.h>

void IO_OutByte(uint16_t port, uint8_t value);
uint8_t IO_InByte(uint16_t port);
void IO_Wait(void);

#endif
