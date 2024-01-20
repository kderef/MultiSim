pub trait Game {
    fn new() -> Self;
    /// draw to the screen and return if Esc was pressed
    fn update(&mut self) -> crate::gamestate::SelectedGame;
}