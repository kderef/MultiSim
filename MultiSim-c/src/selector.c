#ifndef SELECTOR_C_
#define SELECTOR_C_

#include "const.h"
#include "gol/game.c"
#include "selectedgame.c"
#include "dvd/dvd.c"
#include <time.h>

typedef struct {
    SelectedGame selected;
    GameOfLife gol;
    Dvd dvd;
} Selector;

void selector_init(Selector* to_init) {
    to_init->gol = game_of_life_new();
    to_init->dvd = dvd_new();
    to_init->selected = Selected_GOL;

    // seed rand
    SetRandomSeed(time(NULL));
}

void selector_deinit(Selector* ptr) {
    game_of_life_deinit(&(ptr->gol));
    dvd_deinit(&(ptr->dvd));
}

void selector_title_screen(Selector* s) {
    BeginDrawing();
    ClearBackground(BLACK);


    EndDrawing();
}

void selector_update(Selector* s) {
    static SelectedGame next;
    next = Selected_None;

    switch (s->selected) {
        case Selected_None: {
            selector_title_screen(s);
        } break;
        case Selected_GOL: {
            next = game_of_life_update(&(s->gol));
        } break;
        case Selected_DVD: {
            next = dvd_update(&(s->dvd));
        } break;
    }
    if (next != s->selected) {
        SetWindowTitle(selected_get_window_title(next));
        s->selected = next;
    }
}

#endif