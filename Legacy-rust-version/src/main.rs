#![allow(non_snake_case)]
#![cfg_attr(all(target_os = "windows", not(debug_assertions)), windows_subsystem = "windows")]

use dvd::Dvd;
use raylib_ffi::colors;
use raylib_port::{init_audio_device, Window, begin_drawing, clear_background, end_drawing, Image, Rectangle};

mod game;
mod gamestate;

// ports of unsafe code
mod  raylib_port;

mod gol_consts;
mod dvd;

/*
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
*/

use game::Game;

fn main() {
    // seed rand
    init_audio_device();

    let mut window = Window::init(500, 500, "Hello, World!");
    window.set_target_fps(120);
    window.set_exit_key(raylib_ffi::enums::KeyboardKey::Null);
    //window.set_icon();

    window.show();

    let mut dvd = Dvd::new();

    while !window.should_close() {
        begin_drawing();
        dvd.update();
        end_drawing();
    }

    /*

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
    */
}
