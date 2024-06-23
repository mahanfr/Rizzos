#ifndef _INTERRUPTS_
#define _INTERRUPTS_

struct InterruptFrame;

__attribute__((interrupt)) void INT_PageFaultHandler(struct InterruptFrame* frame);
__attribute__((interrupt)) void INT_DoubleFaultHandler(struct InterruptFrame* frame);
__attribute__((interrupt)) void INT_GPFaultHandler(struct InterruptFrame* frame);

#endif
