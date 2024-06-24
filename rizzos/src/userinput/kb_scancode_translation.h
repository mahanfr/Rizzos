#ifndef _KEYBOARD_SCANCODE_TRANSLATION_
#define _KEYBOARD_SCANCODE_TRANSLATION_

#include <stdint.h>
#include <stdbool.h>

#define UI_KB_LEFT_SHIFT 0x2A
#define UI_KB_RIGHT_SHIFT 0x36
#define UI_KB_ENTER 0x1C
#define UI_KB_BACKSPACE 0x0E
#define UI_KB_SPACEBAR 0x39

extern const char UI_KB_QWERTY_ASCIITable[];
char UI_KB_QWERTY_Translate(uint8_t scancode, bool uppercase);

#endif
