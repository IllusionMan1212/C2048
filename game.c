#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "timer.h"
#include "zephr.h"

#define ANIM_SPEED 2.0f

// TODO: configurable color palette
// TODO: particles when merging tiles

Game game = {0};

const u8 neighbors[4][2] = {
  {-1, 0}, // left
  {1, 0},  // right 
  {0, -1}, // up
  {0, 1},  // down
};

///////////////////////////////////
//
//
// Utils
//
//
///////////////////////////////////

void print_board() {
  printf("Board:\n");
  for (int i = 0; i < 4; i++) {
    printf("  ");
    for (int j = 0; j < 4; j++) {
      printf("%d ", game.board[i][j].value);
    }
    printf("\n");
  }
}

void print_merge_status() {
  printf("Merge Board:\n");
  for (int i = 0; i < 4; i++) {
    printf("  ");
    for (int j = 0; j < 4; j++) {
      printf("%d ", game.board[i][j].merged);
    }
    printf("\n");
  }
}

void print_new_board() {
  printf("New Board:\n");
  for (int i = 0; i < 4; i++) {
    printf("  ");
    for (int j = 0; j < 4; j++) {
      printf("%d ", game.board[i][j].new_value);
    }
    printf("\n");
  }
}

Color get_color_from_tile(Tile tile) {
  switch (tile.value) {
    case 2:
      return ColorRGBA(238, 228, 218, 255);
    case 4:
      return ColorRGBA(237, 224, 200, 255);
    case 8:
      return ColorRGBA(242, 177, 121, 255);
    case 16:
      return ColorRGBA(245, 149, 99, 255);
    case 32:
      return ColorRGBA(246, 124, 95, 255);
    case 64:
      return ColorRGBA(246, 94, 59, 255);
    case 128:
      return ColorRGBA(237, 207, 114, 255);
    case 256:
      return ColorRGBA(237, 204, 97, 255);
    case 512:
      return ColorRGBA(237, 200, 80, 255);
    case 1024:
      return ColorRGBA(237, 197, 63, 255);
    case 2048:
      return ColorRGBA(237, 194, 46, 255);
    default:
      return ColorRGBA(205, 193, 180, 255);
  }
}

u8 get_tile_font_size(Tile tile) {
  switch (tile.value) {
    case 2:
    case 4:
    case 8:
      return 60;
    case 16:
    case 32:
    case 64:
      return 55;
    case 128:
    case 256:
    case 512:
      return 50;
    case 1024:
    case 2048:
      return 45;
    default:
      return 35;
  }
}

///////////////////////////////////
//
//
// Drawing
//
//
///////////////////////////////////


void draw_board() {
  Size window_size = zephr_get_window_size();
  UIConstraints board_con = {0};

  set_width_constraint(&board_con, window_size.width, UI_CONSTRAINT_FIXED);
  set_height_constraint(&board_con, window_size.height, UI_CONSTRAINT_FIXED);

  // bg
  draw_quad(&board_con, ColorRGBA(248, 250, 230, 255), 0, ALIGN_TOP_LEFT);

  // text
  UIConstraints text_con = {0};

  set_y_constraint(&text_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&text_con, 1.5, UI_CONSTRAINT_RELATIVE_PIXELS);

  draw_text("2048", 100, text_con, ColorRGBA(34, 234, 82, 155), ALIGN_TOP_CENTER);

  // board
  set_y_constraint(&board_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&board_con, 0.6f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&board_con, 1, UI_CONSTRAINT_ASPECT_RATIO);
  const float board_border_radius = board_con.width * 0.02f;
  draw_quad(&board_con, ColorRGBA(140, 154, 147, 255), board_border_radius, ALIGN_CENTER);

  // empty tiles
  const float tile_padding = board_con.width * 0.02f;
  const float tile_height = board_con.height * 0.225f;
  const float tile_board_radius = tile_height * 0.15f;

  UIConstraints empty_tile_con = {0};
  set_parent_constraint(&empty_tile_con, &board_con);
  set_height_constraint(&empty_tile_con, tile_height, UI_CONSTRAINT_FIXED);
  set_width_constraint(&empty_tile_con, 1, UI_CONSTRAINT_ASPECT_RATIO);

  for (int i = 0; i < 4; i++) {
    set_y_constraint(&empty_tile_con, (tile_height + tile_padding) * i + tile_padding, UI_CONSTRAINT_FIXED);
    for (int j = 0; j < 4; j++) {
      set_x_constraint(&empty_tile_con, (tile_height + tile_padding) * j + tile_padding, UI_CONSTRAINT_FIXED);
      draw_quad(&empty_tile_con, ColorRGBA(120, 133, 126, 255), tile_board_radius, ALIGN_TOP_LEFT);
    }
  }

  // filled tiles
  UIConstraints tile_con = {0};
  set_parent_constraint(&tile_con, &board_con);
  set_height_constraint(&tile_con, tile_height, UI_CONSTRAINT_FIXED);
  set_width_constraint(&tile_con, 1, UI_CONSTRAINT_ASPECT_RATIO);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      Tile *tile = &game.board[i][j];
      Color tile_color = get_color_from_tile(*tile);
      char value[16];
      sprintf(value, "%d", tile->value);

      set_y_constraint(&tile_con, (f32)((tile_height + tile_padding) * i + tile_padding + game.board[i][j].anim_y_offset_relative * board_con.height), UI_CONSTRAINT_FIXED);
      set_x_constraint(&tile_con, (f32)((tile_height + tile_padding) * j + tile_padding + game.board[i][j].anim_x_offset_relative * board_con.height), UI_CONSTRAINT_FIXED);
      if (tile->value > 0) {
        draw_quad(&tile_con, tile_color, tile_board_radius, ALIGN_TOP_LEFT);

        // value text
        set_parent_constraint(&text_con, &tile_con);
        set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
        set_y_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);

        draw_text(value, get_tile_font_size(*tile), text_con, mul_color(tile_color, 0.5), ALIGN_CENTER);
      }
    }
  }

  // score
  char score[16];
  sprintf(score, "%d", game.score);

  set_parent_constraint(&text_con, NULL);
  set_x_constraint(&text_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&text_con, 0.8f, UI_CONSTRAINT_RELATIVE);
  draw_text("Score", 40, text_con, COLOR_BLACK, ALIGN_TOP_LEFT);

  set_x_constraint(&text_con, 0.15f, UI_CONSTRAINT_RELATIVE);
  draw_text(score, 40, text_con, COLOR_BLACK, ALIGN_TOP_LEFT);
}

