#ifndef MINESWEEPER_C
#define MINESWEEPER_C

#include <stdlib.h>
#include "../const.h"
#include "../gamestate.h"
#include "raylib.h"

static const Color MS_BACKGROUND_COLOR = color_hex(0x1d2021);
static const Color MS_FOREGROUND_COLOR = color_hex(0xebdbb2);

typedef enum {
    Empty = 0,
    Flagged,
    Bomb
} MinesweeperCell;

typedef struct {
    MinesweeperCell* cells;
    size_t width;
    size_t height;
    size_t len;
} Minesweeper;

Minesweeper minesweeper_new() {
    Minesweeper ms;
    ms.width = WINDOW_W;
    ms.height = WINDOW_H;
    ms.len = ms.width * ms.height;
    ms.cells = calloc(ms.len, sizeof(MinesweeperCell));

    return ms;
}

void minesweeper_deinit(Minesweeper* ms) {
    free(ms->cells);
}

static inline void minesweeper_draw(Minesweeper* ms) {
    (void)ms; // FIXME

    BeginDrawing();
    ClearBackground(MS_BACKGROUND_COLOR);
}

SelectedGame minesweeper_update(Minesweeper* ms) {
    if (IsKeyPressed(KEY_ESCAPE)) return Selected_None;

    (void)ms; // FIXME


    minesweeper_draw(ms);
    return Selected_MINESWEEPER;
}

#endif
