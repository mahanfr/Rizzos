#ifndef _INTERRUPTS_
#define _INTERRUPTS_

#include <stdint.h>

#define INT_PIC1_COMMAND 0x20
#define INT_PIC1_DATA 0x21
#define INT_PIC2_COMMAND 0xA0
#define INT_PIC2_DATA 0xA1
#define INT_PIC_EOI 0x20

#define INT_ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define INT_ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define INT_ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define INT_ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define INT_ICW1_INIT	0x10		/* Initialization - required! */

#define INT_ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define INT_ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define INT_ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define INT_ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define INT_ICW4_SFNM	0x10		/* Special fully nested (not) */
struct InterruptFrame;

__attribute__((interrupt)) void INT_PageFaultHandler(struct InterruptFrame* frame);
__attribute__((interrupt)) void INT_DoubleFaultHandler(struct InterruptFrame* frame);
__attribute__((interrupt)) void INT_GPFaultHandler(struct InterruptFrame* frame);
__attribute__((interrupt)) void INT_KeyboardIntHandler(struct InterruptFrame* frame);
__attribute__((interrupt)) void INT_MouseIntHandler(struct InterruptFrame* frame);

void INT_PIC_Remap(uint32_t offset0, uint32_t offser1);
void INT_PIC_Disable(void);
void INT_PIC_EndMaster(void);
void INT_PIC_EndSlave(void);

#endif
