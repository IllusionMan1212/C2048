#pragma once

#include "core.h"
#include "ui.h"

typedef enum MoveDir {
    MOVE_DIR_UP,
    MOVE_DIR_DOWN,
    MOVE_DIR_LEFT,
    MOVE_DIR_RIGHT,
} MoveDir;

typedef struct Tile {
    u16 value;
    u16 new_value;

    f64 anim_x_offset_relative;
    f64 anim_y_offset_relative;
    u8 tiles_to_move;
} Tile;

typedef struct Game {
    Tile board[4][4];
    MoveDir last_move_dir;
    bool animating;
} Game;

void draw_board(void);
void game_loop(void);
