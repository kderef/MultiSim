#ifndef CELL_H_
#define CELL_H_

//! Definition of the type Cell, which makes up the grid.
//! This is defined as a typedef instead of enum for size optimization, bool = 1 byte.

typedef _Bool GolCell;
#define Alive 1
#define Dead 0

static inline GolCell cell_next_iteration(GolCell old, int neighbours) {
    return old?
        neighbours >= 2 && neighbours <= 3 :
        neighbours == 3;
}

#endif