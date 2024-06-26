#ifndef _USERINPUT_MOUSE_
#define _USERINPUT_MOUSE_

#include <stdint.h>
void UI_PS2Mouse_Init(void);
void UI_PS2Mouse_Handle(uint8_t data);
void UI_PS2Mouse_ProcessPacket(void);

#endif
