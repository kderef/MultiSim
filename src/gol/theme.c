#ifndef GOL_THEME_C_
#define GOL_THEME_C_

#include "raylib.h"

//! Some themes that can be cycled through.
//! NOTE: secret theme called Bolus can only be activated with B.

typedef struct {
    Color bg;
    Color fg;
    Color ac;
} ThemeStyle;

typedef enum {
    Theme_Default = 0,
    Theme_Gruvbox,
    Theme_Matrix,
    Theme_Midnight,
    Theme_Bolus,
} Theme;

static const char* theme_names[] = {
    [Theme_Default] = "Default",
    [Theme_Gruvbox] = "Gruvbox",
    [Theme_Matrix] = "Matrix",
    [Theme_Midnight] = "Midnight",
    [Theme_Bolus] = "Bolus (cred Fernando!)"
};

static inline const char* theme_to_str(Theme theme) {
    return theme_names[theme];
}

void theme_toggle_bolus(Theme* theme) {
    *theme = (*theme == Theme_Bolus)? Theme_Default : Theme_Bolus;
}

void theme_cycle(Theme* theme) {
    *theme = (*theme == Theme_Bolus)?
        Theme_Default : ((*theme) + 1 == Theme_Bolus)?
            Theme_Default : (*theme) + 1;
}

// store the themes statically
static ThemeStyle _DEFAULT = {
    .bg = {0, 0, 0, 255},
    .fg = {255, 255, 255, 255},
    .ac = {0, 228, 48, 255}
};
static ThemeStyle _GRUVBOX = {
    .bg = {40, 40, 40, 255},
    .fg = {235, 219, 178, 255},
    .ac = {204, 36, 29, 255}
};
static ThemeStyle _MATRIX = {
    .bg = {19, 23, 33, 255},
    .fg = {50, 198, 3, 255},
    .ac = {0, 121, 241, 255}
};
static ThemeStyle _MIDNIGHT = {
    .bg = {13, 16, 23, 255},
    .fg = {255, 255, 255, 255},
    .ac = {214, 93, 14, 255}
};

void theme_destructure(Theme theme, Color* bg, Color* fg, Color* ac) {
    switch (theme) {
        case Theme_Default: // NOTE fallthrough
        case Theme_Bolus: {
            *bg = _DEFAULT.bg;
            *fg = _DEFAULT.fg;
            *ac = _DEFAULT.ac;
        }
        break;

        case Theme_Gruvbox: {
            *bg = _GRUVBOX.bg;
            *fg = _GRUVBOX.fg;
            *ac = _GRUVBOX.ac;
        } break;

        case Theme_Matrix: {
            *bg = _MATRIX.bg;
            *fg = _MATRIX.fg;
            *ac = _MATRIX.ac;
        } break;

        case Theme_Midnight: {
            *bg = _MIDNIGHT.bg;
            *fg = _MIDNIGHT.fg;
            *ac = _MIDNIGHT.ac;
        } break;
    }
}

#endif