///////////////////////////////////
//
//
// Game logic
//
//
///////////////////////////////////

void move_right() {
  if (game.animating) return;
  game.last_move_dir = MOVE_DIR_RIGHT;

  for (int i = 0; i < 4; i++) {
    for (int j = 2; j >= 0; j--) {
      if (game.board[i][j].value == 0) {
        continue;
      }
      u8 tiles_to_move = 0;
      u8 x = j;

      Tile *src = &game.board[i][j];

      do {
        x++;

        // next tile is empty so we can move there
        if (game.board[i][x].new_value == 0) {
          tiles_to_move++;
          continue;
        }

        // next tile has the same value so we can merge and move
        if (game.board[i][x].new_value == src->new_value && !game.board[i][x].merged) {
          tiles_to_move++;
          game.board[i][x].merged = true;
          break;
        } else {
          // next tile has a different value so we can't move there
          x--;
          break;
        }
      } while (x < 3);

      src->tiles_to_move = tiles_to_move;

      if (tiles_to_move > 0) {
        if (game.board[i][x].merged) {
          game.board[i][x].new_value *= 2;
          game.score += game.board[i][x].new_value;
        } else {
          game.board[i][x].new_value = src->new_value;
        }
        src->new_value = 0;
        game.animating = true;
      }
    }
  }
}

void move_left() {
  if (game.animating) return;
  game.last_move_dir = MOVE_DIR_LEFT;

  for (int i = 0; i < 4; i++) {
    for (int j = 1; j < 4; j++) {
      if (game.board[i][j].value == 0) {
        continue;
      }
      u8 tiles_to_move = 0;
      u8 x = j;

      Tile *src = &game.board[i][j];

      do {
        x--;

        // next tile is empty so we can move there
        if (game.board[i][x].new_value == 0) {
          tiles_to_move++;
          continue;
        }

        // next tile has the same value so we can merge and move
        if (game.board[i][x].new_value == src->new_value && !game.board[i][x].merged) {
          tiles_to_move++;
          game.board[i][x].merged = true;
          break;
        } else {
          // next tile has a different value so we can't move there
          x++;
          break;
        }
      } while (x > 0);

      src->tiles_to_move = tiles_to_move;

      if (tiles_to_move > 0) {
        if (game.board[i][x].merged) {
          game.board[i][x].new_value *= 2;
          game.score += game.board[i][x].new_value;
        } else {
          game.board[i][x].new_value = src->new_value;
        }
        src->new_value = 0;
        game.animating = true;
      }
    }
  }

}

