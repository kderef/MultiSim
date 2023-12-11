use std::ops::Range;

use raylib::prelude::*;

#[derive(Clone, Copy)]
pub enum Cell {
    Alive,
    Dead,
}

const SCALE: usize = 15;

pub const WINDOW_W: usize = 800;
pub const WINDOW_H: usize = 600;

pub const GRID_W: usize = WINDOW_W / SCALE;
pub const GRID_H: usize = WINDOW_H / SCALE;

pub const TITLE_DESIGN_MODE: &str = "Game of Life [DESIGN MODE] - h for help";
pub const TITLE_SIM_MODE: &str = "Game of Life [SIMULATION MODE] - h for help";
pub const TITLE_HELP_MODE: &str = "Game of Life [HELP MODE] - ESC to return";

type Cells = [[Cell; GRID_W]; GRID_H];

const CELLS_EMPTY: Cells = [[Cell::Dead; GRID_W]; GRID_H];

/*****************************************************************/

#[derive(Clone)]
enum State {
    DesignMode,
    SimMode,
    HelpMode,
}

/// game struct
pub struct GameOfLife {
    cells: Cells,
    rl: RaylibHandle,
    thread: RaylibThread,
    state: State,
}

impl GameOfLife {
    pub fn new(rl: RaylibHandle, th: RaylibThread) -> Self {
        Self {
            cells: CELLS_EMPTY,
            rl,
            thread: th,
            state: State::DesignMode,
        }
    }
    pub fn prepare(&mut self) {
        self.rl.set_target_fps(30);
        self.rl.set_exit_key(None);
    }
    fn get_cell(&self, x: i32, y: i32) -> Cell {
        const ALLOWED_X: Range<i32> = 0..(GRID_W as i32);
        const ALLOWED_Y: Range<i32> = 0..(GRID_H as i32);
    
        if !(ALLOWED_X.contains(&x) && ALLOWED_Y.contains(&y)) {
            Cell::Dead
        } else {
            self.cells[y as usize][x as usize]
        }
    }
    fn calculate_neighbours(&self, x: usize, y: usize) -> usize {
        let x = x as i32;
        let y = y as i32;
        let neighbours = [
            [
                self.get_cell(x - 1, y - 1),
                self.get_cell(x, y - 1),
                self.get_cell(x + 1, y - 1),
            ],
            [
                self.get_cell(x - 1, y),
                self.get_cell(x, y),
                self.get_cell(x + 1, y),
            ],
            [
                self.get_cell(x - 1, y + 1),
                self.get_cell(x, y + 1),
                self.get_cell(x + 1, y + 1),
            ],
        ];
    
        neighbours
            .iter()
            .flatten()
            .filter(|b| matches!(b, Cell::Alive))
            .count()
    }
    fn update_cells(&mut self) {
        /*
            1 Any live cell with fewer than two live neighbours dies, as if by underpopulation.
            2 Any live cell with two or three live neighbours lives on to the next generation.
            3 Any live cell with more than three live neighbours dies, as if by overpopulation.
            4 Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
        */
        for x in 0..GRID_W {
            for y in 0..GRID_H {
                let neighbours = self.calculate_neighbours(x, y);
                let old_state = self.cells[y][x];

                let new_state = match old_state {
                    Cell::Alive => match neighbours {
                        0..=1 => Cell::Dead,
                        2..=3 => Cell::Alive,
                        _ => Cell::Dead,
                    },
                    Cell::Dead => {
                        if neighbours == 3 {
                            Cell::Alive
                        } else {
                            Cell::Dead
                        }
                    }
                };

                self.cells[y][x] = new_state;
            }
        }
    }
    pub fn run(&mut self) {
        while !self.rl.window_should_close() {
            match self.state {
                State::SimMode => self.update_cells(),
                State::DesignMode => {
                    if self
                        .rl
                        .is_mouse_button_pressed(MouseButton::MOUSE_LEFT_BUTTON)
                    {
                        let mut mouse_pos =
                            self.rl.get_mouse_position().scale_by(1.0 / SCALE as f32);
                        mouse_pos.x = mouse_pos.x.floor();
                        mouse_pos.y = mouse_pos.y.floor();

                        self.cells[mouse_pos.y as usize][mouse_pos.x as usize] = Cell::Alive;
                    }
                }
                _ => {}
            }

            if let Some(key) = self.rl.get_key_pressed() {
                match key {
                    KeyboardKey::KEY_H => {
                        if let State::HelpMode = self.state {
                            self.state = State::DesignMode;
                        } else {
                            self.state = State::HelpMode;
                            self.rl.set_window_title(&self.thread, TITLE_HELP_MODE);
                        }
                    },
                    KeyboardKey::KEY_C => {
                        self.cells = CELLS_EMPTY;
                    },
                    KeyboardKey::KEY_SPACE => {
                        let title;
                        (self.state, title) = match self.state {
                            State::DesignMode => (State::SimMode, TITLE_DESIGN_MODE),
                            State::SimMode => (State::DesignMode, TITLE_SIM_MODE),
                            State::HelpMode => (State::HelpMode, TITLE_HELP_MODE),
                        };
                        self.rl.set_window_title(&self.thread, title);
                    }
                    KeyboardKey::KEY_ESCAPE => {
                        if let State::HelpMode = self.state {
                            self.state = State::DesignMode;
                            self.rl.set_window_title(&self.thread, TITLE_DESIGN_MODE);
                        }
                    }

                    _ => {}
                }
            }

            let mut dr = self.rl.begin_drawing(&self.thread);
            dr.clear_background(Color::BLACK);

            if let State::HelpMode = self.state {
                const FONT_S: i32 = 19;
                const FONT_M: i32 = 30;
                const FONT_L: i32 = 35;
                const FONT_XL: i32 = 40;

                dr.draw_text("CONTROLS:", 0, 0, FONT_XL, Color::WHITE);
                dr.draw_line_ex(
                    Vector2::new(0.0, 45.0),
                    Vector2::new(WINDOW_W as f32, 45.0),
                    5.0,
                    Color::WHITE,
                );
                dr.draw_text(
                    "Left mouse button - make cell alive",
                    0,
                    50,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    "Space                 - pause/unpause game",
                    0,
                    80,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    "H                       - help menu",
                    0,
                    110,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    "C                       - clear the board",
                    0,
                    140,
                    FONT_M,
                    Color::WHITE,
                );

                dr.draw_text(
                    "Press ESC or H to return",
                    0,
                    WINDOW_H as i32 - FONT_L,
                    FONT_L,
                    Color::WHITE,
                );
                dr.draw_text(
                    "Made by Kian (Kvoid)",
                    WINDOW_W as i32 - 190,
                    WINDOW_H as i32 - FONT_S,
                    FONT_S,
                    Color::GRAY,
                );
                continue;
            }

            // draw the cells
            for y in 0..GRID_H {
                for x in 0..GRID_W {
                    dr.draw_rectangle(
                        (x * SCALE) as i32,
                        (y * SCALE) as i32,
                        SCALE as i32,
                        SCALE as i32,
                        if let Cell::Alive = self.cells[y][x] {
                            Color::WHITE
                        } else {
                            Color::BLACK
                        },
                    );
                }
            }
        }
    }
}
