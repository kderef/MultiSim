use crate::selector::SelectedGame;
use macroquad::{prelude::*, audio::{Sound, play_sound, PlaySoundParams}};

const PADDLE_WIDTH: f32 = 250.0;
const PADDLE_HEIGHT: f32 = 10.0;
const PADDLE_PADDING: f32 = 20.0;
const PADDLE_SPEED: f32 = 1.5;

const BALL_SPEED: f32 = 1.4;
const BALL_RADIUS: f32 = 5.0;

struct Ball {
    velocity: Vec2,
    pos: Vec2,
}

pub struct Pong {
    window_size: Vec2,
    ball: Ball,
    score: (u32, u32),
    paddle_top: Vec2,
    paddle_bottom: Vec2,
    countdown_passed: Option<f32>,
    pub sound: Option<Sound>,
}

impl Pong {
    fn reset(&mut self, bottom_scored: bool) {
        let paddle_x = self.window_size.x / 2.0 - PADDLE_WIDTH / 2.0;

        self.paddle_bottom.x = paddle_x;
        self.paddle_top.x = paddle_x;
        self.ball.pos = Vec2::new(
            self.window_size.x / 2.0,
            if bottom_scored {
                self.paddle_top.y + PADDLE_PADDING
            } else {
                self.paddle_bottom.x - PADDLE_PADDING
            },
        );
        self.ball.velocity = vec2(1.0, 1.0);

        self.countdown_passed = Some(0.0);
    }
    fn draw(&self) {
        clear_background(BLACK);

        let line_y = self.window_size.y / 2.0;
        let score_x = self.window_size.x / 2.0 - 10.0;

        draw_line(0.0, line_y, self.window_size.x, line_y, 2.0, DARKGRAY);
        draw_text(
            &self.score.1.to_string(),
            score_x,
            line_y - 30.0,
            50.0,
            GRAY,
        );
        draw_text(
            &self.score.0.to_string(),
            score_x,
            line_y + 50.0,
            50.0,
            GRAY,
        );

        draw_circle(self.ball.pos.x, self.ball.pos.y, BALL_RADIUS, WHITE);

        // draw paddles
        draw_rectangle(
            self.paddle_bottom.x,
            self.paddle_bottom.y,
            PADDLE_WIDTH,
            PADDLE_HEIGHT,
            WHITE,
        );
        draw_rectangle(
            self.paddle_top.x,
            self.paddle_top.y,
            PADDLE_WIDTH,
            PADDLE_HEIGHT,
            WHITE,
        );
    }
}

impl crate::game::Game for Pong {
    fn new() -> Self {
        let win_size = Vec2::new(screen_width(), screen_height());
        let paddle_x = win_size.x / 2.0 - PADDLE_WIDTH / 2.0;

        Self {
            ball: Ball {
                velocity: vec2(BALL_SPEED, BALL_SPEED),
                pos: win_size / 2.0,
            },
            score: (0, 0),
            paddle_top: Vec2::new(paddle_x, PADDLE_PADDING),
            paddle_bottom: Vec2::new(paddle_x, win_size.y - PADDLE_HEIGHT - PADDLE_PADDING),
            window_size: win_size,
            countdown_passed: Some(0.0),
            sound: None
        }
    }
    fn update(&mut self) -> SelectedGame {
        self.window_size = Vec2::new(screen_width(), screen_height());
        self.paddle_bottom.y = self.window_size.y - PADDLE_HEIGHT - PADDLE_PADDING;
        self.paddle_top.y = PADDLE_PADDING;

        if is_key_pressed(KeyCode::Escape) {
            return SelectedGame::None;
        }

        if let Some(cp) = self.countdown_passed.as_mut() {
            if *cp >= 3.0 {
                self.countdown_passed = None;
            } else {
                *cp += get_frame_time();
            }
        } else {
            if is_key_down(KeyCode::A) {
                self.paddle_bottom.x = (self.paddle_bottom.x - PADDLE_SPEED)
                    .clamp(0.0, self.window_size.x - PADDLE_WIDTH);
            }
            if is_key_down(KeyCode::D) {
                self.paddle_bottom.x = (self.paddle_bottom.x + PADDLE_SPEED)
                    .clamp(0.0, self.window_size.x - PADDLE_WIDTH);
            }
            if is_key_down(KeyCode::Left) {
                self.paddle_top.x = (self.paddle_top.x - PADDLE_SPEED)
                    .clamp(0.0, self.window_size.x - PADDLE_WIDTH);
            }
            if is_key_down(KeyCode::Right) {
                self.paddle_top.x = (self.paddle_top.x + PADDLE_SPEED)
                    .clamp(0.0, self.window_size.x - PADDLE_WIDTH);
            }
        }

        match self.countdown_passed {
            None => {
                const AUDIO_PARAMS: PlaySoundParams = PlaySoundParams {looped: false, volume: 0.3};

                self.ball.pos += self.ball.velocity;

                // bounding box
                let ball_box = self.ball.pos + BALL_RADIUS;

                if ball_box.x <= 0.0 || ball_box.x >= self.window_size.x {
                    self.ball.velocity.x *= -1.0;
                    play_sound(self.sound.as_ref().unwrap(), AUDIO_PARAMS);
                }
                // bottom scored
                if self.ball.pos.y + BALL_RADIUS <= 0.0 {
                    self.reset(true);
                    self.score.0 += 1;
                }
                // top scored
                if self.ball.pos.y + BALL_RADIUS >= self.window_size.y {
                    self.reset(false);
                    self.score.1 += 1;
                }
                if self.ball.pos.x >= self.paddle_bottom.x
                    && self.ball.pos.x <= self.paddle_bottom.x + PADDLE_WIDTH
                    && self.ball.pos.y >= self.paddle_bottom.y
                    && self.ball.pos.y <= self.paddle_bottom.y + PADDLE_HEIGHT
                {
                    // Handle collision with the bottom paddle
                    self.ball.velocity.y *= -1.0; // Reverse the ball's y velocity (assuming it's vertical)
                    play_sound(self.sound.as_ref().unwrap(), AUDIO_PARAMS);
                }

                // Check collision with the top paddle
                if self.ball.pos.x >= self.paddle_top.x
                    && self.ball.pos.x <= self.paddle_top.x + PADDLE_WIDTH
                    && self.ball.pos.y >= self.paddle_top.y
                    && self.ball.pos.y <= self.paddle_top.y + PADDLE_HEIGHT
                {
                    // Handle collision with the top paddle
                    self.ball.velocity.y *= -1.0; // Reverse the ball's y velocity (assuming it's vertical)
                    play_sound(self.sound.as_ref().unwrap(), AUDIO_PARAMS);
                }
                self.draw();
            }
            Some(cp) => {
                self.draw();

                let x = self.window_size.x / 2.0 - 30.0;
                let y = self.window_size.y / 2.0 + 28.0;
                let size = 125.0;

                if cp <= 1.0 {
                    draw_text("3", x, y, size, GREEN);
                } else if cp <= 2.0 {
                    draw_text("2", x, y, size, YELLOW);
                } else {
                    draw_text("1", x, y, size, RED);
                }
            }
        }

        SelectedGame::Pong
    }
}
