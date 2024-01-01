#![allow(non_snake_case)]
#![cfg_attr(all(target_os = "windows", not(debug_assertions)), windows_subsystem = "windows")]

// game trait
mod game;

// Selector for the games
mod selector;

// Pong
mod pong;

// Game of Life
mod gol_cell;
mod gol_consts;
mod gol_game;
mod gol_theme;
mod gol_universe;

// DVD bouncing
mod dvd;

use gol_consts::{WINDOW_H, WINDOW_W};
use selector::Selector;

use std::time::{UNIX_EPOCH, SystemTime};
use macroquad::{miniquad::conf::Icon, prelude::*, audio::load_sound_from_bytes};

/// Window configuration
fn window_conf() -> Conf {
    Conf {
        window_title: "MultiSim-rs".to_owned(),
        window_width: WINDOW_W as i32,
        window_height: WINDOW_H as i32,
        high_dpi: true,
        fullscreen: false,
        window_resizable: true,
        sample_count: Default::default(),
        icon: Some(Icon::miniquad_logo()),
        platform: Default::default()
    }
}

#[macroquad::main(window_conf)]
async fn main() {
    // seed rand
    macroquad::rand::srand(
        SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .map(|t| t.as_secs())
            .unwrap_or(69),
    );


    // instantiate games
    let mut selector = Selector::new();

    // FUCK the person who made this function async!
    selector.set_pong_sound(load_sound_from_bytes(
        include_bytes!("../assets/beep.wav")
    ).await.unwrap());

    loop {
        selector.update();
        next_frame().await;
    }
}
