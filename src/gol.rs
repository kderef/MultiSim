use crate::cell::Cell;
use crate::universe::Universe;
use macroquad::{miniquad::window::set_window_size, prelude::*};

const SCALE: usize = 15;

pub const WINDOW_W: usize = 900;
pub const WINDOW_H: usize = 750;

pub const GRID_W: usize = WINDOW_W / SCALE;
pub const GRID_H: usize = WINDOW_H / SCALE;

const UPDATE_TIME_STEP: f32 = 0.05;
const DEFAULT_UPDATE_CAP: f32 = 0.50;

/*****************************************************************/

#[derive(Clone, Debug)]
enum State {
    DesignMode,
    SimulationMode,
    HelpMode,
}

/// game struct
pub struct GameOfLife {
    universe: Universe,
    state: State,
    update_frame_cap: f32,
    passed_time: f32,
    mouse_pos: Vec2,
    window_width: u32,
    window_height: u32,
}

impl GameOfLife {
    pub fn new() -> Self {
        Self {
            universe: Universe::new(GRID_W, GRID_H),
            state: State::DesignMode,
            update_frame_cap: DEFAULT_UPDATE_CAP,
            passed_time: 0.0,
            mouse_pos: (0.0, 0.0).into(),
            window_width: 0,
            window_height: 0,
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

            if (nx >= 0 && nx < self.universe.width as i32)
                && (ny >= 0 && ny < self.universe.height as i32)
            {
                if let Cell::Alive = self.universe.get(nx as usize, ny as usize) {
                    neighbours += 1;
                }
            }
        }
        neighbours
    }
    fn enforce_minimun_screen_size(&mut self) {
        if self.window_width < WINDOW_W as u32 || self.window_height < WINDOW_H as u32 {
            let new_width = self.window_width.max(WINDOW_W as u32);
            let new_height = self.window_height.max(WINDOW_H as u32);

            set_window_size(new_width, new_height);
            self.window_width = new_width;
            self.window_height = new_height;
        }
    }
    fn screen_size_changed(&mut self) -> bool {
        let new_width = screen_width().round() as u32;
        let new_height = screen_height().round() as u32;

        let changed = (self.window_width > 0 && self.window_height > 0)
            && (new_width != self.window_width || new_height != self.window_height);

        self.window_width = new_width;
        self.window_height = new_height;

        changed
    }
    fn handle_keys(&mut self) {
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
                    self.universe.clear();
                }
                KeyCode::Equal | KeyCode::KpEqual => {
                    self.update_frame_cap += UPDATE_TIME_STEP;
                }
                KeyCode::KpSubtract | KeyCode::Minus => {
                    self.update_frame_cap =
                        (self.update_frame_cap - UPDATE_TIME_STEP).clamp(0.0, f32::MAX);
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
    }
    fn update_cells(&mut self) {
        let mut next_generation = self.universe.clone();

        for y in 0..self.universe.height {
            for x in 0..self.universe.width {
                let neighbours = self.calculate_neighbours(x, y);
                let new_state = self.universe.get(x, y).calculate_next_iteration(neighbours);
                next_generation.set(x, y, new_state);
            }
        }

        self.universe = next_generation;
    }

    pub fn run(&mut self) {
        if self.passed_time >= self.update_frame_cap {
            self.passed_time = 0.0;
        }
        self.passed_time += get_frame_time();

        self.handle_keys();

        if self.screen_size_changed() {
            self.enforce_minimun_screen_size();
            self.universe.resize(self.window_width as usize, self.window_height as usize);
        }

        // get mouse position inside grid
        self.mouse_pos = Vec2::from(mouse_position()).floor().clamp(
            vec2(0.0, 0.0),
            Vec2::new(
                self.window_width as f32 - 1.0,
                self.window_height as f32 - 1.0,
            ),
        ) / SCALE as f32;

        match self.state {
            State::SimulationMode => {
                if self.passed_time >= self.update_frame_cap {
                    self.update_cells();
                    self.passed_time = 0.0;
                }
            }
            State::DesignMode => {
                if is_mouse_button_down(MouseButton::Left) {
                    self.universe.set(
                        self.mouse_pos.x as usize,
                        self.mouse_pos.y as usize,
                        Cell::Alive,
                    );
                } else if is_mouse_button_down(MouseButton::Right) {
                    self.universe.set(
                        self.mouse_pos.x as usize,
                        self.mouse_pos.y as usize,
                        Cell::Dead,
                    );
                }
            }
            _ => {}
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
            draw_text("H                  - help menu", 0.0, 160.0, FONT_M, WHITE);
            draw_text(
                "C                  - clear the board",
                0.0,
                190.0,
                FONT_M,
                WHITE,
            );
            draw_text(
                &format!(
                    "+                  - add {UPDATE_TIME_STEP:.2}s to update time ({:.2}s)",
                    self.update_frame_cap
                ),
                0.0,
                220.0,
                FONT_M,
                WHITE,
            );
            draw_text(
                &format!(
                    "-                  - subtract {UPDATE_TIME_STEP:.2}s to update time ({:.2}s)",
                    self.update_frame_cap
                ),
                0.0,
                250.0,
                FONT_M,
                WHITE,
            );

            draw_text(
                "Press ESC or H to return",
                0.0,
                self.window_height as f32 - FONT_L,
                FONT_L,
                WHITE,
            );
            draw_text(
                "Made by Kian (Kvoid)",
                self.window_width as f32 - 190.0,
                self.window_height as f32 - FONT_S,
                FONT_S,
                GRAY,
            );
            return;
        }

        // draw the cells
        for y in 0..self.universe.height {
            for x in 0..self.universe.width {
                draw_rectangle(
                    (x * SCALE) as f32,
                    (y * SCALE) as f32,
                    SCALE as f32,
                    SCALE as f32,
                    if let Cell::Alive = self.universe.get(x, y) {
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
            draw_rectangle_lines(
                self.mouse_pos.x.floor() * dim,
                self.mouse_pos.y.floor() * dim,
                dim,
                dim,
                2.0,
                GREEN,
            );

            draw_text(
                "[DESIGN MODE]",
                0.0,
                self.window_height as f32 - 10.0,
                30.0,
                GREEN,
            );
        }
    }
}
