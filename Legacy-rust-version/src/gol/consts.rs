pub const SCALE: usize = 16;
pub const SCALE_U32: u32 = SCALE as u32;

pub const WINDOW_W: usize = 800;
pub const WINDOW_H: usize = 640;

pub const GRID_W: usize = WINDOW_W / SCALE;
pub const GRID_H: usize = WINDOW_H / SCALE;

pub const UPDATE_TIME_STEP: f32 = 0.05;
pub const DEFAULT_UPDATE_CAP: f32 = 0.50;

pub const RESIZE_TIME_LIMIT: f32 = 0.2;

pub const BOLUS: &[u8] = include_bytes!("../assets/bolus.png");