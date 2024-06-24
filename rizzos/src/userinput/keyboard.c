#include "keyboard.h"
#include "kb_scancode_translation.h"
#include "keyboard.h"
#include "../graphics/basic_graphics.h"
#include <stdint.h>
#include <stdbool.h>

static bool g_IsLeftShiftPressed;
static bool g_IsRightShiftPressed;

void UI_Handle_Keyboard(uint8_t scancode) {
    switch (scancode) {
        case UI_KB_LEFT_SHIFT:
            g_IsLeftShiftPressed = true;
            return;
        case UI_KB_LEFT_SHIFT + 0x80:
            g_IsLeftShiftPressed = false;
            return;
        case UI_KB_RIGHT_SHIFT:
            g_IsRightShiftPressed = true;
            return;
        case UI_KB_RIGHT_SHIFT + 0x80:
            g_IsRightShiftPressed = true;
            return;
        case UI_KB_ENTER:
            BG_NextLine();
            return;
        case UI_KB_SPACEBAR:
            BG_PutChar(' ');
            return;
        case UI_KB_BACKSPACE:
            BG_ClearChar();
            return;
    }

    char ascii = UI_KB_QWERTY_Translate(scancode, g_IsLeftShiftPressed || g_IsRightShiftPressed);

    if (ascii != 0) {
       BG_PutChar(ascii);
    }
}
