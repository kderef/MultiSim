#ifndef GOL_GAME_C_
#define GOL_GAME_C_

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <math.h>

#include "universe.c"
#include "theme.c"

/// The state of the Game of Life app.
typedef enum {
    GOL_DesignMode,
    GOL_SimulationMode,
    GOL_HelpMode,
} GolGameState;

void state_toggle(GolGameState* p_state) {
    *p_state = (*p_state == GOL_DesignMode) ?
        GOL_SimulationMode : GOL_DesignMode;
}

typedef struct {
    Universe universe;
    GolGameState state;
    // dt
    float update_frame_cap;
    float passed_time;
    float passed_resize_time;
    bool resize_queued;
    Vector2 mouse_pos;
    unsigned window_width;
    unsigned window_height;
    Theme theme;
    Image bolus_png;
    Texture2D bolus;
} GameOfLife;

GameOfLife game_of_life_new() {
    GameOfLife gol = {0};
    gol.update_frame_cap = GOL_DEFAULT_UPDATE_CAP;
    gol.universe = universe_new(GOL_GRID_W, GOL_GRID_H);
    gol.bolus_png = LoadImageFromMemory(".png", bolus_data, bolus_size);
    gol.bolus = LoadTextureFromImage(gol.bolus_png);
    gol.state = GOL_HelpMode;

    return gol;
}

void game_of_life_deinit(GameOfLife* ptr) {
    UnloadImage(ptr->bolus_png);
    UnloadTexture(ptr->bolus);

}

bool game_of_life_screen_size_changed(GameOfLife* gol) {
    int new_width = GetScreenWidth();
    int new_height = GetScreenHeight();

    bool changed = new_width != gol->window_width || new_height != gol->window_height;

    gol->window_width = (unsigned)new_width;
    gol->window_height = (unsigned)new_height;

    return changed;
}

