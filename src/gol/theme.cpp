#ifndef GOL_THEME_C_
#define GOL_THEME_C_

#include "raylib.h"

//! Some themes that can be cycled through.
//! NOTE: secret theme called Bolus can only be activated with B.

constexpr Color COLOR_NONE = (Color){0, 0, 0, 0};

struct ThemeStyle {
    Color bg_color;
    Color fg_color;
    Color ac_color;
    Color bg_char_color;
    const char* bg_char;
    const char* fg_char;
    const char* name;

    constexpr ThemeStyle() :
        name(""), bg_char(""), fg_char(""), bg_color(COLOR_NONE), fg_color(COLOR_NONE),
        ac_color(COLOR_NONE), bg_char_color(COLOR_NONE) {}

    constexpr ThemeStyle(
        const char* name_, const char* bg_char_, const char* fg_char_,
        Color bg_color_, Color fg_color_, Color ac_color_, Color bg_char_color_
    ) : name(name_), bg_char(bg_char_), fg_char(fg_char_), bg_color(bg_color_),
        fg_color(fg_color_), ac_color(ac_color_), bg_char_color(bg_char_color_)
    {}
};

enum Theme {
    Theme_Default,
    Theme_Gruvbox,
    Theme_Matrix,
    Theme_Midnight,
    Theme_Bolus,
};

void theme_toggle_bolus(Theme* theme) {
    *theme = (*theme == Theme_Bolus)? Theme_Default : Theme_Bolus;
}

void theme_cycle(Theme* theme) {
    *theme = (*theme == Theme_Bolus)?
        Theme_Default : ((*theme) + 1 == Theme_Bolus)?
            Theme_Default : (Theme)((*theme) + 1);
}

constexpr ThemeStyle THEMES[] = {
    ThemeStyle(
        "Default", ".", "#",
        {0, 0, 0, 255},
        {255, 255, 255, 255},
        {0, 228, 48, 255},
        {130, 130, 130, 255}
    ),
    ThemeStyle(
        "Gruvbox", ".", "@",
        {40, 40, 40, 255},
        {235, 219, 178, 255},
        {204, 36, 29, 255},
        {10, 10, 10, 255}
    ),
    ThemeStyle(
        "Matrix", "0", "1",
        {19, 23, 33, 255},
        {50, 198, 3, 255},
        {0, 121, 241, 255},
        {50, 50, 50, 255}
    ),
    ThemeStyle(
        "Midnight", "#", "",
        {0, 0, 0, 255},
        {200, 200, 200, 255},
        {214, 93, 14, 255},
        {30, 30, 30, 255}
    ),
    ThemeStyle(
        "Bolus (cred Fernando)", ".", "#",
        {0, 0, 0, 255},
        {255, 255, 255, 255},
        {0, 228, 48, 255},
        {130, 130, 130, 255}
    )
};

void theme_destructure(Theme theme, ThemeStyle* out) {
    *out = THEMES[theme];
}

#endif