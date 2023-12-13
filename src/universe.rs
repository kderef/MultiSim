use crate::cell::{Cells, Cell};

#[derive(Clone)]
pub struct Universe {
    cells: Cells,
    pub width: usize,
    pub height: usize,
    max_width: usize,
    max_height: usize,
}

impl Universe {
    pub fn new(width: usize, height: usize) -> Self {
        Self {
            cells: std::iter::repeat(Cell::Dead)
                .take((width * height) as usize)
                .collect(),
            width,
            height,
            max_width: width,
            max_height: height,
        }
    }
    pub fn clear(&mut self) {
        self.cells.fill(Cell::Dead);
    }
    pub fn get(&self, x: usize, y: usize) -> Cell {
        self.cells[y * self.width + x]
    }
    pub fn set(&mut self, x: usize, y: usize, val: Cell) {
        self.cells[y * self.width + x] = val;
    }
    pub fn resize(&mut self, new_width: usize, new_height: usize) {
        // Check if the new dimensions exceed the maximum allowed width and height
        if new_width <= self.max_width && new_height <= self.max_height {
            // Only update the universe dimensions
            self.width = new_width;
            self.height = new_height;
        } else {
            let mut new_cells = vec![Cell::Dead; new_width * new_height];

            // Copy existing cells to the new vector within the bounds of max_width and max_height
            for y in 0..new_height.min(self.max_height) {
                let source_start = y * self.width.min(self.max_width);
                let source_end = source_start + self.width.min(self.max_width);
                let dest_start = y * new_width;
                let dest_end = dest_start + self.width.min(self.max_width);
                new_cells[dest_start..dest_end].copy_from_slice(&self.cells[source_start..source_end]);
            }

            self.cells = new_cells;
            self.width = new_width.min(self.max_width);
            self.height = new_height.min(self.max_height);
        }
    }
}