SelectedGame game_of_life_update(GameOfLife* gol) {
    static float dt;
    static bool size_changed;
    static int key;
    static Color bg, fg, ac;

    if (gol->passed_time >= gol->update_frame_cap)
        gol->passed_time = 0.0;

    dt = GetFrameTime();
    gol->passed_time += dt;
    gol->passed_resize_time += dt;

    // handle window size
    size_changed = game_of_life_screen_size_changed(gol);

    if (size_changed || gol->resize_queued) {
        size_t new_w = gol->window_width / GOL_SCALE;
        size_t new_h = gol->window_height / GOL_SCALE;

        universe_resize(&(gol->universe), new_w, new_h);
        gol->resize_queued = false;
        gol->passed_resize_time = 0.0;
    } else {
        gol->resize_queued = true;
    }

    // handle mouse position
    gol->mouse_pos = GetMousePosition();
    gol->mouse_pos.x = Clamp(floorf(gol->mouse_pos.x), 0.0, (float)(gol->window_width) - 1.0) / (float)GOL_SCALE;
    gol->mouse_pos.y = Clamp(floorf(gol->mouse_pos.y), 0.0, (float)(gol->window_height) - 1.0) / (float)GOL_SCALE;

    // handle the keys
    key = GetKeyPressed();
    switch (key) {
        case 0: break;
        case KEY_H: {
            if (gol->state == GOL_HelpMode)
                gol->state = GOL_DesignMode;
            else
                gol->state = GOL_HelpMode;
        } break;
        case KEY_C: {
            universe_fill(&(gol->universe), Dead);
        } break;
        case KEY_A: {
            universe_fill(&(gol->universe), Alive);
        } break;
        case KEY_T: {
            theme_cycle(&(gol->theme));
        } break;
        case KEY_I: {
            universe_invert(&(gol->universe));
        } break;
        case KEY_R: {
            universe_fill_random(&(gol->universe));
        } break;
        case KEY_B: {
            theme_toggle_bolus(&(gol->theme));
        } break;
        case KEY_EQUAL:
        case KEY_KP_EQUAL: {
            gol->update_frame_cap += GOL_DEFAULT_TIME_STEP;
        } break;
        case KEY_MINUS:
        case KEY_KP_SUBTRACT: {
            gol->update_frame_cap = max(
                0.0, gol->update_frame_cap - GOL_DEFAULT_TIME_STEP
            );
        } break;
        case KEY_SPACE: {
            state_toggle(&(gol->state));
        } break;
        case KEY_ENTER:
        case KEY_KP_ENTER: {
            if (gol->state == GOL_HelpMode)
                gol->state = GOL_DesignMode;
        } break;
        case KEY_ESCAPE: return Selected_None;
        case KEY_F11: {
            ToggleFullscreen();
        } break;
        default:
    }

    theme_destructure(gol->theme, &bg, &fg, &ac);

    switch (gol->state) {
        case GOL_SimulationMode: {
            if (gol->passed_time >= gol->update_frame_cap) {
                universe_update_cells(&(gol->universe));
                gol->passed_time = 0.0;
            }
        } break;
        case GOL_DesignMode: {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                universe_set(&(gol->universe), (size_t)(gol->mouse_pos.x), (size_t)(gol->mouse_pos.y), Alive);
            }
            else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                universe_set(&(gol->universe), (size_t)(gol->mouse_pos.x), (size_t)(gol->mouse_pos.y), Dead);
            }
        }
        default:
    }

    BeginDrawing();
    ClearBackground(bg);

    if (gol->state == GOL_HelpMode) {
        static const int FONT_S = 19;
        static const int FONT_M = 30;
        static const int FONT_L = 35;
        static const int FONT_XL = 60;

        static char buffer[512];

        float spacing = 0;

        DrawTextD("CONTROLS", 2, spacing, FONT_L, ac);
        spacing += FONT_L;

        // helper macros
        #define ADD_CONTROL(S) \
            DrawTextD((S), 3, spacing, FONT_M, fg); \
            spacing += FONT_M

        #define ADD_FMT_CONTROL(...) \
            snprintf(buffer, sizeof buffer, __VA_ARGS__); \
            DrawTextD(buffer, 3, spacing, FONT_M, fg); \
            spacing += FONT_M

        ADD_CONTROL("Left mouse  - make cell alive");
        ADD_CONTROL("Right mouse - make cell dead");
        ADD_CONTROL("Space        - pause/unpause game");
        ADD_CONTROL("H              - help menu");
        ADD_CONTROL("C              - clear the board");
        ADD_CONTROL("A              - fill the board with live cells");
        DrawTextD("I              - invert the cells", 8, spacing, FONT_M, fg);
        spacing += FONT_M;
        ADD_CONTROL("R              - generate a random pattern");
        ADD_FMT_CONTROL("T              - switch theme (currently: %s)", theme_to_str(gol->theme));
        ADD_FMT_CONTROL("+              - add " STRINGIFY(GOL_DEFAULT_TIME_STEP) " to update time (%.2f)", gol->update_frame_cap);
        ADD_FMT_CONTROL("-              - subtract " STRINGIFY(GOL_DEFAULT_TIME_STEP) " to update time (%.2f)", gol->update_frame_cap);
        spacing += FONT_XL;

        DrawTextD("Made by Kian (Kn-Ht)", gol->window_width - 182, gol->window_height - 20, 20.0, GRAY);
        DrawTextD(
            "To exit to the main menu press Escape",
            4, spacing, FONT_L, ac
        );
        spacing += FONT_L;
        DrawTextD(
            "Press Enter or H to start Drawing!",
            4, spacing, FONT_L, ac
        );

        EndDrawing();
        return Selected_GOL;
    }

    switch (gol->theme) {
        case Theme_Midnight: {
            for (size_t y = 0; y < gol->universe.height; y++) {
                for (size_t x = 0; x < gol->universe.width; x++) {
                    DrawRectangle(
                        x * GOL_SCALE,
                        y * GOL_SCALE,
                        GOL_SCALE,
                        GOL_SCALE,
                        universe_get(&(gol->universe), x, y)?
                            (Color){x, y, 100, 255} :
                            bg
                    );
                }
            }
        } break;
        case Theme_Bolus: {
            for (size_t y = 0; y < gol->universe.height; y++) {
                for (size_t x = 0; x < gol->universe.width; x++) {
                    if (universe_get(&(gol->universe), x, y)) {
                        DrawTextureEx(
                            gol->bolus,
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
            for (size_t y = 0; y < gol->universe.height; y++) {
                for (size_t x = 0; x < gol->universe.width; x++) {
                    DrawRectangle(
                        x * GOL_SCALE,
                        y * GOL_SCALE,
                        GOL_SCALE,
                        GOL_SCALE,
                        universe_get(&(gol->universe), x, y)?
                            fg : bg
                    );
                }
            }
        }
    }

    if (gol->state == GOL_DesignMode) {
        DrawRectangleLines(
            floorf(gol->mouse_pos.x) * GOL_SCALE,
            floorf(gol->mouse_pos.y) * GOL_SCALE,
            GOL_SCALE,
            GOL_SCALE,
            ac
        );

        DrawTextD(
            "[DESIGN MODE]",
            3,
            gol->window_height - 25 ,
            25,
            ac
        );
    }

    EndDrawing();

    return Selected_GOL;
}

#endif