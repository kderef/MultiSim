#ifndef GOL_GAME_C_
#define GOL_GAME_C_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"

#include "universe.c"
#include "theme.c"
#include "../ui/font.c"

//! The general game code. Things such as rendering and state management
//! are done in here.

INCBIN(bolus, "../assets/bolus.png");

/* some aliases */
#define ThemeStyle GolThemeStyle
#define Theme GolTheme
#define Cell GolCell

#define GOL_SPEED_SLIDER_MAX 0.65f

typedef struct GameOfLife {
    Universe universe;
    GameState state;
    float update_frame_cap;
    int window_width;
    int window_height;
    Vector2 mouse_pos;
    Theme theme;
    Theme prev_theme;
    Texture2D bolus;

    float speed_slider_value;

    uint64_t iterations;
} GameOfLife;

void gol_state_toggle(GameOfLife* ptr) {
    if (ptr->state == GameState_Paused) {
        ptr->iterations = 0;
        ptr->state = GameState_Running;
    }
    else {
        ptr->state = GameState_Paused;
    }
}

GameOfLife* gol_alloc() {
    GameOfLife* gol = (GameOfLife*) calloc(1, sizeof(GameOfLife));

    if (!gol) {
        panic("Initializing game of life failed.");
    }

    gol->update_frame_cap = GOL_DEFAULT_UPDATE_CAP;
    gol->universe = universe_new(GOL_GRID_W, GOL_GRID_H);

    Image bolus_png = LoadImageFromMemory(".png", bolus_data, bolus_size);
    gol->bolus = LoadTextureFromImage(bolus_png);
    UnloadImage(bolus_png);

    gol->state = GameState_Paused;
    gol->theme = GOLTheme_Default;
    gol->prev_theme = -1;
    gol->speed_slider_value = GOL_SPEED_SLIDER_MAX - GOL_DEFAULT_UPDATE_CAP;

    return gol;
}

void gol_free(GameOfLife* ptr) {
    UnloadTexture(ptr->bolus);
    universe_deinit(&(ptr->universe));
    free(ptr);
}

bool gol_screen_size_changed(GameOfLife* gol) {
    bool changed = global_state.screen_w != gol->window_width || global_state.screen_w != gol->window_height;

    gol->window_width = global_state.screen_w;
    gol->window_height = global_state.screen_h;

    return changed;
}

