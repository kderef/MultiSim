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

    int screen_width;
    int screen_height;
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

    s->screen_width = GetScreenWidth();
    s->screen_height = GetScreenHeight();
    screen_x_center = s->screen_width * 0.5;
    screen_y_center = s->screen_height * 0.5;

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
        s->screen_height - 20, 20.0, GRAY
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

    if (button_is_hovered(&gamebutton_gol, &mouse_pos)) {
        if (mouse_clicked) {
            button_draw_clicked(&gamebutton_gol);
            return Selected_GOL;
        }
        button_draw_hovered(&gamebutton_gol);
    }
    else button_draw(&gamebutton_gol);
    
    if (button_is_hovered(&gamebutton_dvd, &mouse_pos)) {
        if (mouse_clicked) {
            button_draw_clicked(&gamebutton_dvd);
            return Selected_DVD;
        }
        button_draw_hovered(&gamebutton_dvd);
    }
    else button_draw(&gamebutton_dvd);

    if (button_is_hovered(&gamebutton_pong, &mouse_pos)) {
        if (mouse_clicked) {
            button_draw_clicked(&gamebutton_pong);
            return Selected_PONG;
        }
        button_draw_hovered(&gamebutton_pong);
    }
    else button_draw(&gamebutton_pong);
    

    EndDrawing();
    return Selected_None;
}

void selector_update(Selector* s) {
    static SelectedGame next;
    next = Selected_None;

    TraceLog(LOG_ALL, "%d", s->selected);

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
    if (next != s->selected) {
        SetWindowTitle(selected_get_window_title(next));
        s->selected = next;
    }
}

#endif