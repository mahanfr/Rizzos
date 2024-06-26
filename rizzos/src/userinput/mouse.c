#include "mouse.h"
#include "../io.h"
#include <stdbool.h>
#include <stdint.h>
#include "../../common/graphics.h"
#include "../graphics/basic_graphics.h"

#define PS2Leftbutton 0b00000001
#define PS2Middlebutton 0b00000010
#define PS2Rightbutton 0b00000100
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

static void MouseWait(void) {
    uint64_t timeout = 100000;
    while(timeout--) {
        if((IO_InByte(0x64) & 0b10) == 0) {
            return;
        }
    }
}

static void MouseWaitInput(void) {
    uint64_t timeout = 100000;
    while(timeout--) {
        if((IO_InByte(0x64) & 1)) {
            return;
        }
    }
}

static void MouseWrite(uint8_t value) {
    MouseWait();
    IO_OutByte(0x64, 0xD4);
    MouseWait();
    IO_OutByte(0x60, value);

}

static uint8_t MouseRead(void) {
    MouseWaitInput();
    return IO_InByte(0x60);
}

static uint8_t g_MouseCycle = 0;
static uint8_t g_MousePacket[4];
static bool g_IsMousePacketReady = false;
void UI_PS2Mouse_Handle(uint8_t data) {
    switch(g_MouseCycle) {
        case 0:
            if (g_IsMousePacketReady) break;
            if ((data & 0b00001000) == 0) break;
            g_MousePacket[0] = data;
            g_MouseCycle++;
            break;
        case 1:
            if (g_IsMousePacketReady) break;
            g_MousePacket[1] = data;
            g_MouseCycle++;
            break;
        case 2:
            if (g_IsMousePacketReady) break;
            g_MousePacket[2] = data;
            g_IsMousePacketReady = true;
            g_MouseCycle = 0;
            break;
    }
}

static Point MousePosition;
void UI_PS2Mouse_ProcessPacket(void) {
    if (!g_IsMousePacketReady) return;
    
    bool xNegative, yNegative, xOverflow, yOverflow;

    xNegative = (g_MousePacket[0] & PS2XSign);
    yNegative = (g_MousePacket[0] & PS2YSign);
    xOverflow = (g_MousePacket[0] & PS2XOverflow);
    yOverflow = (g_MousePacket[0] & PS2YOverflow);

    if (!xNegative) {
        MousePosition.x += g_MousePacket[1];
        if (xOverflow) {
            MousePosition.x += 255 * 2;
        }
    } else {
        g_MousePacket[1] = 256 - g_MousePacket[1];
        MousePosition.x -= g_MousePacket[1];
        if (xOverflow) {
            MousePosition.x -= 255 * 2;
        }
    }

    if (!yNegative) {
        MousePosition.y -= g_MousePacket[2];
        if (yOverflow) {
            MousePosition.y -= 255;
        }
    } else {
        g_MousePacket[2] = 256 - g_MousePacket[2];
        MousePosition.y += g_MousePacket[2];
        if (yOverflow) {
            MousePosition.y += 255;
        }
    }
    
    if (MousePosition.x < 0) {
        MousePosition.x = 0;
    }
    if (MousePosition.x > BG_GetScreenWidth() - 8) {
        MousePosition.x = BG_GetScreenWidth() - 8;
    }
    if (MousePosition.y < 0) {
        MousePosition.y = 0;
    }
    if (MousePosition.y > BG_GetScreenHeight() - 16) {
        MousePosition.y = BG_GetScreenHeight() - 16;
    }

    BG_SetCursor(MousePosition.x, MousePosition.y);
    BG_PutChar('a');
    //print("%d - %d\n", MousePosition.x, MousePosition.y);

    g_IsMousePacketReady = false;
}

void UI_PS2Mouse_Init(void) {
    MouseWait();
    IO_OutByte(0x64, 0xA8);

    MouseWait();
    IO_OutByte(0x64, 0x20);
    MouseWaitInput();

    uint8_t status = IO_InByte(0x60);
    status |= 2;
    MouseWait();
    IO_OutByte(0x64, 0x60);
    MouseWait();
    IO_OutByte(0x60, status);
    
    MouseWrite(0xF6);
    MouseWaitInput();
    MouseRead();
  
    // setting scaling
    MouseWrite(0xE7);
    MouseWaitInput();
    MouseRead();

    // setting resolution
    MouseWrite(0xE8);
    MouseWaitInput();
    MouseRead();
    MouseWrite(0b11);
    MouseWaitInput();
    MouseRead();

    // Setting the sample rate to 100
    MouseWrite(0xF3);
    MouseWaitInput();
    MouseRead();
    MouseWrite(100);
    MouseWaitInput();
    MouseRead();

    MouseWrite(0xF4);
    MouseWaitInput();
    MouseRead();
}