void move_down() {
  if (game.animating) return;
  game.last_move_dir = MOVE_DIR_DOWN;

  for (int i = 2; i >= 0; i--) {
    for (int j = 0; j < 4; j++) {
      if (game.board[i][j].value == 0) {
        continue;
      }
      u8 tiles_to_move = 0;
      u8 y = i;

      Tile *src = &game.board[i][j];

      do {
        y++;

        if (game.board[y][j].new_value == 0) {
          tiles_to_move++;
          continue;
        }

        if (game.board[y][j].new_value == src->new_value && !game.board[y][j].merged) {
          tiles_to_move++;
          game.board[y][j].merged = true;
          break;
        } else {
          y--;
          break;
        }
      } while (y < 3);

      src->tiles_to_move = tiles_to_move;

      if (tiles_to_move > 0) {
        if (game.board[y][j].merged) {
          game.board[y][j].new_value *= 2;
          game.score += game.board[y][j].new_value;
        } else {
          game.board[y][j].new_value = src->new_value;
        }
        src->new_value = 0;
        game.animating = true;
      }
    }
  }
}

void move_up() {
  if (game.animating) return;
  game.last_move_dir = MOVE_DIR_UP;

  for (int i = 1; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (game.board[i][j].new_value == 0) {
        continue;
      }
      u8 tiles_to_move = 0;
      u8 y = i;

      Tile *src = &game.board[i][j];

      do {
        y--;

        if (game.board[y][j].new_value == 0) {
          tiles_to_move++;
          continue;
        }

        if (game.board[y][j].new_value == src->new_value && !game.board[y][j].merged) {
          tiles_to_move++;
          game.board[y][j].merged = true;
          break;
        } else {
          y++;
          break;
        }
      } while (y > 0);

      src->tiles_to_move = tiles_to_move;

      if (tiles_to_move > 0) {
        if (game.board[y][j].merged) {
          game.board[y][j].new_value *= 2;
          game.score += game.board[y][j].new_value;
        } else {
          game.board[y][j].new_value = src->new_value;
        }
        src->new_value = 0;
        game.animating = true;
      }
    }
  }
}

void spawn_new_tile(u8 x, u8 y) {
  // 90% chance of spawning a 2, 10% chance of spawning a 4
  if (rand() % 10 < 9) {
    game.board[x][y].value = 2;
    game.board[x][y].new_value = 2;
  } else {
    game.board[x][y].value = 4;
    game.board[x][y].new_value = 4;
  }
}

void spawn_random_tile() {
  u8 available_tiles_count = 0;
  u8 available_tiles[16];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j <= 3; j++) {
      if (game.board[i][j].new_value == 0 && !game.board[i][j].merged) {
        available_tiles[available_tiles_count] = i * 4 + j;
        available_tiles_count++;
      }
    }
  }

  u8 rand_idx = rand() % available_tiles_count;

  u8 x = available_tiles[rand_idx] / 4;
  u8 y = available_tiles[rand_idx] % 4;

  spawn_new_tile(x, y);
}

