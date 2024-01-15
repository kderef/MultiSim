#ifndef SELECTOR_C_
#define SELECTOR_C_

#include "raygui_incl.h"
#include "raygui_style_dark.h"
#include "../const.h"
#include "../gol/game.c"
#include "../gamestate.h"
#include "../dvd/dvd.c"
#include "../pong/pong.c"
#include "../minesweeper/minesweeper.c"

typedef struct Selector {
    GameOfLife gol;
    Dvd dvd;
    Pong pong;
    Minesweeper minesweeper;

    SelectedGame selected;
} Selector;

// the global selector, used in the selector_update() method.
// intialized upon selector_init()
static Selector _static_selector;

// intialize _static_selector
void selector_init(void) {
    _static_selector.gol = gol_new();
    _static_selector.dvd = dvd_new();
    _static_selector.pong = pong_new();
    _static_selector.minesweeper = minesweeper_new();
    _static_selector.selected = Selected_None;

    GuiLoadStyleDark();
}

// show the title screen and check if a button is pressed
static inline SelectedGame title_screen() {
    static int screen_x_center, screen_y_center, button_x;

    #define BUTTONS_SPACING 80
    #define BUTTON_HEIGHT 50

    screen_x_center = global_state.screen_w  / 2;
    screen_y_center = global_state.screen_h  / 2;

    if (IsKeyPressed(KEY_F5)) global_state.show_fps = !global_state.show_fps;

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextD(
        "MultiSim",
        screen_x_center - 145,
        50,
        100.0, RAYWHITE
    );

    GuiDrawText("By Kian (kn-ht)", rect(0, global_state.screen_h - 20, 200, 20), TEXT_ALIGN_LEFT, GRAY);

    // draw the buttons
    button_x = screen_x_center / 2;

    if (GuiButton(rect(button_x, screen_y_center - BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "Game of Life")) {
        return Selected_GOL;
    }
    if (GuiButton(rect(button_x, screen_y_center, screen_x_center, BUTTON_HEIGHT), "DvD Bouncy")) {
        return Selected_DVD;
    }
    if (GuiButton(rect(button_x, screen_y_center + BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "Pong")) {
        return Selected_PONG;
    }
    if (GuiButton(rect(button_x, screen_y_center + 2*BUTTONS_SPACING, screen_x_center, BUTTON_HEIGHT), "Minesweeper")) {
        return Selected_MINESWEEPER;
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
        case Selected_MINESWEEPER: {
            next_game = minesweeper_update(&(_static_selector.minesweeper));
        } break;
    }

    if (global_state.show_fps) DrawTextD(TextFormat("FPS: %d", GetFPS()), global_state.screen_h - 130, 0, 33.0, GOLD);
    EndDrawing();

    if (next_game != _static_selector.selected) {
        SetWindowTitle(selected_get_window_title(next_game));
        _static_selector.selected = next_game;
    }
}

#endif