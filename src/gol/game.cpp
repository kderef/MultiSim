#ifndef GOL_GAME_C_
#define GOL_GAME_C_

#include <stdint.h>
#include <math.h>
#include <iostream>

using std::min;
using std::max;

#include "raylib.h"
#include "raymath.h"

#include "universe.cpp"
#include "theme.cpp"
#include "../ui/font.c"
#include "../assets.h"

//! The general game code. Things such as rendering and state management
//! are done in here.

struct GameOfLife {
private:
    float dt;
    bool size_changed;
    int key, scrollwheel_move;
    ThemeStyle theme_style;
    char buffer[512];
    // dt shit
    bool draw_update_time, mouse_left_down, mouse_right_down;
    float passed_time;
    float passed_resize_time;
    float passed_show_scroll_time;
    bool mouse_in_grid;

public:
    Universe universe;
    GameState state;
    float update_frame_cap;
    int window_width;
    int window_height;
    bool resize_queued;
    Vector2 mouse_pos;
    Theme theme;
    Theme prev_theme;
    uint64_t iterations;

    GameOfLife() {
        universe = Universe(GOL_GRID_W, GOL_GRID_H);
        state = GameState_Paused;
        window_width = 0;
        window_height = 0;
        resize_queued = false;
        mouse_pos = vec2(0, 0);
        update_frame_cap = GOL_DEFAULT_UPDATE_CAP;

        theme = Theme_Default;
        prev_theme = Theme_Gruvbox;
        iterations = 0;
    }

    void toggle_state() {
        if (state == GameState_Paused) {
            iterations = 0;
            state = GameState_Running;
        } else state = GameState_Paused;
    }

    bool screen_size_changed() {
        bool changed = global_screen_width != window_width || global_screen_height != window_height;

        window_width = global_screen_width;
        window_height = global_screen_height;

        return changed;
    }