void update_positions(f64 delta_t) {
  if (game.animating) {
    switch (game.last_move_dir) {
      case MOVE_DIR_LEFT:
        for (int i = 0; i < 4; i++) {
          for (int j = 1; j < 4; j++) {
            u8 tiles_to_move = game.board[i][j].tiles_to_move;
            f32 speed = ANIM_SPEED * tiles_to_move;
            Tile *src = &game.board[i][j];
            Tile *dest = &game.board[i][j - tiles_to_move];

            if (src->tiles_to_move != 0) {
              src->anim_x_offset_relative -= speed * delta_t;

              if (src->anim_x_offset_relative <= -0.240f * (tiles_to_move + (tiles_to_move * 0.02f))) {
                src->anim_x_offset_relative = -0.240f * (tiles_to_move + (tiles_to_move * 0.02f));

                if (dest->new_value == src->value * 2) {
                  dest->merged = false;
                }

                src->value = src->new_value;
                dest->value = dest->new_value;

                src->tiles_to_move = 0;
                src->anim_x_offset_relative = 0;
                game.animating = false;
                game.spawning_new_tile = true;
              }
            }
          }
        }
        break;
      case MOVE_DIR_RIGHT:
        for (int i = 0; i < 4; i++) {
          for (int j = 2; j >= 0; j--) {
            u8 tiles_to_move = game.board[i][j].tiles_to_move;
            f32 speed = ANIM_SPEED * tiles_to_move;
            Tile *src = &game.board[i][j];
            Tile *dest = &game.board[i][j + tiles_to_move];

            if (src->tiles_to_move != 0) {
              src->anim_x_offset_relative += speed * delta_t;

              if (src->anim_x_offset_relative >= 0.240f * (tiles_to_move + (tiles_to_move * 0.02f))) {
                src->anim_x_offset_relative = 0.240f * (tiles_to_move + (tiles_to_move * 0.02f));

                if (dest->new_value == src->value * 2) {
                  dest->merged = false;
                }

                src->value = src->new_value;
                dest->value = dest->new_value;

                src->tiles_to_move = 0;
                src->anim_x_offset_relative = 0;
                game.animating = false;
                game.spawning_new_tile = true;
              }
            }
          }
        }
        break;
      case MOVE_DIR_UP:
        for (int i = 1; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
            u8 tiles_to_move = game.board[i][j].tiles_to_move;
            f32 speed = ANIM_SPEED * tiles_to_move;
            Tile *src = &game.board[i][j];
            Tile *dest = &game.board[i - tiles_to_move][j];

            if (src->tiles_to_move != 0) {
              src->anim_y_offset_relative -= speed * delta_t;

              if (src->anim_y_offset_relative <= -0.240f * (tiles_to_move + (tiles_to_move * 0.02f))) {
                src->anim_y_offset_relative = -0.240f * (tiles_to_move + (tiles_to_move * 0.02f));

                if (dest->new_value == src->value * 2) {
                  dest->merged = false;
                }

                src->value = src->new_value;
                dest->value = dest->new_value;

                src->tiles_to_move = 0;
                src->anim_y_offset_relative = 0;
                game.animating = false;
                game.spawning_new_tile = true;
              }
            }
          }
        }
        break;
      case MOVE_DIR_DOWN:
        for (int i = 2; i >= 0; i--) {
          for (int j = 0; j < 4; j++) {
            u8 tiles_to_move = game.board[i][j].tiles_to_move;
            f32 speed = ANIM_SPEED * tiles_to_move;
            Tile *src = &game.board[i][j];
            Tile *dest = &game.board[i + tiles_to_move][j];

            if (src->tiles_to_move != 0) {
              src->anim_y_offset_relative += speed * delta_t;

              if (src->anim_y_offset_relative >= 0.240f * (tiles_to_move + (tiles_to_move * 0.02f))) {
                src->anim_y_offset_relative = 0.240f * (tiles_to_move + (tiles_to_move * 0.02f));

                if (dest->new_value == src->value * 2) {
                  dest->merged = false;
                }

                src->value = src->new_value;
                dest->value = dest->new_value;

                src->tiles_to_move = 0;
                src->anim_y_offset_relative = 0;
                game.animating = false;
                game.spawning_new_tile = true;
              }
            }
          }
        }
        break;
    }
  }

  if (game.spawning_new_tile) {
    spawn_random_tile();
    game.spawning_new_tile = false;
  }
}

bool gameover() {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (game.board[y][x].value == 0) {
        return false;
      }

      for (int i = 0; i < 4; i++) {
        u8 new_x = CORE_CLAMP(x + neighbors[i][0], 0, 3);
        u8 new_y = CORE_CLAMP(y + neighbors[i][1], 0, 3);

        if ((new_y != y || new_x != x) && game.board[y][x].value == game.board[new_y][new_x].value) {
          return false;
        }
      }
    }
  }

  return true;
}

void game_init(void) {
  srand(time(NULL));

  for (int i = 0; i < 2; i++) {
    spawn_random_tile();
  }
}

///////////////////////////////////
//
//
// Loop and input handling
//
//
///////////////////////////////////


void handle_keyboard_input(ZephrEvent e) {
  if (
      (e.key.mods & ZEPHR_KEY_MOD_CTRL && e.key.code == ZEPHR_KEYCODE_Q) ||
      e.key.code == ZEPHR_KEYCODE_ESCAPE
     ) {
    // TODO: show confirmation dialog
    zephr_quit();
  } else if (e.key.code == ZEPHR_KEYCODE_F11) {
    zephr_toggle_fullscreen();
  } else if (e.key.code == ZEPHR_KEYCODE_UP) {
    move_up();
  } else if (e.key.code == ZEPHR_KEYCODE_DOWN) {
    move_down();
  } else if (e.key.code == ZEPHR_KEYCODE_LEFT) {
    move_left();
  } else if (e.key.code == ZEPHR_KEYCODE_RIGHT) {
    move_right();
  }
}

void game_loop(void) {
  game_init();

  f64 last_frame = 0.0;

  while (!zephr_should_quit()) {
    ZephrEvent e;

    while (zephr_iter_events(&e)) {
      switch (e.type) {
        case ZEPHR_EVENT_WINDOW_CLOSED:
          zephr_quit();
          break;
        case ZEPHR_EVENT_KEY_PRESSED:
          handle_keyboard_input(e);
          break;
        default:
          break;
      }
    }

    f64 now = get_time();
    f64 delta_t = now - last_frame;
    last_frame = now;

    update_positions(delta_t);
    draw_board();

    zephr_swap_buffers();
  }
}