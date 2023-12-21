use macroquad::{prelude::*, miniquad::window::set_window_size};

use crate::selector;

const DVD_LOGO_BYTES: &[u8] = include_bytes!("../assets/DVD_logo.png");

pub struct Dvd {
    pos: Vec2,
    velocity: Vec2,
    window_size: Vec2,
    logo: Texture2D,
    logo_size: Vec2,
}

impl crate::game::Game for Dvd {
    fn new() -> Self {
        let window_size = Vec2::new(screen_width(), screen_height());
        let logo = Texture2D::from_file_with_format(DVD_LOGO_BYTES, Some(ImageFormat::Png));
        let logo_size = logo.size();

        let rand_x = macroquad::rand::rand() % (window_size.x as u32 - logo_size.x as u32);
        let rand_y = macroquad::rand::rand() % (window_size.y as u32 - logo_size.y as u32);
        

        Self {
            pos: Vec2::new(
                rand_x as f32, rand_y as f32
            ),
            velocity: vec2(1.0, 1.0),
            window_size,
            logo_size,
            logo,
        }
    }

    fn update(&mut self) -> selector::SelectedGame {
        let sel = if is_key_pressed(KeyCode::Escape) {
            selector::SelectedGame::None
        } else {
            selector::SelectedGame::Dvd
        };

        self.window_size.x = screen_width();
        self.window_size.y = screen_height();

        self.pos = self.pos.clamp(vec2(0., 0.), self.window_size - self.logo_size) + self.velocity;

        if self.pos.x + self.logo_size.x >= self.window_size.x || self.pos.x <= 0.0 {
            self.velocity.x *= -1.0;
        }
        if self.pos.y + self.logo_size.y >= self.window_size.y || self.pos.y <= 0.0 {
            self.velocity.y *= -1.0;
        }

        clear_background(WHITE);
        draw_texture(&self.logo, self.pos.x, self.pos.y, BLACK);

        sel
    }
}