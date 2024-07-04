#include "interrupts.h"
#include <stdbool.h>
#include <stdint.h>
#include "../panic.h"
#include "../io.h"
#include "../userinput/keyboard.h"
#include "../userinput/mouse.h"
#include "../scheduling/pit.h"

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

__attribute__((interrupt)) void INT_KeyboardIntHandler(struct InterruptFrame* frame) {
    uint8_t scan_code = IO_InByte(0x60);

    UI_Handle_Keyboard(scan_code);

    INT_PIC_EndMaster();
}

__attribute__((interrupt)) void INT_MouseIntHandler(struct InterruptFrame* frame) {
    uint8_t mouse_data = IO_InByte(0x60);
    
    UI_PS2Mouse_Handle(mouse_data);

    INT_PIC_EndSlave();
}

__attribute__((interrupt)) void INT_PIT_Handler(struct InterruptFrame* frame) {
    PIT_Tick();
    INT_PIC_EndMaster();
}

void INT_PIC_Disable(void) {
    IO_OutByte(INT_PIC1_DATA, 0xFF);
    IO_OutByte(INT_PIC2_DATA, 0xFF);
}

void INT_PIC_EndMaster(void){
    IO_OutByte(INT_PIC1_COMMAND, INT_PIC_EOI);
}

void INT_PIC_EndSlave(void){
    IO_OutByte(INT_PIC2_COMMAND, INT_PIC_EOI);
    IO_OutByte(INT_PIC1_COMMAND, INT_PIC_EOI);
}

void INT_PIC_Remap(uint32_t offset1, uint32_t offset2) {
    uint8_t a1,a2;
    a1 = IO_InByte(INT_PIC1_DATA);
    IO_Wait();
    a1 = IO_InByte(INT_PIC2_DATA);
    IO_Wait();

    IO_OutByte(INT_PIC1_COMMAND, INT_ICW1_INIT | INT_ICW1_ICW4);
    IO_Wait();
    IO_OutByte(INT_PIC2_COMMAND, INT_ICW1_INIT | INT_ICW1_ICW4);
    IO_Wait();
    IO_OutByte(INT_PIC1_DATA, offset1);
    IO_Wait();
    IO_OutByte(INT_PIC2_DATA, offset2);
    IO_Wait();
    IO_OutByte(INT_PIC1_DATA, 4);
    IO_Wait();
    IO_OutByte(INT_PIC2_DATA, 2);
    IO_Wait();
    IO_OutByte(INT_PIC1_DATA, INT_ICW4_8086);
    IO_Wait();
    IO_OutByte(INT_PIC2_DATA, INT_ICW4_8086);
    IO_Wait();
    IO_OutByte(INT_PIC1_DATA, a1);
    IO_Wait();
    IO_OutByte(INT_PIC2_DATA, a2);
}
