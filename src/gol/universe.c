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
    uvs.width  = init_width;
    uvs.height = init_height;
    uvs.size   = uvs.width * uvs.height;
    uvs.cells      = (Cell*) calloc(uvs.size, sizeof(Cell));
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
    if (new_width <= uvs->width && new_height <= uvs->height) 
        return;

    size_t to_width  = (new_width  + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
    size_t to_height = (new_height + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
    size_t to_size = to_width * to_height;

    free(uvs->cells_copy);
    uvs->cells_copy = (Cell*) calloc(to_size, sizeof(Cell));
    memcpy(uvs->cells_copy, uvs->cells, uvs->size);

    free(uvs->cells);
    uvs->cells = (Cell*) calloc(to_size, sizeof(Cell));

    for (size_t y = 0; y < min(to_height, new_height); y++) {
        for (size_t x = 0; x < min(to_width, new_width); x++) {
            uvs->cells[y * uvs->width + x] = uvs->cells_copy[y * to_width + x];
        }
    }
    uvs->width = to_width;
    uvs->height = to_height;
    uvs->size = to_size;
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
                    
                    int nx = x + dx;
                    int ny = y + dy;

                    if (
                        nx >= 0 &&
                        nx < (int)uvs->width &&
                        ny >= 0 &&
                        ny < (int)uvs->height &&
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