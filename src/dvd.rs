use macroquad::prelude::*;

use crate::selector::SelectedGame;

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

        let rand_x = rand::gen_range(0.0, window_size.x - logo_size.x);
        let rand_y = rand::gen_range(0.0, window_size.y - logo_size.y);

        Self {
            pos: Vec2::new(rand_x, rand_y),
            velocity: vec2(180.0, 180.0),
            window_size,
            logo_size,
            logo,
        }
    }

    fn update(&mut self) -> SelectedGame {
        let next = if let Some(key) = get_last_key_pressed() {
            match key {
                KeyCode::Escape => SelectedGame::None,
                _ => SelectedGame::Dvd,
            }
        } else {
            SelectedGame::Dvd
        };

        let dt = get_frame_time();

        self.window_size.x = screen_width();
        self.window_size.y = screen_height();

        self.pos = self
            .pos
            .clamp(vec2(0., 0.), self.window_size - self.logo_size)
            + self.velocity * dt;

        if self.pos.x + self.logo_size.x >= self.window_size.x || self.pos.x <= 0.0 {
            self.velocity.x *= -1.0;
        }
        if self.pos.y + self.logo_size.y >= self.window_size.y || self.pos.y <= 0.0 {
            self.velocity.y *= -1.0;
        }

        clear_background(WHITE);
        draw_texture(
            &self.logo,
            self.pos.x,
            self.pos.y,
            WHITE
        );

        next
    }
}
