#ifndef SELECTEDGAME_H_
#define SELECTEDGAME_H_

#include "raylib.h"

enum SelectedGame {
    Selected_None = 0,
    Selected_GOL,
    Selected_DVD,
    Selected_PONG,
};

enum GameState {
    GameState_Running = 0,
    GameState_Paused,
    GameState_Help,
};

// global state

static bool global_show_fps = false;
static int global_screen_width = 0;
static int global_screen_height = 0;

static const char* SELECTED_TITLES[] = {
    "MultiSim - menu",
    "MultiSim - Game of Life",
    "MultiSim - DvD bouncy",
    "MultiSim - Pong"
};

#define selected_get_window_title(S) (SELECTED_TITLES[S])

#endif