#ifndef CELL_H_
#define CELL_H_

//! Definition of the type Cell, which makes up the grid.
//! This is defined as a typedef instead of enum for size optimization, bool = 1 byte.

typedef bool Cell;
#define Alive true
#define Dead false

static inline Cell cell_next_iteration(Cell old, int neighbours) {
    return old?
        neighbours >= 2 && neighbours <= 3 :
        neighbours == 3;
}

#endif