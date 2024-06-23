#include "interrupts.h"
#include <stdbool.h>
#include "../graphics/basic_graphics.h"

__attribute__((interrupt)) void INT_PageFaultHandler(struct InterruptFrame* frame) {
    (void) frame;
    print("Page Fault Detected!");
    while(true);
}
