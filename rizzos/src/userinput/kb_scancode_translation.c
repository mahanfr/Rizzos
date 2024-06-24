#include "kb_scancode_translation.h"
#include <stdint.h>
#include <stdbool.h>

const char UI_KB_QWERTY_ASCIITable[] = {
    0 ,  0 , '1', '2',
    '3', '4', '5', '6',
    '7', '8', '9', '0',
    '-', '=',  0 ,  0 ,
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i',
    'o', 'p', '[', ']',
    0 ,  0 , 'a', 's',
    'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';',
    '\'','`',  0 , '\\',
    'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',',
    '.', '/',  0 , '*',
    0 , ' '
};

char UI_KB_QWERTY_Translate(uint8_t scancode, bool uppercase) {
    if (scancode > 58) return 0;

    if (uppercase){
        return UI_KB_QWERTY_ASCIITable[scancode] - 32;
    }
    else return UI_KB_QWERTY_ASCIITable[scancode];
}
