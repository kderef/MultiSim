#ifndef FONT_C_
#define FONT_C_

#include "raylib.h"
#include "../const.hpp"
#include "../assets.h"

/// draw text with the default font
void DrawTextD(const char* text, int pos_x, int pos_y, float font_size, Color tint) {
    DrawTextEx(font, text, (Vector2){(float)pos_x, (float)pos_y}, font_size, 1.0, tint);
}

// font size
typedef enum {
    FONT_S = 24,
    FONT_M = 30,
    FONT_L = 35,
    FONT_XL = 60,
} FontSize;

#endif