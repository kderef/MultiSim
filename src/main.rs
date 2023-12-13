// #![cfg_attr(target_os = "windows", windows_subsystem = "windows")]

mod cell;
mod gol;
mod universe;

use gol::{GameOfLife, WINDOW_H, WINDOW_W};
use macroquad::prelude::*;

fn window_conf() -> Conf {
    Conf {
        window_title: "Game of Life | H - Help".to_owned(),
        window_width: WINDOW_W as i32,
        window_height: WINDOW_H as i32,
        high_dpi: true,
        fullscreen: false,
        window_resizable: true,
        sample_count: Default::default(),
        icon: None,
        platform: Default::default()
    }
}

#[macroquad::main(window_conf)]
async fn main() {
    // instantiate game
    let mut gol = GameOfLife::new();

    loop {
        gol.run();
        next_frame().await;
    }
    
}
