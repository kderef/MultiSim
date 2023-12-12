use macroquad::prelude::*;
use std::ops::Div;

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

pub const UPDATE_TIME_STEP: f32 = 0.05;

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
    state: State,
}

impl GameOfLife {
    pub fn new() -> Self {
        Self {
            cells: CELLS_EMPTY,
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
    pub fn run(&mut self, passed_time: &mut f32, update_frame_cap: &mut f32) {
        // get mouse position inside grid
        let mut mouse_pos = mouse_position();
        mouse_pos.0 = mouse_pos
            .0
            .div(SCALE as f32)
            .floor()
            .clamp(0.0, GRID_W as f32 - 1.0);
        mouse_pos.1 = mouse_pos
            .1
            .div(SCALE as f32)
            .floor()
            .clamp(0.0, GRID_H as f32 - 1.0);

        match self.state {
            State::SimulationMode => {
                if passed_time >= update_frame_cap {
                    self.update_cells();
                    *passed_time = 0.0;
                }
            }
            State::DesignMode => {
                if is_mouse_button_down(MouseButton::Left) {
                    self.cells[mouse_pos.1 as usize][mouse_pos.0 as usize] = Cell::Alive;
                } else if is_mouse_button_down(MouseButton::Right) {
                    self.cells[mouse_pos.1 as usize][mouse_pos.0 as usize] = Cell::Dead;
                }
            }
            _ => {}
        }

        if let Some(key) = get_last_key_pressed() {
            match key {
                KeyCode::H => {
                    if let State::HelpMode = self.state {
                        self.state = State::DesignMode;
                    } else {
                        self.state = State::HelpMode;
                    }
                }
                KeyCode::C => {
                    self.cells = CELLS_EMPTY;
                }
                KeyCode::Equal | KeyCode::KpEqual => {
                    *update_frame_cap += UPDATE_TIME_STEP;
                }
                KeyCode::KpSubtract | KeyCode::Minus => {
                    *update_frame_cap = (*update_frame_cap - UPDATE_TIME_STEP).clamp(0.0, f32::MAX);
                }
                KeyCode::Space => {
                    self.state = match self.state {
                        State::DesignMode => State::SimulationMode,
                        State::SimulationMode => State::DesignMode,
                        State::HelpMode => State::HelpMode,
                    };
                }
                KeyCode::Escape => {
                    if let State::HelpMode = self.state {
                        self.state = State::DesignMode;
                    }
                }

                _ => {}
            }
        }

        clear_background(BLACK);

        // if help mode
        if let State::HelpMode = self.state {
            const FONT_S: f32 = 19.0;
            const FONT_M: f32 = 30.0;
            const FONT_L: f32 = 35.0;
            const FONT_XL: f32 = 60.0;

            draw_text("CONTROLS", 0.0, 34.0, FONT_XL, GREEN);

            draw_text(
                "Left mouse button  - make cell alive",
                0.0,
                70.0,
                FONT_M,
                WHITE,
            );
            draw_text(
                "Right mouse button - make cell dead",
                0.0,
                100.0,
                FONT_M,
                WHITE,
            );
            draw_text(
                "Space              - pause/unpause game",
                0.0,
                130.0,
                FONT_M,
                WHITE,
            );
            draw_text(
                "H                  - help menu",
                0.0,
                160.0,
                FONT_M,
                WHITE,
            );
            draw_text(
                "C                  - clear the board",
                0.0,
                190.0,
                FONT_M,
                WHITE,
            );
            draw_text(
                &format!(
                    "+                  - add {UPDATE_TIME_STEP:.2}s to update time ({:.2}s)", *update_frame_cap
                ),
                0.0,
                220.0,
                FONT_M,
                WHITE,
            );
            draw_text(
                &format!("-                  - subtract {UPDATE_TIME_STEP:.2}s to update time ({:.2}s)", *update_frame_cap),
                0.0,
                250.0,
                FONT_M,
                WHITE,
            );

            draw_text(
                "Press ESC or H to return",
                0.0,
                WINDOW_H as f32 - FONT_L,
                FONT_L,
                WHITE,
            );
            draw_text(
                "Made by Kian (Kvoid)",
                WINDOW_W as f32 - 190.0,
                WINDOW_H as f32 - FONT_S,
                FONT_S,
                GRAY,
            );
            return;
        }

        // draw the cells
        for y in 0..GRID_H {
            for x in 0..GRID_W {
                draw_rectangle(
                    (x * SCALE) as f32,
                    (y * SCALE) as f32,
                    SCALE as f32,
                    SCALE as f32,
                    if let Cell::Alive = self.cells[y][x] {
                        WHITE
                    } else {
                        BLACK
                    },
                );
            }
        }

        // draw mouse hover
        if let State::DesignMode = self.state {
            let dim = SCALE as f32;
            draw_rectangle_lines(mouse_pos.0 * dim, mouse_pos.1 * dim, dim, dim, 2.0, GREEN);

            draw_text("[DESIGN MODE]", 0.0, WINDOW_H as f32 - 10.0, 30.0, GREEN);
        }
    }
}
