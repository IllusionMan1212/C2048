#pragma once

#include "core.h"
#include "ui.h"

typedef enum MoveDir {
    MOVE_DIR_UP,
    MOVE_DIR_DOWN,
    MOVE_DIR_LEFT,
    MOVE_DIR_RIGHT,
} MoveDir;

typedef enum IconTexture {
    HELP_ICON,
    SETTINGS_ICON,
    CLOSE_ICON,

    ICON_TEXTURE_COUNT,
} IconTexture;

typedef struct Tile {
    u16 value;
    u16 new_value;
    bool merged;

    f64 anim_x_offset_relative;
    f64 anim_y_offset_relative;
    u8 tiles_to_move;
} Tile;

typedef struct Game {
    Tile board[4][4];
    u32 score;
    MoveDir last_move_dir;
    bool animating;
    bool spawning_new_tile;
    bool has_lost;
    float game_over_bg_opacity;
    float game_over_opacity;
    float spawning_tile_scale;
    Vec2 spawning_tile_coords;
    u16 spawning_tile_value;

    TextureId icon_textures[ICON_TEXTURE_COUNT];
    bool quit_dialog;
    bool help_dialog;
} Game;

void draw_board(void);
void game_loop(void);
