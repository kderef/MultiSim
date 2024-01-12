#ifndef UNIVERSE_C_
#define UNIVERSE_C_

//! Implementation of the 'universe' of cells.
//! AKA a dynamically sized flat array that stores the state of all the cells.
//! NOTE sizeof(Cell) is omitted in pointer arithmetic because it is of type bool,
//! which has a size of 1 byte.

#include "raylib.h"
#include "cell.cpp"
#include "../gamestate.hpp"
#include "../const.hpp"

#include <stdlib.h>
#include <memory.h>

struct Universe {
private:
    Cell* cells;
    Cell* cells_copy;

public:
    size_t width;
    size_t height;
    size_t len;

    Universe() :
        cells(static_cast<Cell*>(calloc(GOL_GRID_W * GOL_GRID_H, sizeof(Cell)))),
        cells_copy(static_cast<Cell*>(calloc(GOL_GRID_W * GOL_GRID_H, sizeof(Cell)))),
        width(GOL_GRID_W),
        height(GOL_GRID_H),
        len(GOL_GRID_W * GOL_GRID_H)
    {}

    ~Universe() {
        free(cells);
        free(cells_copy);
    }

    void fill(Cell with) {
        memset(cells, (int)with, len);
    }

    Cell get(size_t x, size_t y) {
        return cells[min(y, height - 1) * width + min(x, width - 1)];
    }

    void set(size_t x, size_t y, Cell to) {
        cells[min(y, height - 1) * width + min(x, width - 1)] = to;
    }

    void invert() {
        for (size_t i = 0; i < len; i++) cells[i] = !(cells[i]);
    }

    void fill_random() {
        for (size_t i = 0; i < len; i++) {
            int n = GetRandomValue(0, INT_MAX);
            cells[i] = n % 3 == 0 || n % 7 == 0;
        }
    }

    void resize(size_t new_width, size_t new_height) {
        if (new_width <= width && new_height <= height) return;
        
        width = (new_width + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
        height = (new_height + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
        len = width * height;

        memcpy(cells_copy, cells, len);
        cells = (Cell*) realloc(cells, len);
        memset(cells, 0, len);
        memcpy(cells, cells_copy, len);

        for (size_t y = 0; y < min(height, new_height); y++) {
            for (size_t x = 0; x < min(width, new_width); x++) {
                const size_t idx = y * width + x;
                cells[idx] = cells_copy[idx];
            }
        }
    }

    void update_cells() {
        memcpy(cells_copy, cells, len);

        static u8 neighbours;
        static Cell cell_state;

        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                neighbours = 0;
                cell_state = this->get(x, y);

                for (i8 dx = -1; dx <= 1; dx++) {
                    for (i8 dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0) continue;
                        
                        size_t nx = x + dx;
                        size_t ny = y + dy;

                        if (
                            nx >= 0 &&
                            nx < width &&
                            ny >= 0 &&
                            ny < height &&
                            this->get(nx, ny)
                        ) neighbours += 1;
                    }
                }
                cells_copy[y * width + x] = cell_next_iteration(cell_state, neighbours);
            }
        }
        memcpy(cells, cells_copy, len);
    }
};

#endif