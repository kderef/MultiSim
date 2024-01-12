#ifndef SELECTEDGAME_H_
#define SELECTEDGAME_H_

#include "raylib.h"
#include "const.h"
#include <stdlib.h>

typedef enum SelectedGame {
    Selected_None = 0,
    Selected_GOL,
    Selected_DVD,
    Selected_PONG,
    Selected_MINESWEEPER
} SelectedGame;

typedef enum GameState {
    GameState_Running = 0,
    GameState_Paused,
    GameState_Help,
} GameState;

// global state

#define GLOBAL_TEXT_BUF_SIZE (256 + 1)

static bool global_show_fps = false;
static int global_screen_width = 0;
static int global_screen_height = 0;
static char global_text_buf[GLOBAL_TEXT_BUF_SIZE];

// global snprintf to global_text_buf
#define g_sprintf(FMT, ...) snprintf(global_text_buf, GLOBAL_TEXT_BUF_SIZE, FMT, ##__VA_ARGS__)

static const char* SELECTED_TITLES[] = {
    [Selected_None] = "MultiSim - menu",
    [Selected_GOL] = "MultiSim - Game of Life",
    [Selected_DVD] = "MultiSim - DvD bouncy",
    [Selected_PONG] = "MultiSim - Pong",
    [Selected_MINESWEEPER] = "MultiSim - Minesweeper"
};

#define selected_get_window_title(S) (SELECTED_TITLES[S])

#endif