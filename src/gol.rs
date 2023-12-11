use raylib::prelude::*;

#[repr(u8)]
#[derive(Clone, Copy, Debug)]
pub enum Cell {
    Dead = 0,
    Alive = 1
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
    fn calculate_neighbours(&self, x: usize, y: usize) -> usize {
        const NEIGHBOR_OFFSETS: [(i32, i32); 8] = [
            (-1, -1), (0, -1), (1, -1),
            (-1, 0),          (1, 0),
            (-1, 1),  (0, 1),  (1, 1),
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
                    }
                    KeyboardKey::KEY_C => {
                        self.cells = CELLS_EMPTY;
                    }
                    KeyboardKey::KEY_SPACE => {
                        let title;
                        (self.state, title) = match self.state {
                            State::DesignMode => (State::SimMode, TITLE_SIM_MODE),
                            State::SimMode => (State::DesignMode, TITLE_DESIGN_MODE),
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
