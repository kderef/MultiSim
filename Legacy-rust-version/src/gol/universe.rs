use crate::{
    gol_cell::{Cell, Cells},
    gol_consts::SCALE,
};
use std::iter;

#[derive(Clone)]
pub struct Universe {
    cells: Cells,
    pub width: usize,
    pub height: usize,
}

impl Universe {
    pub fn new(width: usize, height: usize) -> Self {
        Self {
            cells: std::iter::repeat(Cell::Dead)
                .take((width * height) as usize)
                .collect(),
            width,
            height,
        }
    }
    pub fn fill(&mut self, val: Cell) {
        self.cells.fill(val);
    }
    fn index(&self, x: usize, y: usize) -> usize {
        y.clamp(0, self.height as usize) * self.width + x.clamp(0, self.width as usize)
    }
    pub fn get(&self, x: usize, y: usize) -> Cell {
        self.cells[self.index(x, y)]
    }
    pub fn set(&mut self, x: usize, y: usize, val: Cell) {
        let i = self.index(x, y);
        self.cells[i] = val;
    }
    pub fn invert(&mut self) {
        self.cells = self.cells.iter().map(Cell::flip).collect();
    }
    pub fn fill_random(&mut self) {
        self.cells.fill(Cell::Dead);
        self.cells = iter::repeat_with(|| {
            let n = macroquad::rand::rand();
            (n % 2 == 0 || n % 7 == 0).into()
        })
        .take(self.width * self.height)
        .collect();
    }
    pub fn resize(&mut self, new_width: usize, new_height: usize) {
        if new_width <= self.width && new_height <= self.height {
            return;
        }

        // Adjust width and height to be multiples of SCALE
        let new_width = (new_width + SCALE - 1) / SCALE * SCALE;
        let new_height = (new_height + SCALE - 1) / SCALE * SCALE;

        // Limit the grid size to the maximum defined values
        let mut new_cells = vec![Cell::Dead; new_width * new_height];

        // Copy existing cells to new_cells, adjusting for the size difference
        for y in 0..self.height.min(new_height) {
            for x in 0..self.width.min(new_width) {
                new_cells[y * new_width + x] = self.get(x, y);
            }
        }

        self.width = new_width;
        self.height = new_height;

        self.cells = new_cells;
    }
}