static inline
SelectedGame gol_update(GameOfLife* gol) {
    static float dt;
    static bool size_changed;
    static int key;
    static ThemeStyle theme_style;
    // dt shit
    static bool mouse_left_down, mouse_right_down, show_help_window;
    static float passed_time;
    static bool mouse_in_grid;

    dt = GetFrameTime();
    passed_time = (passed_time >= gol->update_frame_cap) ? 0.0f : passed_time + dt;

    // handle window size
    size_changed = gol_screen_size_changed(gol);

    if (size_changed) {
        int new_w = gol->window_width / GOL_SCALE;
        int new_h = (gol->window_height - GOL_STATUS_BAR_HEIGHT) / GOL_SCALE;

        universe_resize(&(gol->universe), (size_t)new_w, (size_t)new_h);
    }

    // handle mouse position
    gol->mouse_pos = GetMousePosition();
    gol->mouse_pos.x = Clamp(floorf(gol->mouse_pos.x), 0.0, (float)(gol->window_width) - 1.0) / (float)GOL_SCALE;
    gol->mouse_pos.y = Clamp(floorf(gol->mouse_pos.y), 0.0, (float)(gol->window_height - GOL_STATUS_BAR_HEIGHT) - 1.0) / (float)GOL_SCALE;
    mouse_left_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    mouse_right_down = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    mouse_in_grid = gol->mouse_pos.y < (gol->window_height - GOL_STATUS_BAR_HEIGHT - 1.0) / GOL_SCALE;

    // handle the keys
    key = GetKeyPressed();

    if (IsKeyDown(KEY_MINUS) || IsKeyDown(KEY_KP_SUBTRACT) || global_state.mouse_wheel_move < 0.0f) {
        gol->update_frame_cap = Clamp(gol->update_frame_cap + GOL_DEFAULT_TIME_STEP, 0.0f, GOL_SPEED_SLIDER_MAX);
        gol->speed_slider_value = GOL_SPEED_SLIDER_MAX - gol->update_frame_cap;
    }
    if (IsKeyDown(KEY_EQUAL) || IsKeyDown(KEY_KP_EQUAL) || global_state.mouse_wheel_move > 0.0f) {
        gol->update_frame_cap = max(
            0.0, gol->update_frame_cap - GOL_DEFAULT_TIME_STEP
        );
        gol->speed_slider_value = GOL_SPEED_SLIDER_MAX - gol->update_frame_cap;
    }

    switch (key) {
    case 0: break;
    case KEY_C: {
        universe_fill(&(gol->universe), Dead);
    } break;
    case KEY_A: {
        universe_fill(&(gol->universe), Alive);
    } break;
    case KEY_T: {
        gol->prev_theme = gol->theme;
        theme_cycle(&(gol->theme));
    } break;
    case KEY_I: {
        universe_invert(&(gol->universe));
    } break;
    case KEY_R: {
        universe_fill_random(&(gol->universe));
    } break;
    case KEY_B: {
        gol->prev_theme = gol->theme;
        theme_toggle_bolus(&(gol->theme));
    } break;
    case KEY_SPACE: {
        gol_state_toggle(gol);
    } break;
    case KEY_ESCAPE: return Selected_None;
    case KEY_F5: {
        global_state.show_fps = !global_state.show_fps;
    } break;
    case KEY_F11: {
        ToggleFullscreen();
    } break;
    default: {}
    }

    if (gol->prev_theme != gol->theme) {
        theme_destructure(gol->theme, &theme_style);
        gol->prev_theme = gol->theme;
    }

    switch (gol->state) {
    case GameState_Running: {
        if (passed_time >= gol->update_frame_cap) {
            universe_update_cells(&(gol->universe));
            passed_time = 0.0;
            gol->iterations += 1;
        }
    } break;
    case GameState_Paused: {
        if ((!mouse_in_grid) || show_help_window) break;
        if (mouse_left_down) {
            universe_set(&(gol->universe), (size_t)(gol->mouse_pos.x), (size_t)(gol->mouse_pos.y), Alive);
        }
        else if (mouse_right_down) {
            universe_set(&(gol->universe), (size_t)(gol->mouse_pos.x), (size_t)(gol->mouse_pos.y), Dead);
        }
    }
    default: {}
    }

    BeginDrawing();
    ClearBackground(theme_style.bg_color);

    switch (gol->theme) {
    case GOLTheme_Midnight: {
        Color midnight_fg_color = color(0, 0, 100);
        for (uint32_t y = 0; y < gol->universe.height; y++) {
            for (uint32_t x = 0; x < gol->universe.width; x++) {
                DrawRectangle(
                    x * GOL_SCALE,
                    y * GOL_SCALE,
                    GOL_SCALE,
                    GOL_SCALE,
                    universe_get(&(gol->universe), x, y) ?
                    color(x, y, 100) :
                    theme_style.bg_color
                );
                if (universe_get(&(gol->universe), x, y)) {
                    midnight_fg_color.r = x;
                    midnight_fg_color.g = y;
                    DrawTextD(
                        theme_style.fg_char,
                        x * GOL_SCALE,
                        y * GOL_SCALE,
                        (float)GOL_SCALE,
                        midnight_fg_color
                    );
                }
                else {
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
    case GOLTheme_Bolus: {
        for (uint32_t y = 0; y < gol->universe.height; y++) {
            for (uint32_t x = 0; x < gol->universe.width; x++) {
                if (universe_get(&(gol->universe), x, y)) {
                    DrawTextureEx(
                        gol->bolus,
                        vec2(x * GOL_SCALE, y * GOL_SCALE),
                        0.0,
                        0.5,
                        WHITE
                    );
                }
            }
        }
    } break;
    default: {
        for (uint32_t y = 0; y < gol->universe.height; y++) {
            for (uint32_t x = 0; x < gol->universe.width; x++) {
                bool alive = universe_get(&(gol->universe), x, y);
                DrawTextD(
                    alive ? theme_style.fg_char : theme_style.bg_char,
                    x * GOL_SCALE,
                    y * GOL_SCALE,
                    (float)GOL_SCALE,
                    alive ? theme_style.fg_color : theme_style.bg_char_color
                );
            }
        }
    }
    }

    GuiDrawRectangle(
        rect(0, global_state.screen_h - GOL_STATUS_BAR_HEIGHT, global_state.screen_w, GOL_STATUS_BAR_HEIGHT),
        1, theme_style.fg_color, theme_style.bg_color
    );

#define ICON_SIZE 38
#define ICON_PADDING 1

    const int icon_y = global_state.screen_h - ICON_SIZE - ICON_PADDING;
    int icon_padding_x = ICON_PADDING;

    const int slider_width = ICON_SIZE * 3.5;

    // pause button
    if (GuiButton(
        rect(slider_width + 70, icon_y, ICON_SIZE, ICON_SIZE),
        (gol->state == GameState_Paused) ? "#131#" : "#132#"
    )) gol_state_toggle(gol);

    GuiSliderPro(
        rect(ICON_PADDING + 65, icon_y + (14.25 / 2) - ICON_PADDING, slider_width, ICON_SIZE * 0.75),
        "SPEED", "", &(gol->speed_slider_value), 0.0f, GOL_SPEED_SLIDER_MAX,
        8
    );

    gol->update_frame_cap = GOL_SPEED_SLIDER_MAX - gol->speed_slider_value;

    icon_padding_x = gol->window_width - ICON_PADDING - ICON_SIZE;

    if (GuiButton(
        rect(icon_padding_x, icon_y, ICON_SIZE, ICON_SIZE), "#159#"
    )) return Selected_None; // Leave

    if (GuiButton(
        rect(icon_padding_x -= ICON_SIZE, icon_y, ICON_SIZE, ICON_SIZE),
         "#193#"
    )) show_help_window = !show_help_window;


    if (GuiButton(
        rect(
            icon_padding_x -= (ICON_SIZE - ICON_PADDING) * 2,
            icon_y, ICON_SIZE, ICON_SIZE
        ), "#29#"
    )) universe_fill(&(gol->universe), Alive);

    if (GuiButton(
        rect(icon_padding_x -= ICON_SIZE - ICON_PADDING, icon_y, ICON_SIZE, ICON_SIZE), "#143#"
    )) universe_fill(&(gol->universe), Dead);

    if (GuiButton(
        rect(icon_padding_x -= ICON_SIZE - ICON_PADDING, icon_y, ICON_SIZE, ICON_SIZE),
        "#194#"
    )) universe_fill_random(&(gol->universe));
    if (GuiButton(
        rect(icon_padding_x -= ICON_SIZE - ICON_PADDING, icon_y, ICON_SIZE, ICON_SIZE),
        "#26#"
    )) theme_cycle(&(gol->theme));

    //snprintf(buffer, sizeof buffer, "%llu", gol->iterations); // NOTE: this might be useful later
    //DrawTextD(buffer, icon_padding_x - 100, icon_y, 30.0, WHITE);

    // draw the outline around the mouse selection
    if (gol->state == GameState_Paused && mouse_in_grid) {
        DrawRectangleLines(
            floorf(gol->mouse_pos.x) * GOL_SCALE,
            floorf(gol->mouse_pos.y) * GOL_SCALE,
            GOL_SCALE,
            GOL_SCALE,
            theme_style.ac_color
        );
    }

    if (show_help_window) {
        static Rectangle bounds_win = rect(50, 50, 0, 0);
        bounds_win.width = global_state.screen_w - 100;
        bounds_win.height = global_state.screen_h - 100 - GOL_STATUS_BAR_HEIGHT;
        if (GuiWindowBox(
            bounds_win, "Help"
        )) show_help_window = false;

        const int padx = 50;
        const int pady = ICON_SIZE + 10;

        Rectangle bounds;
        // draw help info
        bounds = rect(bounds_win.x + padx, bounds_win.y + pady, ICON_SIZE, ICON_SIZE);
        GuiButton(bounds, "#26#");
        bounds.x += padx;
        bounds.width *= 20;
        //bounds.x += ICON_SIZE*1.5;
        GuiLabel(bounds, TextFormat("Cycle the theme (current: %s)", theme_style.name));

        bounds = rect(bounds_win.x + padx, bounds_win.y + pady*2, ICON_SIZE, ICON_SIZE);

        GuiButton(bounds, "#194#");
        bounds.x += padx;
        bounds.width *= 10;
        GuiLabel(bounds, "Fill the grid with random cells");
    
        bounds = rect(bounds_win.x + padx, bounds_win.y + pady*3, ICON_SIZE, ICON_SIZE);
        
        GuiButton(bounds, "#143#");
        bounds.x += padx;
        bounds.width *= 10;
        GuiLabel(bounds, "Clear the grid by deleting all cells");

        bounds = rect(bounds_win.x + padx, bounds_win.y + pady*4, ICON_SIZE, ICON_SIZE);
        
        GuiButton(bounds, "#29#");
        bounds.x += padx;
        bounds.width *= 10;
        GuiLabel(bounds, "Fill the grid with all live cells");
    }

    return Selected_GOL;
}

#undef Cell
#undef Theme
#undef ThemeStyle
#endif