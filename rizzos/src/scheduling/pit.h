#ifndef _SCHEDULING_PIT_H_
#define _SCHEDULING_PIT_H_

#include <stdint.h>
extern double TimeSinceBoot;
#define PIT_BASE_FREQUENCY (uint64_t)1193182

void PIT_Sleepd(double seconds);
void PIT_Sleep(uint64_t milliseconds);

void PIT_SetDivisor(uint16_t divisor);
uint64_t PIT_GetFrequency(void);
void PIT_SetFrequency(uint64_t frequency);
void PIT_Tick(void);

#endif
