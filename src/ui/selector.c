#ifndef SELECTOR_C_
#define SELECTOR_C_

// NOTE: idea: Github icon in the corner to take you to the repository

#include "raygui_incl.h"
#include "raygui_style_dark.h"
#include "splashtext.c"
#include "../const.h"
#include "../gol/game.c"
#include "../gamestate.h"
#include "../dvd/dvd.c"
#include "../pong/pong.c"
#include "../minesweeper/minesweeper.c"

INCBIN(menu_img, "../assets/menu-bg.png");

typedef struct Selector {
    GameOfLife* gol;
    Dvd* dvd;
    Pong* pong;

    SelectedGame selected;
    Texture2D background;
} Selector;

// create new selector
Selector* selector_alloc(void) {
    Selector* s = (Selector*)malloc(sizeof(Selector));

    if (!s) panic("Failed selector_alloc");

    s->gol = gol_alloc();
    s->dvd = dvd_alloc();
    s->pong = pong_alloc();
    s->selected = Selected_None;

    Image bg_img = LoadImageFromMemory(
        ".png", menu_img_data, menu_img_size
    );
    s->background = LoadTextureFromImage(bg_img);
    UnloadImage(bg_img);

    GuiLoadStyleDark();

    return s;
}

// show the title screen and check if a button is pressed
static inline SelectedGame title_screen(Selector* s) {
    static int screen_x_center, screen_y_center, button_x, button_y;
    static float passed_time;
    static float text_zoom_offset;

    passed_time = (passed_time > 0.5f) ? 0.0f : passed_time + GetFrameTime();
    text_zoom_offset = SPLASH_TEXT_BASE + sinf(passed_time * 12);

    #define BUTTONS_SPACING 80
    #define BUTTON_HEIGHT 50

    screen_x_center = global_state.screen_w / 2;
    screen_y_center = global_state.screen_h / 2;

    if (IsKeyPressed(KEY_F5)) global_state.show_fps = !global_state.show_fps;

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTextureRec(
        s->background, rect(0, 0, global_state.screen_w, global_state.screen_h),
        VEC2_ZERO, WHITE
    );

    DrawTextD(
        "MultiSim",
        screen_x_center - 145,
        50,
        100.0, LIGHTGRAY
    );

    DrawTextPro(
        font, splash_text, vec2(screen_x_center + 140, 120),
        splash_text_len_half, 340.0, text_zoom_offset, 1.0, GOLD
    );

    if (GuiLabelButton(rect(0, 0, 300, 20), "#171# repository")) {
        OpenURL(GIT_URL);
    }
    GuiDrawText("Version " VERSION, rect(0, global_state.screen_h - 60, 200, 20), TEXT_ALIGN_LEFT, GRAY);
    GuiDrawText("By Kian (kn-ht)", rect(0, global_state.screen_h - 40, 200, 20), TEXT_ALIGN_LEFT, GRAY);
    GuiDrawText("Written using         " RAYLIB_VERSION, rect(0, global_state.screen_h - 20, 300, 20), TEXT_ALIGN_LEFT, GRAY);
    if (GuiLabelButton(rect(133, global_state.screen_h - 20, 60, 20), "Raylib")) {
        OpenURL("https://github.com/raysan5/raylib");
    }

    // draw the buttons
    button_y = screen_y_center - BUTTONS_SPACING;
    button_x = screen_x_center / 2;

    if (GuiButton(rect(button_x, button_y, screen_x_center, BUTTON_HEIGHT), "Game of Life")) {
        return Selected_GOL;
    }
    if (GuiButton(rect(button_x, button_y + BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "DvD Bouncy")) {
        return Selected_DVD;
    }
    if (GuiButton(rect(button_x, button_y + 2 * BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "Pong")) {
        return Selected_PONG;
    }

    return Selected_None;
}

// updates the current selected game
void selector_update(Selector* s) {
    SelectedGame next_game = Selected_None;

    update_global_state();

    switch (s->selected) {
    case Selected_None: {
        next_game = title_screen(s);
    } break;
    case Selected_GOL: {
        next_game = gol_update(s->gol);
    } break;
    case Selected_DVD: {
        next_game = dvd_update(s->dvd);
    } break;
    case Selected_PONG: {
        next_game = pong_update(s->pong);
    } break;
    }

    g_sprintf("FPS: %d", GetFPS());
    if (global_state.show_fps) DrawTextD(global_text_buf, 3, 0, 33.0, GOLD);
    EndDrawing();

    if (next_game != s->selected) {
        SetWindowTitle(selected_get_window_title(next_game));
        s->selected = next_game;
    }
}

void selector_free(Selector* s) {
    UnloadTexture(s->background);
    gol_free(s->gol);
    dvd_free(s->dvd);
    pong_free(s->pong);

    free(s);
}

#undef BUTTON_HEIGHT
#undef BUTTONS_SPACING
#endif