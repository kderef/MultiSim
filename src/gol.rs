use crate::cell::Cell;
use crate::consts::*;
use crate::universe::Universe;
use macroquad::{miniquad::window::set_window_size, prelude::*};

/*****************************************************************/

#[derive(Clone, Debug)]
enum State {
    DesignMode,
    SimulationMode,
    HelpMode,
}

impl State {
    pub fn toggle(&mut self) {
        *self = match self {
            State::DesignMode => Self::SimulationMode,
            State::SimulationMode => Self::DesignMode,
            State::HelpMode => Self::HelpMode,
        };
    }
}

/// game struct
pub struct GameOfLife {
    universe: Universe,
    state: State,
    update_frame_cap: f32,
    passed_time: f32,
    passed_resize_time: f32,
    resize_queued: bool,
    mouse_pos: Vec2,
    window_width: u32,
    window_height: u32,
}

impl GameOfLife {
    pub fn new() -> Self {
        Self {
            universe: Universe::new(GRID_W, GRID_H),
            state: State::HelpMode,
            update_frame_cap: DEFAULT_UPDATE_CAP,
            passed_time: 0.0,
            passed_resize_time: 0.0,
            resize_queued: false,
            mouse_pos: (0.0, 0.0).into(),
            window_width: 0,
            window_height: 0,
        }
    }
    pub fn update_screen_size(&mut self) {
        self.window_width = screen_width() as u32;
        self.window_height = screen_height() as u32;
    }
    fn enforce_minimun_screen_size(&mut self) {
        const WIN_W_U32: u32 = WINDOW_W as u32;
        const WIN_H_U32: u32 = WINDOW_H as u32;

        println!("{} < {WIN_W_U32} || {} < {WIN_H_U32}", self.window_width, self.window_height);
        if self.window_width < WIN_W_U32 || self.window_height < WIN_H_U32 {
            self.window_width = self.window_width.max(WIN_W_U32);
            self.window_height = self.window_height.max(WIN_H_U32);

            set_window_size(self.window_width, self.window_height); 
        }
    }
    fn screen_size_changed(&mut self) -> bool {
        let new_width = screen_width().floor() as u32;
        let new_height = screen_height().floor() as u32;

        let changed = new_width != self.window_width || new_height != self.window_height;

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
                    self.universe.fill(Cell::Dead);
                }
                KeyCode::A => {
                    self.universe.fill(Cell::Alive);
                }
                KeyCode::Equal | KeyCode::KpEqual => {
                    self.update_frame_cap += UPDATE_TIME_STEP;
                }
                KeyCode::KpSubtract | KeyCode::Minus => {
                    self.update_frame_cap =
                        (self.update_frame_cap - UPDATE_TIME_STEP).clamp(0.0, f32::MAX);
                }
                KeyCode::Space => {
                    self.state.toggle();
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
                let cell_state = self.universe.get(x, y);
                let mut neighbours = 0;

                for dx in -1..=1 {
                    for dy in -1..=1 {
                        if dx == 0 && dy == 0 {
                            continue;
                        }

                        let nx = x as i32 + dx;
                        let ny = y as i32 + dy;

                        if nx >= 0
                            && nx < self.universe.width as i32
                            && ny >= 0
                            && ny < self.universe.height as i32
                            && matches!(self.universe.get(nx as usize, ny as usize), Cell::Alive)
                        {
                            neighbours += 1;
                        }
                    }
                }

                let new_state = cell_state.calculate_next_iteration(neighbours);
                next_generation.set(x, y, new_state);
            }
        }

        self.universe = next_generation;
    }

    pub fn run(&mut self) {
        if self.passed_time >= self.update_frame_cap {
            self.passed_time = 0.0;
        }
        let dt = get_frame_time();
        self.passed_time += dt;
        self.passed_resize_time += dt;

        self.handle_keys();

        let size_changed = self.screen_size_changed();
        if size_changed || self.resize_queued {
            if size_changed {
                //  self.enforce_minimun_screen_size();
            }
            if self.passed_resize_time >= RESIZE_TIME_LIMIT {
                let new_w = self.window_width / SCALE_U32;
                let new_h = self.window_height / SCALE_U32;

                self.universe.resize(new_w as usize, new_h as usize);
                self.resize_queued = false;
                self.passed_resize_time = 0.0;
            } else {
                self.resize_queued = true;
            }
        }
        println!("{} FPS", get_fps());
        println!("{}x{}", self.window_width, self.window_height);

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

            let mut spacing = FONT_L;

            draw_text("CONTROLS", 0.0, spacing, FONT_XL, GREEN);
            spacing += FONT_L;
            draw_text(
                "Left mouse button  - make cell alive",
                0.0,
                spacing,
                FONT_M,
                WHITE,
            );
            spacing += FONT_M;
            draw_text(
                "Right mouse button - make cell dead",
                0.0,
                spacing,
                FONT_M,
                WHITE,
            );
            spacing += FONT_M;
            draw_text(
                "Space              - pause/unpause game",
                0.0,
                spacing,
                FONT_M,
                WHITE,
            );
            spacing += FONT_M;
            draw_text("H                  - help menu", 0.0, spacing, FONT_M, WHITE);
            spacing += FONT_M;
            draw_text(
                "C                  - clear the board",
                0.0,
                spacing,
                FONT_M,
                WHITE,
            );
            spacing += FONT_M;
            draw_text(
                "A                  - fill the board with live cells",
                0.0, spacing,
                FONT_M, WHITE
            );
            spacing += FONT_M;
            draw_text(
                &format!(
                    "+                  - add {UPDATE_TIME_STEP:.2}s to update time ({:.2}s)",
                    self.update_frame_cap
                ),
                0.0,
                spacing,
                FONT_M,
                WHITE,
            );
            spacing += FONT_M;
            draw_text(
                &format!(
                    "-                  - subtract {UPDATE_TIME_STEP:.2}s to update time ({:.2}s)",
                    self.update_frame_cap
                ),
                0.0,
                spacing,
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
