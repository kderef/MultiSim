#ifndef MINESWEEPER_C
#define MINESWEEPER_C

#include <stdlib.h>
#include "../const.hpp"
#include "../gamestate.hpp"

namespace NMinesweeper {
    enum Cell : u8 {
        Empty = 0,
        Flagged,
        Bomb
    };

    struct Minesweeper {
    private:
        Cell* cells;
        size_t width, height;

    public:
        Minesweeper() {
            width = WINDOW_W;
            height = WINDOW_H;
            cells = (Cell*) malloc(sizeof(Cell) * width * height);
        }

        ~Minesweeper() {
            free(cells);
        }

        SelectedGame update() {
            // TODO
        }

        void draw() {
            // TODO
        }
    };
}


#endif