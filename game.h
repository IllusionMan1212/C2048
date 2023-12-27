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
    MoveDir last_move_dir;
    bool animating;
    bool spawning_new_tile;
    bool game_over;
    u32 score;

    TextureId icon_textures[ICON_TEXTURE_COUNT];
} Game;

void draw_board(void);
void game_loop(void);
