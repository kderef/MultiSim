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
#include <time.h>

typedef struct Selector {
    GameOfLife gol;
    Dvd dvd;
    Pong pong;
    Minesweeper minesweeper;

    SelectedGame selected;
    GameState state;
} Selector;

Selector selector_new() {
    Selector s;
    s.gol = gol_new();
    s.dvd = dvd_new();
    s.pong = pong_new();
    s.minesweeper = minesweeper_new();
    s.selected = Selected_None;

    GuiLoadStyleDark();
    
    return s;
}

static inline SelectedGame title_screen() {
    static int screen_x_center, screen_y_center, button_x;

    #define BUTTONS_SPACING 80
    #define BUTTON_HEIGHT 50

    global_screen_width = GetScreenWidth();
    global_screen_height = GetScreenHeight();

    screen_x_center = global_screen_width / 2;
    screen_y_center = global_screen_height / 2;

    if (IsKeyPressed(KEY_F5)) global_show_fps = !global_show_fps;

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextD(
        "MultiSim",
        screen_x_center - 145,
        50,
        100.0, RAYWHITE
    );

    GuiDrawText("By Kian (kn-ht)", rect(0, global_screen_height - 20, 200, 20), TEXT_ALIGN_LEFT, GRAY);

    // draw the buttons
    button_x = screen_x_center * 0.5;

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

void selector_update(Selector* s) {
    static SelectedGame next_game;

    global_screen_width = GetScreenWidth();
    global_screen_height = GetScreenHeight();

    switch (s->selected) {
        case Selected_None: {
            next_game = title_screen();
        } break;
        case Selected_GOL: {
            next_game = gol_update(&(s->gol));
        } break;
        case Selected_DVD: {
            next_game = dvd_update(&(s->dvd));
        } break;
        case Selected_PONG: {
            next_game = pong_update(&(s->pong));
        } break;
        case Selected_MINESWEEPER: {
            next_game = minesweeper_update(&(s->minesweeper));
        } break;
    }

    if (global_show_fps) DrawTextD(TextFormat("FPS: %d", GetFPS()), global_screen_width - 130, 0, 33.0, GOLD);
    EndDrawing();

    if (next_game != s->selected) {
        SetWindowTitle(selected_get_window_title(next_game));
        s->selected = next_game;
    }
}

#endif