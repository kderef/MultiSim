#ifndef UNIVERSE_C_
#define UNIVERSE_C_

#include "raylib.h"
#include <stdlib.h>
#include <memory.h>
#include "cell.c"
#include "../gamestate.c"
#include "../const.h"

typedef struct {
    Cell* cells;
    size_t width;
    size_t height;
    size_t len;
} Universe;

Universe universe_new(size_t width, size_t height) {
    Universe u;

    u.len = width * height;
    u.cells = (Cell*) malloc(sizeof(Cell) * u.len);
    u.width = width;
    u.height = height;

    memset(u.cells, Dead, sizeof(Cell) * u.len);

    for (int i = 0; i < u.len; i++) {
        TraceLog(LOG_ALL, "%d", u.cells[i]);
    }

    return u;
}
void universe_fill(Universe* p_uvs, Cell with) {
    memset(p_uvs->cells, with, (p_uvs->len) * sizeof(Cell));
}
Cell universe_get(Universe* p_uvs, size_t x, size_t y) {
    return p_uvs->cells[min(y, p_uvs->height - 1) * (p_uvs->width) + min(x, p_uvs->width - 1)];
}
void universe_set(Universe* p_uvs, size_t x, size_t y, Cell to) {
    p_uvs->cells[min(y, p_uvs->height - 1) * (p_uvs->width) + min(x, p_uvs->width - 1)] = to;
}
void universe_invert(Universe* p_uvs) {
    for (size_t i = 0; i < p_uvs->len; i++) {
        p_uvs->cells[i] = !(p_uvs->cells[i]);
    }
}
void universe_fill_random(Universe* p_uvs) {
    for (size_t i = 0; i < p_uvs->len; i++) {
        int n = GetRandomValue(0, INT_MAX);
        p_uvs->cells[i] = n % 3 == 0 || n % 7 == 0;
    }
}
void universe_resize(Universe* p_uvs, size_t new_width, size_t new_height) {
    if (new_width <= p_uvs->width && new_height <= p_uvs->height) return;

    const size_t new_w = (new_width + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
    const size_t new_h = (new_height + GOL_SCALE -1) / GOL_SCALE * GOL_SCALE;
    const size_t new_len = new_w * new_h;

    Cell* new_cells = (Cell*) malloc(sizeof(Cell) * new_len);
    memset(new_cells, Dead, sizeof(Cell) * new_len);

    for (size_t y = 0; y < min(p_uvs->height, new_height); y++) {
        for (size_t x = 0; x < min(p_uvs->width, new_width); x++) {
            new_cells[y * new_w + x] = universe_get(p_uvs, x, y);
        }
    }

    p_uvs->width = new_w;
    p_uvs->height = new_h;
    p_uvs->len = new_len;

    free(p_uvs->cells);
    p_uvs->cells = new_cells;
}

void universe_deinit(Universe* p_uvs) {
    free(p_uvs->cells);
    p_uvs->width = 0;
    p_uvs->height = 0;
    p_uvs->len = 0;
}

void universe_update_cells(Universe* p_uvs) {
    Cell* next_generation = (Cell*) malloc(sizeof(Cell) * p_uvs->len);
    memcpy(next_generation, p_uvs->cells, sizeof(Cell) * p_uvs->len);

    /* used each time the function is called */
    static uint8_t neighbours = 0;
    static Cell cell_state = Dead;

    for (size_t y = 0; y < p_uvs->height; y++) {
        for (size_t x = 0; x < p_uvs->width; x++) {
            cell_state = universe_get(p_uvs, x, y);
            neighbours = 0;

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    
                    int nx = (int)(x) + dx;
                    int ny = (int)(y) + dy;

                    if (
                        nx >= 0 &&
                        nx < (int)p_uvs->width &&
                        ny >= 0 &&
                        ny < (int)p_uvs->height &&
                        universe_get(p_uvs, (size_t)nx, (size_t)ny)
                    ) neighbours += 1;
                }
            }

            cell_next_iteration(&cell_state, neighbours);
            next_generation[y * (p_uvs->width) + x] = cell_state;
        }
    }
    free(p_uvs->cells);
    p_uvs->cells = next_generation;
}

#endif