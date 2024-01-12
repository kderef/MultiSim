#ifndef GOL_THEME_C_
#define GOL_THEME_C_

#include "raylib.h"
#include "../const.h"

//! Some themes that can be cycled through.
//! NOTE: secret theme called Bolus can only be activated with B.

typedef struct GolThemeStyle {
    Color bg_color;
    Color fg_color;
    Color ac_color;
    Color bg_char_color;
    const char* bg_char;
    const char* fg_char;
    const char* name;
} GolThemeStyle;

#define ThemeStyle GolThemeStyle


#define THEMESTYLE_NEW(NAME, BG_C, FG_C, C_BG, C_FG, C_AC, C_BC) \
    (ThemeStyle){C_BG, C_FG, C_AC, C_BC, BG_C, FG_C, NAME}

typedef enum {
    GOLTheme_Default = 0,
    GOLTheme_Gruvbox,
    GOLTheme_Matrix,
    GOLTheme_Midnight,
    GOLTheme_Bolus,
} GolTheme;

#define Theme GolTheme

static inline void theme_toggle_bolus(Theme* theme) {
    *theme = (*theme == GOLTheme_Bolus)? GOLTheme_Default : GOLTheme_Bolus;
}

static inline void theme_cycle(Theme* theme) {
    *theme = (*theme == GOLTheme_Bolus)?
        GOLTheme_Default : ((*theme) + 1 == GOLTheme_Bolus)?
            GOLTheme_Default : (*theme) + 1;
}

static const ThemeStyle THEMES[] = {
    [GOLTheme_Default] = THEMESTYLE_NEW(
        "Default", ".", "#",
        color(0, 0, 0),
        color(255, 255, 255),
        color(0, 228, 48),
        color(130, 130, 130)
    ),
    [GOLTheme_Gruvbox] = THEMESTYLE_NEW(
        "Gruvbox", ".", "@",
        color(40, 40, 40),
        color(235, 219, 178),
        color(204, 36, 29),
        color(10, 10, 10)
    ),
    [GOLTheme_Matrix] = THEMESTYLE_NEW(
        "Matrix", "0", "1",
        color(19, 23, 33),
        color(50, 198, 3),
        color(0, 121, 241),
        color(50, 50, 50)
    ),
    [GOLTheme_Midnight] = THEMESTYLE_NEW(
        "Midnight", "#", "",
        color(0, 0, 0),
        color(200, 200, 200),
        color(214, 93, 14),
        color(30, 30, 30)
    ),
    [GOLTheme_Bolus] = THEMESTYLE_NEW(
        "Bolus (cred Fernando)", ".", "#",
        color(0, 0, 0),
        color(255, 255, 255),
        color(0, 228, 48),
        color(130, 130, 130)
    )
};

static inline void theme_destructure(Theme theme, ThemeStyle* out) {
    *out = THEMES[theme];
}

#undef ThemeStyle
#undef Theme
#undef THEMESTYLE_NEW
#endif