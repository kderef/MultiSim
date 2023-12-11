use raylib::prelude::*;
use std::ops::Sub;

#[repr(u8)]
#[derive(Clone, Copy, Debug)]
pub enum Cell {
    Dead = 0,
    Alive = 1,
}

impl Cell {
    fn calculate_next_iteration(&self, neighbours: usize) -> Self {
        match self {
            Self::Alive => match neighbours {
                2..=3 => Self::Alive, // survival
                _ => Self::Dead,      // under- or overpopulation
            },
            Self::Dead => {
                if neighbours == 3 {
                    Self::Alive // reproduction
                } else {
                    Self::Dead // remain dead
                }
            }
        }
    }
}

const SCALE: usize = 15;

pub const WINDOW_W: usize = 900;
pub const WINDOW_H: usize = 750;

pub const GRID_W: usize = WINDOW_W / SCALE;
pub const GRID_H: usize = WINDOW_H / SCALE;

type Cells = [[Cell; GRID_W]; GRID_H];

const CELLS_EMPTY: Cells = [[Cell::Dead; GRID_W]; GRID_H];

/*****************************************************************/

#[derive(Clone, Debug)]
enum State {
    DesignMode,
    SimulationMode,
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
    fn calculate_neighbours(&self, x: usize, y: usize) -> usize {
        const NEIGHBOR_OFFSETS: [(i32, i32); 8] = [
            (-1, -1),
            (0, -1),
            (1, -1),
            (-1, 0),
            (1, 0),
            (-1, 1),
            (0, 1),
            (1, 1),
        ];

        let mut neighbours = 0;

        for &(dx, dy) in &NEIGHBOR_OFFSETS {
            let nx = x as i32 + dx;
            let ny = y as i32 + dy;

            if (nx >= 0 && nx < GRID_W as i32) && (ny >= 0 && ny < GRID_H as i32) {
                if let Cell::Alive = self.cells[ny as usize][nx as usize] {
                    neighbours += 1;
                }
            }
        }

        neighbours
    }
    fn update_cells(&mut self) {
        let mut next_generation = self.cells;

        for y in 0..GRID_H {
            for x in 0..GRID_W {
                let neighbours = self.calculate_neighbours(x, y);
                let new_state = self.cells[y][x].calculate_next_iteration(neighbours);
                next_generation[y][x] = new_state;
            }
        }

        self.cells = next_generation;
    }
    pub fn run(&mut self) {
        self.rl.set_exit_key(None);

        const UPDATE_TIME_STEP: f32 = 0.05;

        let mut update_frame_cap = 0.75;
        let mut passed_time: f32 = 0.0;

        while !self.rl.window_should_close() {
            // limit redrawing
            if passed_time >= update_frame_cap {
                passed_time = 0.0;
            }
            passed_time += self.rl.get_frame_time();

            // get mouse position inside grid
            let mut mouse_pos = self.rl.get_mouse_position().scale_by(1.0 / SCALE as f32);
            mouse_pos.x = mouse_pos.x.floor();
            mouse_pos.y = mouse_pos.y.floor();

            // update window title
            let title = format!(
                "Game of Life | mode: {:?} | redraw time: {:.2} | H - help menu",
                self.state, update_frame_cap
            );
            self.rl.set_window_title(&self.thread, &title);

            match self.state {
                State::SimulationMode => {
                    if passed_time >= update_frame_cap {
                        self.update_cells();
                        passed_time = 0.0;
                    }
                }
                State::DesignMode => {
                    if self.rl.is_mouse_button_down(MouseButton::MOUSE_LEFT_BUTTON) {
                        self.cells[mouse_pos.y as usize][mouse_pos.x as usize] = Cell::Alive;
                    } else if self
                        .rl
                        .is_mouse_button_down(MouseButton::MOUSE_RIGHT_BUTTON)
                    {
                        self.cells[mouse_pos.y as usize][mouse_pos.x as usize] = Cell::Dead;
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
                        }
                    }
                    KeyboardKey::KEY_C => {
                        self.cells = CELLS_EMPTY;
                    }
                    KeyboardKey::KEY_KP_ADD | KeyboardKey::KEY_EQUAL => {
                        update_frame_cap += UPDATE_TIME_STEP;
                    }
                    KeyboardKey::KEY_KP_SUBTRACT | KeyboardKey::KEY_MINUS => {
                        update_frame_cap = update_frame_cap.sub(UPDATE_TIME_STEP).clamp(0.0, f32::MAX);
                    }
                    KeyboardKey::KEY_SPACE => {
                        self.state = match self.state {
                            State::DesignMode => State::SimulationMode,
                            State::SimulationMode => State::DesignMode,
                            State::HelpMode => State::HelpMode,
                        };
                    }
                    KeyboardKey::KEY_ESCAPE => {
                        if let State::HelpMode = self.state {
                            self.state = State::DesignMode;
                        }
                    }

                    _ => {}
                }
            }

            let mut dr = self.rl.begin_drawing(&self.thread);
            dr.clear_background(Color::BLACK);

            // if help mode
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
                    "Left mouse button  - make cell alive",
                    0,
                    50,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    "Right mouse button - make cell dead",
                    0,
                    80,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    "Space                  - pause/unpause game",
                    0,
                    110,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    "H                        - help menu",
                    0,
                    140,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    "C                        - clear the board",
                    0,
                    170,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    &format!("+                        - add {UPDATE_TIME_STEP:.2}s to update time"),
                    0,
                    200,
                    FONT_M,
                    Color::WHITE,
                );
                dr.draw_text(
                    &format!(
                        "-                        - subtract {UPDATE_TIME_STEP:.2}s from update time",
                    ),
                    0,
                    230,
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

            // draw mouse hover
            if let State::DesignMode = self.state {
                let dim = SCALE as i32;
                dr.draw_rectangle_lines(
                    mouse_pos.x as i32 * dim,
                    mouse_pos.y as i32 * dim,
                    dim,
                    dim,
                    Color::GREEN,
                );
            }
        }
    }
}
