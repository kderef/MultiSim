#ifndef UNIVERSE_C_
#define UNIVERSE_C_

//! Implementation of the 'universe' of cells.
//! AKA a dynamically sized flat array that stores the state of all the cells.
//! NOTE sizeof(Cell) is omitted in pointer arithmetic because it is of type _Bool,
//! which has a size of 1 byte.

#include "raylib.h"
#include <iostream>
#include <vector>
#include "cell.cpp"
#include "../gamestate.hpp"
#include "../const.hpp"

using std::min;
using std::max;
using std::vector;

struct Universe {
private:
    vector<Cell> cells;
    vector<Cell> cells_copy;

public:
    size_t width;
    size_t height;

    Universe() {
        width = GOL_GRID_W;
        height = GOL_GRID_H;
        cells = vector<Cell>(width * height, Dead);
        cells_copy = vector<Cell>(width * height, Dead);
    }
    Universe(size_t init_width, size_t init_height) {
        width = init_width;
        height = init_height;

        cells = vector<Cell>(width * height, Dead);
        cells_copy = vector<Cell>(width * height, Dead);
    }

    void fill(Cell with) {
        std::fill(cells.begin(), cells.end(), with);
    }

    Cell get(size_t x, size_t y) {
        return cells[min(y, height - 1) * width + min(x, width - 1)];
    }

    void set(size_t x, size_t y, Cell to) {
        cells[min(y, height - 1) * width + min(x, width - 1)] = to;
    }

    void invert() {
        cells.flip();
    }

    void fill_random() {
        for (size_t i = 0; i < cells.size(); i++) {
            int n = GetRandomValue(0, INT_MAX);
            cells[i] = n % 3 == 0 || n % 7 == 0;
        }
    }

    void resize(size_t new_width, size_t new_height) {
        if (new_width <= width && new_height <= height) return;
        
        width = (new_width + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;
        height = (new_height + GOL_SCALE - 1) / GOL_SCALE * GOL_SCALE;

        cells_copy = vector<Cell>(cells);
        cells.resize(width * height, Dead);

        for (size_t y = 0; y < min(height, new_height); y++) {
            for (size_t x = 0; x < min(width, new_width); x++) {
                const size_t idx = y * width + x;
                cells[idx] = cells_copy[idx];
            }
        }
    }

    void update_cells() {
        cells_copy = vector<Cell>(cells);

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

    }
};

#endif