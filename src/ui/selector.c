#ifndef SELECTOR_C_
#define SELECTOR_C_

#include "raygui_incl.h"
#include "raygui_style_dark.h"
#include "../const.h"
#include "../gol/game.c"
#include "../gamestate.c"
#include "../dvd/dvd.c"
#include "../pong/pong.c"
#include "../ui/button.c"
#include <time.h>

typedef struct {
    // games
    GameOfLife gol;
    Dvd dvd;
    Pong pong;

    SelectedGame selected;
    GameState state;
} Selector;

void selector_init(Selector* to_init) {
    to_init->gol = game_of_life_new();
    to_init->dvd = dvd_new();
    to_init->pong = pong_new();
    to_init->state = GameState_Help;
    to_init->selected = Selected_None;

    // setup raygui
    GuiLoadStyleDark();
}

void selector_deinit(Selector* ptr) {
    game_of_life_deinit(&(ptr->gol));
    dvd_deinit(&(ptr->dvd));
    pong_deinit(&(ptr->pong));
}

SelectedGame selector_title_screen(Selector* s) {
    // buttons
    static int button_x;
    static const int BUTTONS_SPACING = 80;
    static const int BUTTON_HEIGHT = 50;
    // screen coords
    static int screen_x_center;
    static int screen_y_center;

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

    return Selected_None;
}

void selector_update(Selector* s) {
    static SelectedGame next;

    global_screen_width = GetScreenWidth();
    global_screen_height = GetScreenHeight();

    switch (s->selected) {
        case Selected_None: {
            next = selector_title_screen(s);
        } break;
        case Selected_GOL: {
            next = game_of_life_update(&(s->gol));
        } break;
        case Selected_DVD: {
            next = dvd_update(&(s->dvd));
        } break;
        case Selected_PONG: {
            next = pong_update(&(s->pong));
        } break;
    }

    if (global_show_fps)
        DrawTextD(TextFormat("FPS: %d", GetFPS()), global_screen_width - 130, 0, 33.0, GOLD);
    EndDrawing();

    if (next != s->selected) {
        SetWindowTitle(selected_get_window_title(next));
        s->selected = next;
    }
}

#endif