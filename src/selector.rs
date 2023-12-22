use macroquad::{
    prelude::*,
    ui::root_ui
};

pub enum SelectedGame {
    None,
    GameOfLife,
    Pong,
    Dvd,
}

pub struct Selector {
    window_size: Vec2,
}

impl crate::game::Game for Selector {
    fn new() -> Self {
        let mut skin = root_ui().default_skin();
        skin.button_style = root_ui()
            .style_builder()
            .font_size(40)
            .color_hovered(GRAY)
            .color_clicked(DARKGRAY)
            .build();
        
        skin.label_style = root_ui()
            .style_builder()
            .font_size(24)
            .text_color(GRAY)
            .build();

        root_ui().push_skin(&skin);

        Self {
            window_size: Vec2::new(screen_width(), screen_height()),
        }
    }
    fn update(&mut self) -> SelectedGame {
        let mut sel = SelectedGame::None;

        self.window_size = (screen_width(), screen_height()).into();

        clear_background(BLACK);
        draw_text(
            "MultiSim.rs",
            self.window_size.x / 2.0 - 160.0,
            self.window_size.y / 6.0,
            70.0,
            WHITE,
        );

        let button_x = self.window_size.x / 2.0 - 145.0;
        let button_y = self.window_size.y / 3.0;

        if root_ui().button(Vec2::new(button_x, button_y), "   Game of Life  ") {
            sel = SelectedGame::GameOfLife
        }
        if root_ui().button(Vec2::new(button_x, button_y + 50.0), " DvD bouncin sim ") {
            sel = SelectedGame::Dvd;
        }
        if root_ui().button(Vec2::new(button_x, button_y + 100.0), "       Pong      ") {
            sel = SelectedGame::Pong;
        }
        root_ui().label(
            Vec2::new(0.0, self.window_size.y - 20.0),
            env!("CARGO_PKG_VERSION")
        );

        sel
    }
}
