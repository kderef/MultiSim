#ifndef FONT_C_
#define FONT_C_

#include "raylib.h"
#include "../const.h"
#include <stdio.h>

// the default font, used in the entire application
static Font font;
INCBIN(default_font, "assets/CnC-RedAlert.ttf");

void load_default_font(void) {
    font = LoadFontFromMemory(
        ".ttf", default_font_data, default_font_size,
        100, NULL, 0
    );
}

/// draw text with the default font
void DrawTextD(const char* text, int pos_x, int pos_y, float font_size, Color tint) {
    DrawTextEx(font, text, (Vector2){pos_x, pos_y}, font_size, 1.0, tint);
}

// font size
typedef enum {
    FONT_S = 24,
    FONT_M = 30,
    FONT_L = 35,
    FONT_XL = 60,
} FontSize;

#endif