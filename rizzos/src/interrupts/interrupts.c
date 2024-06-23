#include "interrupts.h"
#include <stdbool.h>
#include "../panic.h"

__attribute__((interrupt)) void INT_PageFaultHandler(struct InterruptFrame* frame) {
    (void) frame;
    Panic("Page Fault Detected!");
    while(true);
}

__attribute__((interrupt)) void INT_DoubleFaultHandler(struct InterruptFrame *frame) {
    (void) frame;
    Panic("Double Fault Detected!");
    while(true);
}

__attribute__((interrupt)) void INT_GPFaultHandler(struct InterruptFrame* frame) {
    (void) frame;
    Panic("General Protection Fault Detected!");
    while(true);
}
