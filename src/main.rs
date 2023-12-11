#![cfg_attr(target_os = "windows", windows_subsystem = "windows")]

mod gol;

use gol::{
    WINDOW_W, WINDOW_H,
    GameOfLife, TITLE_DESIGN_MODE,
};

fn main() {
    let (rl, thread) = raylib::init()
        .size(WINDOW_W as i32, WINDOW_H as i32)
        .title(TITLE_DESIGN_MODE)
        .build();

    let mut gol = GameOfLife::new(rl, thread);

    gol.prepare();
    gol.run();
}
