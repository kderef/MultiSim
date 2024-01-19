#ifndef SELECTOR_C_
#define SELECTOR_C_

#include "raygui_incl.h"
#include "raygui_style_dark.h"
#include "splashtext.c"
#include "../const.h"
#include "../gol/game.c"
#include "../gamestate.h"
#include "../dvd/dvd.c"
#include "../pong/pong.c"
#include "../minesweeper/minesweeper.c"
#include "../galaxy/galaxy.c"

INCBIN(menu_img, "../assets/menu-bg.png");

typedef struct Selector {
    GameOfLife gol;
    Dvd dvd;
    Pong pong;
    Galaxy galaxy;

    SelectedGame selected;
    Texture2D background;
} Selector;

// the global selector, used in the selector_update() method.
// intialized upon selector_init()
static Selector _static_selector;

// intialize _static_selector
void selector_init(void) {
    _static_selector.gol = gol_new();
    _static_selector.dvd = dvd_new();
    _static_selector.pong = pong_new();
    _static_selector.galaxy = galaxy_new();
    _static_selector.selected = Selected_None;

    Image bg_img = LoadImageFromMemory(
        ".png", menu_img_data, menu_img_size
    );
    _static_selector.background = LoadTextureFromImage(bg_img);
    UnloadImage(bg_img);

    GuiLoadStyleDark();
}

// show the title screen and check if a button is pressed
static inline SelectedGame title_screen() {
    static int screen_x_center, screen_y_center, button_x, button_y;
    static float passed_time;
    static float text_zoom_offset;

    passed_time = (passed_time > 0.5f)? 0.0f : passed_time + GetFrameTime();
    text_zoom_offset = SPLASH_TEXT_BASE + sinf(passed_time * 12);

    #define BUTTONS_SPACING 80
    #define BUTTON_HEIGHT 50

    screen_x_center = global_state.screen_w  / 2;
    screen_y_center = global_state.screen_h  / 2;

    if (IsKeyPressed(KEY_F5)) global_state.show_fps = !global_state.show_fps;

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTextureRec(
        _static_selector.background, rect(0, 0, global_state.screen_w, global_state.screen_h),
        vec2(0, 0), WHITE
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

    GuiDrawText("Version " VERSION, rect(0, global_state.screen_h - 60, 200, 20), TEXT_ALIGN_LEFT, GRAY);
    GuiDrawText("By Kian (kn-ht)", rect(0, global_state.screen_h - 40, 200, 20), TEXT_ALIGN_LEFT, GRAY);
    GuiDrawText("Written using Raylib " RAYLIB_VERSION, rect(0, global_state.screen_h - 20, 300, 20), TEXT_ALIGN_LEFT, GRAY);

    // draw the buttons
    button_y = screen_y_center - BUTTONS_SPACING;
    button_x = screen_x_center / 2;

    if (GuiButton(rect(button_x, button_y, screen_x_center, BUTTON_HEIGHT), "Game of Life")) {
        return Selected_GOL;
    }
    if (GuiButton(rect(button_x, button_y + BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "DvD Bouncy")) {
        return Selected_DVD;
    }
    if (GuiButton(rect(button_x, button_y + 2*BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "Pong")) {
        return Selected_PONG;
    }
    if (GuiButton(rect(button_x, button_y + 3*BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "[WIP] Galaxy")) {
        return Selected_GALAXY;
    }

    return Selected_None;
}

// updates the current selected game
void selector_update(void) {
    static SelectedGame next_game;

    global_state.screen_w = GetScreenWidth();
    global_state.screen_h = GetScreenHeight();
    global_state.left_mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    global_state.right_mouse_down = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    global_state.mouse_pos = GetMousePosition();
    global_state.mouse_delta = GetMouseDelta();
    global_state.mouse_wheel_move = GetMouseWheelMove();

    switch (_static_selector.selected) {
        case Selected_None: {
            next_game = title_screen();
        } break;
        case Selected_GOL: {
            next_game = gol_update(&(_static_selector.gol));
        } break;
        case Selected_DVD: {
            next_game = dvd_update(&(_static_selector.dvd));
        } break;
        case Selected_PONG: {
            next_game = pong_update(&(_static_selector.pong));
        } break;
        case Selected_GALAXY: {
            next_game = galaxy_update(&(_static_selector.galaxy));
        } break;
    }

    g_sprintf("FPS: %d", GetFPS());
    if (global_state.show_fps) DrawTextD(global_text_buf, 3, 0, 33.0, GOLD);
    EndDrawing();

    if (next_game != _static_selector.selected) {
        SetWindowTitle(selected_get_window_title(next_game));
        _static_selector.selected = next_game;
    }
}

void selector_deinit(void) {
    UnloadTexture(_static_selector.background);
    game_of_life_deinit(&_static_selector.gol);
    dvd_deinit(&_static_selector.dvd);
    pong_deinit(&_static_selector.pong);
    galaxy_deinit(&_static_selector.galaxy);
}

#endif