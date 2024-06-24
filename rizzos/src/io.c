#include "io.h"
#include <stdint.h>

void IO_OutByte(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd" (port));
}

uint8_t IO_InByte(uint16_t port) {
    uint8_t ret_value;
    asm volatile ("inb %1, %0"
            : "=a" (ret_value)
            : "Nd" (port));
    return ret_value;
}

void IO_Wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}