    SelectedGame update() {
        dt = GetFrameTime();

        passed_time = (passed_time >= update_frame_cap)? 0.0f : passed_time + dt;
        passed_resize_time += dt;
        passed_show_scroll_time += dt;

        // handle window size
        size_changed = this->screen_size_changed();

        if (size_changed || resize_queued) {
            int new_w = window_width / GOL_SCALE;
            int new_h = (window_height - GOL_STATUS_BAR_HEIGHT) / GOL_SCALE;

            universe.resize(new_w, new_h);
            resize_queued = false;
            passed_resize_time = 0.0;
        } else {
            resize_queued = true;
        }

        // handle mouse position
        mouse_pos = GetMousePosition();
        mouse_pos.x = Clamp(floorf(mouse_pos.x), 0.0, (float)(window_width) - 1.0) / (float)GOL_SCALE;
        mouse_pos.y = Clamp(floorf(mouse_pos.y), 0.0, (float)(window_height - GOL_STATUS_BAR_HEIGHT) - 1.0) / (float)GOL_SCALE;
        mouse_left_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        mouse_right_down = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

        mouse_in_grid = mouse_pos.y < (window_height - GOL_STATUS_BAR_HEIGHT - 1.0) / GOL_SCALE;

        // handle the keys
        scrollwheel_move = GetMouseWheelMove();
        key = GetKeyPressed();

        if (key == KEY_MINUS || key == KEY_KP_SUBTRACT || scrollwheel_move < 0.0f) {
            update_frame_cap = max(
                0.0f, update_frame_cap - GOL_DEFAULT_TIME_STEP
            );
            draw_update_time = true;
            passed_show_scroll_time = 0.0;
        }
        if (key == KEY_EQUAL || key == KEY_KP_EQUAL || scrollwheel_move > 0.0f) {
            update_frame_cap += GOL_DEFAULT_TIME_STEP;
            draw_update_time = true;
            passed_show_scroll_time = 0.0;
        }

        switch (key) {
            case 0: break;
            case KEY_C: {
                universe.fill(Dead);
            } break;
            case KEY_A: {
                universe.fill(Alive);
            } break;
            case KEY_T: {
                prev_theme = theme;
                theme_cycle(&theme);
            } break;
            case KEY_I: {
                universe.invert();
            } break;
            case KEY_R: {
                universe.fill_random();
            } break;
            case KEY_B: {
                prev_theme = theme;
                theme_toggle_bolus(&theme);
            } break;
            case KEY_SPACE: {
                this->toggle_state();
            } break;
            case KEY_ENTER:
            case KEY_KP_ENTER: {
                if (state == GameState_Help)
                    state = GameState_Running;
            } break;
            case KEY_ESCAPE: return Selected_None;
            case KEY_F5: {
                global_show_fps = !global_show_fps;
            } break;
            case KEY_F11: {
                ToggleFullscreen();
            } break;
            default: {}
        }

        if (prev_theme != theme)
            theme_destructure(theme, &theme_style);

        switch (state) {
            case GameState_Running: {
                if (passed_time >= update_frame_cap) {
                    universe.update_cells();
                    passed_time = 0.0;
                    iterations += 1;
                }
            } break;
            case GameState_Paused: {
                if (mouse_in_grid) {
                    if (mouse_left_down) {
                        universe.set((size_t)mouse_pos.x, (size_t)mouse_pos.y, Alive);
                    }
                    else if (mouse_right_down) {
                        universe.set((size_t)mouse_pos.x, (size_t)mouse_pos.y, Dead);
                    }
                }
            }
            default: {}
        }

        BeginDrawing();
        ClearBackground(theme_style.bg_color);

        switch (theme) {
            case Theme_Midnight: {
                static Color midnight_fg_color = {
                    .r = 0,
                    .g = 0,
                    .b = 100,
                    .a = 255,
                };
                for (uint32_t y = 0; y < universe.height; y++) {
                    for (uint32_t x = 0; x < universe.width; x++) {
                        DrawRectangle(
                            x * GOL_SCALE,
                            y * GOL_SCALE,
                            GOL_SCALE,
                            GOL_SCALE,
                            universe.get(x, y)?
                                (Color){(unsigned char)x, (unsigned char)y, 100, 255} :
                                theme_style.bg_color
                        );
                        if (universe.get(x, y)) {
                            midnight_fg_color.r = x;
                            midnight_fg_color.g = y;
                            DrawTextD(
                                theme_style.fg_char,
                                x * GOL_SCALE,
                                y * GOL_SCALE,
                                (float)GOL_SCALE,
                                midnight_fg_color
                            );
                        } else {
                            DrawTextD(
                                theme_style.bg_char,
                                x * GOL_SCALE,
                                y * GOL_SCALE,
                                (float)GOL_SCALE,
                                theme_style.bg_char_color
                            );
                        }
                    }
                }
            } break;
            case Theme_Bolus: {
                for (uint32_t y = 0; y < universe.height; y++) {
                    for (uint32_t x = 0; x < universe.width; x++) {
                        if (universe.get(x, y)) {
                            DrawTextureEx(
                                bolus_texture,
                                (Vector2){(float)(x * GOL_SCALE), (float)(y * GOL_SCALE)},
                                0.0,
                                0.5,
                                WHITE
                            );
                        }
                    }
                }
            } break;
            default: {
                for (uint32_t y = 0; y < universe.height; y++) {
                    for (uint32_t x = 0; x < universe.width; x++) {
                        bool alive = universe.get(x, y);
                        DrawTextD(
                            alive? theme_style.fg_char : theme_style.bg_char,
                            x * GOL_SCALE,
                            y * GOL_SCALE,
                            (float)GOL_SCALE,
                            alive? theme_style.fg_color : theme_style.bg_char_color
                        );
                    }
                }
            }
        }

        if (passed_show_scroll_time < 0.6f && draw_update_time) {
            snprintf(buffer, sizeof buffer, "[UPDATE TIME = %.2fs]", update_frame_cap);
            DrawTextD(buffer, window_width / 2 - 170, window_height / 2 - 40, 40, theme_style.ac_color);
        } else {
            passed_show_scroll_time = 0.0f;
            draw_update_time = false;
        }

        GuiDrawRectangle(
                rect(0, global_screen_height - GOL_STATUS_BAR_HEIGHT, global_screen_width, GOL_STATUS_BAR_HEIGHT),
                1, theme_style.fg_color, theme_style.bg_color
        );

        #define ICON_SIZE 38
        #define ICON_PADDING 1

        const int icon_y = global_screen_height - ICON_SIZE - ICON_PADDING;
        int icon_padding_x = ICON_PADDING;

        if (GuiButton(rect(icon_padding_x, icon_y, ICON_SIZE, ICON_SIZE), "#129#")) {
            update_frame_cap = max(0.0f, update_frame_cap - GOL_DEFAULT_TIME_STEP);
            draw_update_time = true;
            passed_show_scroll_time = 0.0;
        }
        if (GuiButton(
            rect(icon_padding_x += ICON_SIZE + ICON_PADDING, icon_y, ICON_SIZE, ICON_SIZE),
            (state == GameState_Paused)? "#131#" : "#132#"
        )) this->toggle_state();
        if (GuiButton(
            rect(icon_padding_x += ICON_SIZE + ICON_PADDING, icon_y, ICON_SIZE, ICON_SIZE),
            "#134#"
        )) {
            update_frame_cap += GOL_DEFAULT_TIME_STEP;
            draw_update_time = true;
            passed_show_scroll_time = 0.0;
        }

        icon_padding_x = window_width - ICON_PADDING - ICON_SIZE;

        if (GuiButton(
            rect(icon_padding_x, icon_y, ICON_SIZE, ICON_SIZE), "#159#"
        )) return Selected_None;

        if (GuiButton(
            rect(
                icon_padding_x -= (ICON_SIZE - ICON_PADDING) * 2,
                icon_y, ICON_SIZE, ICON_SIZE
            ), "#29#"
        )) universe.fill(Alive);

        if (GuiButton(
            rect(icon_padding_x -= ICON_SIZE - ICON_PADDING, icon_y, ICON_SIZE, ICON_SIZE), "#143#"
        )) universe.fill(Dead);

        if (GuiButton(
            rect(icon_padding_x -= ICON_SIZE - ICON_PADDING, icon_y, ICON_SIZE, ICON_SIZE),
            "#194#"
        )) universe.fill_random();
        if (GuiButton(
            rect(icon_padding_x -= ICON_SIZE - ICON_PADDING, icon_y, ICON_SIZE, ICON_SIZE),
            "#26#"
        )) {
            prev_theme = theme;
            theme_cycle(&theme);
        }

        //snprintf(buffer, sizeof buffer, "%llu", iterations); // NOTE: this might be useful later
        //DrawTextD(buffer, icon_padding_x - 100, icon_y, 30.0, WHITE);

        if (state == GameState_Paused && mouse_in_grid) {
            DrawRectangleLines(
                floorf(mouse_pos.x) * GOL_SCALE,
                floorf(mouse_pos.y) * GOL_SCALE,
                GOL_SCALE,
                GOL_SCALE,
                theme_style.ac_color
            );
        }

            return Selected_GOL;
        }
};

#endif
