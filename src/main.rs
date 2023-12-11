#![cfg_attr(target_os = "windows", windows_subsystem = "windows")]

mod gol;
use gol::{GameOfLife, WINDOW_H, WINDOW_W};

fn main() {
    let (rl, thread) = raylib::init()
        .size(WINDOW_W as i32, WINDOW_H as i32)
        .vsync()
        .build();

    let mut gol = GameOfLife::new(rl, thread);

    gol.run();
}
