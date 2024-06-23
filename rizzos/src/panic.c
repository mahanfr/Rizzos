#include "panic.h"
#include "graphics/basic_graphics.h"

void Panic(const char *panic_message) {
    BG_SetBgColor(0XFF2B1E19);
    BG_ClearBg();
    BG_ResetCursor();
    print("Rizzos Copyright 2024-2025 Mahan Farzaneh GLP3.0\n\n");
    print("---- KERNEL PANIC ----\n");
    print("Error: %s\n", panic_message);
}
