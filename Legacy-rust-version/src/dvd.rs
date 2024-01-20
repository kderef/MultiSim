use raylib_ffi::colors;
use raylib_ffi::enums::KeyboardKey;

use crate::gamestate::SelectedGame;
use crate::raylib_port::{Vector2, Texture2D, screen_width, screen_height, Image, get_random, get_last_key_pressed, get_frame_time, VEC2_ZERO, clear_background};

const DVD_LOGO_BYTES: &[u8] = include_bytes!("../assets/DVD_logo.png");

pub struct Dvd {
    pos: Vector2,
    velocity: Vector2,
    window_size: Vector2,
    logo: Texture2D,
    logo_size: Vector2,
}

impl crate::game::Game for Dvd {
    fn new() -> Self {
        let window_size = Vector2::new(screen_width() as f32, screen_height() as f32);
        let logo = Texture2D::from_image(Image::from_bytes(".png", DVD_LOGO_BYTES));
        let logo_size = logo.size();

        let rand_x = get_random(0, (window_size.x - logo_size.x) as i32);
        let rand_y = get_random(0, (window_size.y - logo_size.y) as i32);

        Self {
            pos: Vector2::new(rand_x as f32, rand_y as f32),
            velocity: Vector2::new(180.0, 180.0),
            window_size,
            logo_size,
            logo,
        }
    }

    fn update(&mut self) -> SelectedGame {
        let next = if let Some(KeyboardKey::Escape) = get_last_key_pressed() {
            SelectedGame::None
        } else {
            SelectedGame::Dvd
        };

        let dt = get_frame_time();

        self.window_size.x = screen_width() as f32;
        self.window_size.y = screen_height() as f32;

        self.pos = self
            .pos
            .clamp(VEC2_ZERO, self.window_size - self.logo_size)
            + self.velocity * dt;

        if self.pos.x + self.logo_size.x >= self.window_size.x || self.pos.x <= 0.0 {
            self.velocity.x *= -1.0;
        }
        if self.pos.y + self.logo_size.y >= self.window_size.y || self.pos.y <= 0.0 {
            self.velocity.y *= -1.0;
        }

        clear_background(colors::RED);
        self.logo.draw(self.pos.x as i32, self.pos.y as i32, colors::WHITE);

        next
    }
}
