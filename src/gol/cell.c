#ifndef CELL_H_
#define CELL_H_

#include <stdint.h>

typedef _Bool Cell;
#define Dead 0
#define Alive 1

void cell_next_iteration(Cell* p_cell, uint8_t neighbours) {
    *p_cell = (*p_cell)?
        neighbours >= 2 && neighbours <= 3 :
        neighbours == 3;
}

#endif