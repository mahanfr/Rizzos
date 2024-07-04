#include "pit.h"
#include <stdint.h>
#include "../io.h"

double TimeSinceBoot = 0;

static uint16_t g_Divisor = 65535;

void PIT_Sleepd(double seconds) {
    double startTime = TimeSinceBoot;
    while(TimeSinceBoot < startTime + seconds) {
        asm("hlt");
    }
}

void PIT_Sleep(uint64_t miliseconds) {
    PIT_Sleepd((double) miliseconds / 1000);
}

void PIT_SetDivisor(uint16_t divisor) {
    if (divisor < 100) divisor = 100;

    g_Divisor = divisor;

    IO_OutByte(0x40, (uint8_t)(divisor & 0x00ff));
    IO_Wait();
    IO_OutByte(0x40, (uint8_t)(divisor & 0xff00) >> 8);
}

uint64_t PIT_GetFrequency(void) {
    return PIT_BASE_FREQUENCY / g_Divisor;
}

void PIT_SetFrequency(uint64_t frequency) {
    PIT_SetDivisor(PIT_BASE_FREQUENCY / frequency);
}

void PIT_Tick() {
    TimeSinceBoot += 1 / (double) PIT_GetFrequency();
}
