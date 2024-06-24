#include "keyboard.h"
#include "kb_scancode_translation.h"
#include "keyboard.h"
#include "../graphics/basic_graphics.h"
#include <stdint.h>
#include <stdbool.h>

void UI_Handle_Keyboard(uint8_t scancode) {
    char ascii = UI_KB_QWERTY_Translate(scancode, false);

    if (ascii != 0) {
       BG_PutChar(ascii);
    }
}
