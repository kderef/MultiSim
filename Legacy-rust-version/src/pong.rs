use crate::selector::SelectedGame;
use macroquad::{
    audio::{play_sound, PlaySoundParams, Sound},
    prelude::*,
};

const PADDLE_WIDTH: f32 = 200.0;
const PADDLE_HEIGHT: f32 = 10.0;
const PADDLE_PADDING: f32 = 20.0;
const PADDLE_SPEED: f32 = 180.0;

const BALL_RADIUS: f32 = 5.0;
const DEFAULT_VELOCITY: Vec2 = vec2(170.0, 170.0);

struct Ball {
    velocity: Vec2,
    hitbox: Rect,
}

pub struct Pong {
    window_size: Vec2,
    ball: Ball,
    score: (u32, u32),
    paddle_top: Rect,
    paddle_bottom: Rect,
    countdown_passed: Option<f32>,
    pub sound: Option<Sound>,
}

impl Pong {
    fn reset(&mut self, bottom_scored: bool) {
        let paddle_x = self.window_size.x / 2.0 - PADDLE_WIDTH / 2.0;

        self.paddle_bottom.x = paddle_x;
        self.paddle_top.x = paddle_x;
        self.ball.hitbox = Rect::new(
            self.window_size.x / 2.0,
            if bottom_scored {
                self.paddle_top.y + PADDLE_PADDING
            } else {
                self.paddle_bottom.y - PADDLE_PADDING
            },
            BALL_RADIUS,
            BALL_RADIUS,
        );
        self.ball.velocity = DEFAULT_VELOCITY;

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

        draw_circle(self.ball.hitbox.x, self.ball.hitbox.y, BALL_RADIUS, WHITE);

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
        let win_size_center = win_size / 2.0;
        let paddle_x = win_size_center.x - PADDLE_WIDTH / 2.0;

        Self {
            ball: Ball {
                velocity: DEFAULT_VELOCITY,
                hitbox: Rect::new(
                    win_size_center.x,
                    win_size_center.y,
                    BALL_RADIUS,
                    BALL_RADIUS,
                ),
            },
            score: (0, 0),
            paddle_top: Rect::new(paddle_x, PADDLE_PADDING, PADDLE_WIDTH, PADDLE_HEIGHT),
            paddle_bottom: Rect::new(
                paddle_x,
                win_size.y - PADDLE_HEIGHT - PADDLE_PADDING,
                PADDLE_WIDTH,
                PADDLE_HEIGHT,
            ),
            window_size: win_size,
            countdown_passed: Some(0.0),
            sound: None,
        }
    }
    fn update(&mut self) -> SelectedGame {
        self.window_size = Vec2::new(screen_width(), screen_height());
        self.paddle_bottom.y = self.window_size.y - PADDLE_HEIGHT - PADDLE_PADDING;
        self.paddle_top.y = PADDLE_PADDING;

        let dt = get_frame_time();

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
            let fixed_paddle_speed = PADDLE_SPEED * dt;
            if is_key_down(KeyCode::A) {
                self.paddle_bottom.x = (self.paddle_bottom.x - fixed_paddle_speed)
                    .clamp(0.0, self.window_size.x - PADDLE_WIDTH);
            }
            if is_key_down(KeyCode::D) {
                self.paddle_bottom.x = (self.paddle_bottom.x + fixed_paddle_speed)
                    .clamp(0.0, self.window_size.x - PADDLE_WIDTH);
            }
            if is_key_down(KeyCode::Left) {
                self.paddle_top.x = (self.paddle_top.x - fixed_paddle_speed)
                    .clamp(0.0, self.window_size.x - PADDLE_WIDTH);
            }
            if is_key_down(KeyCode::Right) {
                self.paddle_top.x = (self.paddle_top.x + fixed_paddle_speed)
                    .clamp(0.0, self.window_size.x - PADDLE_WIDTH);
            }
        }

        match self.countdown_passed {
            None => {
                const AUDIO_PARAMS: PlaySoundParams = PlaySoundParams {
                    looped: false,
                    volume: 0.3,
                };

                self.ball.hitbox.x += self.ball.velocity.x * dt;
                self.ball.hitbox.y += self.ball.velocity.y * dt;

                // screen side collide || side paddle collide
                if self.ball.hitbox.x - BALL_RADIUS <= 0.0
                    || self.ball.hitbox.x + BALL_RADIUS >= self.window_size.x
                    || (self.ball.hitbox.overlaps(&self.paddle_top) && self.ball.hitbox.y <= self.paddle_top.bottom())
                {
                    self.ball.velocity.x *= -1.0;
                    play_sound(self.sound.as_ref().unwrap(), AUDIO_PARAMS);
                }

                // bottom scored
                if self.ball.hitbox.y + BALL_RADIUS <= 0.0 {
                    self.reset(true);
                    self.score.0 += 1;
                }
                // top scored
                if self.ball.hitbox.y + BALL_RADIUS >= self.window_size.y {
                    self.reset(false);
                    self.score.1 += 1;
                }

                // bottom paddle
                let bottom_paddle_collided = self.paddle_bottom.overlaps(&self.ball.hitbox);
                let top_paddle_collided = self.paddle_top.overlaps(&self.ball.hitbox);

                // top paddle
                if bottom_paddle_collided || top_paddle_collided {
                    self.ball.velocity.y *= -1.0;
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
