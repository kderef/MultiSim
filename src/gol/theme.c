#ifndef GOL_THEME_C_
#define GOL_THEME_C_

#include "raylib.h"

//! Some themes that can be cycled through.
//! NOTE: secret theme called Bolus can only be activated with B.

typedef struct {
    Color bg_color;
    Color fg_color;
    Color ac_color;
    Color bg_char_color;
    const char* bg_char;
    const char* fg_char;
    const char* name;
} ThemeStyle;

typedef enum {
    Theme_Default = 0,
    Theme_Gruvbox,
    Theme_Matrix,
    Theme_Midnight,
    Theme_Bolus,
} Theme;

void theme_toggle_bolus(Theme* theme) {
    *theme = (*theme == Theme_Bolus)? Theme_Default : Theme_Bolus;
}

void theme_cycle(Theme* theme) {
    *theme = (*theme == Theme_Bolus)?
        Theme_Default : ((*theme) + 1 == Theme_Bolus)?
            Theme_Default : (*theme) + 1;
}

static const ThemeStyle THEMES[] = {
    [Theme_Default] = {
        .name = "Default",
        .bg_color = {0, 0, 0, 255},
        .fg_color = {255, 255, 255, 255},
        .ac_color = {0, 228, 48, 255},
        .bg_char = ".",
        .fg_char = "#",
        .bg_char_color = {130, 130, 130, 255}
    },
    [Theme_Gruvbox] = {
        .name = "Gruvbox",
        .bg_color = {40, 40, 40, 255},
        .fg_color = {235, 219, 178, 255},
        .ac_color = {204, 36, 29, 255},
        .bg_char = ".",
        .fg_char = "@",
        .bg_char_color = {10, 10, 10, 255}
    },
    [Theme_Matrix] = {
        .name = "Matrix",
        .bg_color = {19, 23, 33, 255},
        .fg_color = {50, 198, 3, 255},
        .ac_color = {0, 121, 241, 255},
        .bg_char = "0",
        .fg_char = "1",
        .bg_char_color = {50, 50, 50, 255}
    },
    [Theme_Midnight] = {
        .name = "Midnight",
        .bg_color = {0, 0, 0, 255},
        .fg_color = {200, 200, 200, 255},
        .ac_color = {214, 93, 14, 255},
        .bg_char = "#",
        .fg_char = "",
        .bg_char_color = {30, 30, 30, 255},
    },
    [Theme_Bolus] = {
        .name = "Bolus (cred Fernando)",
        .bg_color = {0, 0, 0, 255},
        .fg_color = {255, 255, 255, 255},
        .ac_color = {0, 228, 48, 255},
        .bg_char = ".",
        .fg_char = "#",
        .bg_char_color = {130, 130, 130, 255}
    }
};

void theme_destructure(Theme theme, ThemeStyle* out) {
    *out = THEMES[theme];
}

#endif