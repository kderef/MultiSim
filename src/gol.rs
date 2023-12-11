use std::fmt;

use raylib::prelude::*;

use crate::rl_port;

// basic types
#[derive(Clone, Copy)]
pub enum Cell {
    Alive, Dead
}

pub const WINDOW_W: usize = 800;
pub const WINDOW_H: usize = 600;

pub const GRID_W: usize = 0;
pub const GRID_H: usize = 0;

type Cells = [[Cell; WINDOW_W]; WINDOW_H];

const CELLS_EMPTY: Cells = [[Cell::Dead; WINDOW_W]; WINDOW_H];

/// game struct
pub struct GameOfLife {
    cells: Cells,
    rl: RaylibHandle,
    thread: RaylibThread
}

impl GameOfLife {
    pub fn new(rl: RaylibHandle, th: RaylibThread) -> Self {
        Self {
            cells: CELLS_EMPTY,
            rl, thread: th
        }
    }
    pub fn prepare(&mut self) {
        let monitor = rl_port::get_current_monitor();
        let refresh_rate = rl_port::get_monitor_refresh_rate(monitor);

        self.rl.set_target_fps(refresh_rate as u32);
        self.rl.set_exit_key(None);
    }
    pub fn run(&mut self) {
        while !self.rl.window_should_close() {
            let mut dr = self.rl.begin_drawing(&self.thread);

            dr.clear_background(Color::BLACK);

            for row in dr.get_window
        }
    }
}