#ifndef UNIVERSE_C_
#define UNIVERSE_C_

//! Implementation of the 'universe' of cells.
//! AKA a dynamically sized flat array that stores the state of all the cells.
//! NOTE sizeof(Cell) is omitted in pointer arithmetic because it is of type bool,
//! which has a size of 1 byte.

#include "raylib.h"
#include "cell.c"
#include "../gamestate.h"
#include "../const.h"

#include <stdlib.h>
#include <memory.h>

#define Cell GolCell

typedef struct Universe {
    Cell* cells;
    Cell* cells_copy;
    size_t width, height, size;
} Universe;

Universe universe_new(size_t init_width, size_t init_height) {
    Universe uvs;
    uvs.width = init_width;
    uvs.height = init_height;
    uvs.size = uvs.width * uvs.height;
    uvs.cells = (Cell*) calloc(uvs.size, sizeof(Cell));
    uvs.cells_copy = (Cell*) calloc(uvs.size, sizeof(Cell));

    return uvs;
}

void universe_deinit(Universe* uvs) {
    free(uvs->cells);
    free(uvs->cells_copy);
}

void universe_set(Universe* uvs, size_t x, size_t y, Cell to) {
    uvs->cells[min(y, uvs->height - 1) * uvs->width + min(x, uvs->width - 1)] = to;
}

Cell universe_get(Universe* uvs, size_t x, size_t y) {
    return uvs->cells[y * uvs->width + x];
}

void universe_fill(Universe* uvs, Cell with) {
    memset(uvs->cells, with, uvs->size);
}

void universe_invert(Universe* uvs) {
    for (size_t i = 0; i < uvs->size; i++) uvs->cells[i] = !(uvs->cells[i]);
}

void universe_fill_random(Universe* uvs) {
    for (size_t i = 0; i < uvs->size; i++) {
        int n = GetRandomValue(0, INT_MAX);
        uvs->cells[i] = n % 3 == 0 || n % 7 == 0;
    }
}

void universe_resize(Universe* uvs, size_t new_width, size_t new_height) {
    if (new_width <= uvs->width && new_height <= uvs->height) return;
    
    uvs->width = (new_width + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
    uvs->height = (new_height + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
    uvs->size = new_width * new_height;

    memcpy(uvs->cells_copy, uvs->cells, uvs->size);
    uvs->cells = (Cell*) realloc(uvs->cells, uvs->size);
    memset(uvs->cells, 0, uvs->size);
    memcpy(uvs->cells, uvs->cells_copy, uvs->size);

    for (size_t y = 0; y < new_height; y++) {
        for (size_t x = 0; x < new_width; x++) {
            const size_t idx = y * uvs->width + x;
            uvs->cells[idx] = uvs->cells_copy[idx];
        }
    }
    uvs->cells_copy = (Cell*) realloc(uvs->cells_copy, uvs->size);
}

void universe_update_cells(Universe* uvs) {
    memcpy(uvs->cells_copy, uvs->cells, uvs->size);

    static u8 neighbours;
    static Cell cell_state;

    for (size_t y = 0; y < uvs->height; y++) {
        for (size_t x = 0; x < uvs->width; x++) {
            neighbours = 0;
            cell_state = universe_get(uvs, x, y);

            for (i8 dx = -1; dx <= 1; dx++) {
                for (i8 dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    
                    size_t nx = x + dx;
                    size_t ny = y + dy;

                    if (
                        nx >= 0 &&
                        nx < uvs->width &&
                        ny >= 0 &&
                        ny < uvs->height &&
                        universe_get(uvs, nx, ny)
                    ) neighbours += 1;
                }
            }
            uvs->cells_copy[y * uvs->width + x] = cell_next_iteration(cell_state, neighbours);
        }
    }
    memcpy(uvs->cells, uvs->cells_copy, uvs->size);
}

#undef Cell
#endif