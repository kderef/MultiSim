#ifndef UNIVERSE_C_
#define UNIVERSE_C_

//! Implementation of the 'universe' of cells.
//! AKA a dynamically sized flat array that stores the state of all the cells.
//! NOTE sizeof(Cell) is omitted in pointer arithmetic because it is of type _Bool,
//! which has a size of 1 byte.

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

    Cell* next_generation;
    size_t next_generation_size;
} Universe;

Universe universe_new(size_t width, size_t height) {
    Universe u;

    u.len = width * height;
    u.cells = (Cell*) calloc(u.len, sizeof(Cell));
    u.width = width;
    u.height = height;

    u.next_generation_size = u.len;
    u.next_generation = (Cell*) malloc(u.len * sizeof(Cell));

    return u;
}
static inline void universe_fill(Universe* p_uvs, Cell with) {
    memset(p_uvs->cells, with, (p_uvs->len) * sizeof(Cell));
}

static inline Cell universe_get(Universe* p_uvs, size_t x, size_t y) {
    return p_uvs->cells[min(y, p_uvs->height - 1) * (p_uvs->width) + min(x, p_uvs->width - 1)];
}

static inline void universe_set(Universe* p_uvs, size_t x, size_t y, Cell to) {
    p_uvs->cells[min(y, p_uvs->height - 1) * (p_uvs->width) + min(x, p_uvs->width - 1)] = to;
}

static inline void universe_invert(Universe* p_uvs) {
    for (size_t i = 0; i < p_uvs->len; i++) {
        p_uvs->cells[i] = !(p_uvs->cells[i]);
    }
}

static inline void universe_fill_random(Universe* p_uvs) {
    for (size_t i = 0; i < p_uvs->len; i++) {
        int n = GetRandomValue(0, INT_MAX);
        p_uvs->cells[i] = n % 3 == 0 || n % 7 == 0;
    }
}

void universe_resize(Universe* p_uvs, size_t new_width, size_t new_height) {
    static size_t new_w, new_h, new_len;
    static Cell* new_cells;

    if (new_width <= p_uvs->width && new_height <= p_uvs->height) return;

    new_w = (new_width + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
    new_h = (new_height + GOL_SCALE -1) / GOL_SCALE * GOL_SCALE;
    new_len = new_w * new_h;

    new_cells = (Cell*) calloc(new_len, sizeof(Cell));

    for (size_t y = 0; y < min(p_uvs->height, new_height); y++) {
        for (size_t x = 0; x < min(p_uvs->width, new_width); x++) {
            new_cells[y * new_w + x] = universe_get(p_uvs, x, y);
        }
    }

    p_uvs->width = new_w;
    p_uvs->height = new_h;
    p_uvs->len = new_len;

    p_uvs->cells = realloc(p_uvs->cells, new_len);
    memcpy(p_uvs->cells, new_cells, new_len);
}

void universe_deinit(Universe* p_uvs) {
    free(p_uvs->cells);
    free(p_uvs->next_generation);
}

void universe_update_cells(Universe* p_uvs) {
    if (p_uvs->len > p_uvs->next_generation_size) {
        p_uvs->next_generation = realloc(p_uvs->next_generation, p_uvs->len);
        p_uvs->next_generation_size = p_uvs->len;
    }

    memcpy(p_uvs->next_generation, p_uvs->cells, p_uvs->len);

    /* used each time the function is called */
    static u8 neighbours;
    static Cell cell_state;

    for (size_t y = 0; y < p_uvs->height; y++) {
        for (size_t x = 0; x < p_uvs->width; x++) {
            cell_state = universe_get(p_uvs, x, y);
            neighbours = 0;

            for (i8 dx = -1; dx <= 1; dx++) {
                for (i8 dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    
                    size_t nx = x + dx;
                    size_t ny = y + dy;

                    if (
                        nx >= 0 &&
                        nx < p_uvs->width &&
                        ny >= 0 &&
                        ny < p_uvs->height &&
                        universe_get(p_uvs, nx, ny)
                    ) neighbours += 1;
                }
            }

            p_uvs->next_generation[y * (p_uvs->width) + x] = cell_next_iteration(cell_state, neighbours);
        }
    }
    memcpy(p_uvs->cells, p_uvs->next_generation, p_uvs->next_generation_size);
}

#endif