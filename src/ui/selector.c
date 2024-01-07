#ifndef SELECTOR_C_
#define SELECTOR_C_

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
}

void selector_deinit(Selector* ptr) {
    game_of_life_deinit(&(ptr->gol));
    dvd_deinit(&(ptr->dvd));
    pong_deinit(&(ptr->pong));
}

SelectedGame selector_title_screen(Selector* s) {
    static bool mouse_clicked;
    static Vector2 mouse_pos;
    // buttons
    static int button_x;
    static const int BUTTONS_SPACING = 80;
    static const int BUTTON_HEIGHT = 50;
    // screen coords
    static int screen_x_center;
    static int screen_y_center;
    // buttons
    static Button gamebutton_gol, gamebutton_dvd, gamebutton_pong;
    global_screen_width = GetScreenWidth();
    global_screen_height = GetScreenHeight();

    screen_x_center = global_screen_width / 2;
    screen_y_center = global_screen_height / 2;

    mouse_clicked = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    mouse_pos = GetMousePosition();

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextD(
        "MultiSim",
        screen_x_center - 145,
        50,
        100.0, RAYWHITE
    );
    DrawTextD(
        "by Kian (Kn-Ht)",
        3,
        global_screen_height - 20, 20.0, GRAY
    );

    // draw the buttons
    button_x = screen_x_center * 0.5;

    gamebutton_gol = button_new(
        "Game of Life",
        screen_x_center - 100, button_x, screen_y_center - BUTTONS_SPACING,
        screen_x_center, BUTTON_HEIGHT
    );

    gamebutton_dvd = button_new(
        "DvD bouncing sim",
        screen_x_center - 120,
        button_x, screen_y_center,
        screen_x_center, BUTTON_HEIGHT
    );
    gamebutton_pong = button_new(
        "Pong",
        screen_x_center - 50,
        button_x,
        screen_y_center + BUTTONS_SPACING,
        screen_x_center, BUTTON_HEIGHT
    );

    // this checks if the button is highlighted/clicked
    // and draws it accordingly
    BUTTON_DRAW(gamebutton_gol, mouse_pos, Selected_GOL);
    BUTTON_DRAW(gamebutton_dvd, mouse_pos, Selected_DVD);
    BUTTON_DRAW(gamebutton_pong, mouse_pos, Selected_PONG);

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
        DrawTextD(TextFormat("FPS: %d", GetFPS()), global_screen_width - 130, global_screen_height - 30, 33.0, GOLD);
    EndDrawing();

    if (next != s->selected) {
        SetWindowTitle(selected_get_window_title(next));
        s->selected = next;
    }
}

#endif