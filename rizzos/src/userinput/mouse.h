#ifndef _USERINPUT_MOUSE_
#define _USERINPUT_MOUSE_
#include <stdint.h>

#define MOUSE_POINTER_WIDTH 11
#define MOUSE_POINTER_HEIGHT 18

void UI_PS2Mouse_Init(void);
void UI_PS2Mouse_Handle(uint8_t data);
void UI_PS2Mouse_ProcessPacket(void);

extern uint8_t UI_MousePointer[];

#endif
