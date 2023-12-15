#![cfg_attr(target_os = "windows", windows_subsystem = "windows")]

mod cell;
mod consts;
mod gol;
mod theme;
mod universe;

use std::time::{UNIX_EPOCH, SystemTime};

use consts::{WINDOW_H, WINDOW_W};
use gol::GameOfLife;
use macroquad::{miniquad::conf::Icon, prelude::*};

fn window_conf() -> Conf {
    Conf {
        window_title: "Game of Life | H - Help".to_owned(),
        window_width: WINDOW_W as i32,
        window_height: WINDOW_H as i32,
        high_dpi: true,
        fullscreen: false,
        window_resizable: true,
        sample_count: Default::default(),
        icon: Some(Icon::miniquad_logo()),
        platform: Default::default(),
    }
}

#[macroquad::main(window_conf)]
async fn main() {
    // seed rand
    macroquad::rand::srand(
        SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .and_then(|t| Ok(t.as_secs()))
            .unwrap_or(69),
    );

    // instantiate game
    let mut gol = GameOfLife::new();
    gol.update_screen_size();

    loop {
        gol.run();
        next_frame().await;
    }
}
