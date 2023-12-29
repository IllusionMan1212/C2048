#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "timer.h"
#include "zephr.h"

#define TILE_ANIM_SPEED 2.5f
#define GAMEOVER_ANIM_SPEED 150.f

Game game = {0};

const Vec2 neighbors[4] = {
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
      return ColorRGBA(237, 194, 46, 255);
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
      return 40;
  }
}

///////////////////////////////////
//
//
// Game logic
//
//
///////////////////////////////////

void move_right(void) {
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

void move_left(void) {
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

void move_down(void) {
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

void move_up(void) {
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

void spawn_random_tile(void) {
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

bool gameover(void) {
  if (game.has_lost) return false;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (game.board[y][x].value == 0) {
        return false;
      }

      for (int i = 0; i < 4; i++) {
        u8 new_y = CORE_CLAMP(y + neighbors[i].y, 0, 3);
        u8 new_x = CORE_CLAMP(x + neighbors[i].x, 0, 3);

        if ((new_y != y || new_x != x) && game.board[y][x].value == game.board[new_y][new_x].value) {
          return false;
        }
      }
    }
  }

  return true;
}

void game_new(void) {
  game.score = 0;
  game.animating = false;
  game.spawning_new_tile = false;
  game.quit_dialog = false;
  game.help_dialog = false;
  game.has_lost = false;
  game.game_over_bg_opacity = 0;
  game.game_over_opacity = 0;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      game.board[y][x].value = 0;
      game.board[y][x].new_value = 0;
      game.board[y][x].merged = false;
      game.board[y][x].tiles_to_move = 0;
      game.board[y][x].anim_x_offset_relative = 0;
      game.board[y][x].anim_y_offset_relative = 0;
    }
  }

  for (int i = 0; i < 2; i++) {
    spawn_random_tile();
  }
}

void game_attempt_quit(void) {
  game.help_dialog = false;
  game.quit_dialog = true;
}

void game_init(void) {
  srand(time(NULL));

  game.icon_textures[HELP_ICON] = load_texture("assets/icons/help.png");
  game.icon_textures[SETTINGS_ICON] = load_texture("assets/icons/settings.png");
  game.icon_textures[CLOSE_ICON] = load_texture("assets/icons/close.png");

  for (int i = 0; i < 2; i++) {
    spawn_random_tile();
  }
}

///////////////////////////////////
//
//
// Drawing
//
//
///////////////////////////////////


void draw_bg(void) {
  Size window_size = zephr_get_window_size();
  UIConstraints board_con = {0};

  set_width_constraint(&board_con, window_size.width, UI_CONSTRAINT_FIXED);
  set_height_constraint(&board_con, window_size.height, UI_CONSTRAINT_FIXED);

  // bg
  draw_quad(&board_con, ColorRGBA(248, 250, 230, 255), 0, ALIGN_TOP_LEFT);

  // 2048 text
  UIConstraints text_con = {0};

  set_y_constraint(&text_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&text_con, 1.5, UI_CONSTRAINT_RELATIVE_PIXELS);

  draw_text("2048", 100, text_con, ColorRGBA(34, 234, 82, 155), ALIGN_TOP_CENTER);

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

void draw_quit_dialog(void) {
  UIConstraints dialog_con = {0};
  set_x_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_width_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  draw_quad(&dialog_con, ColorRGBA(0, 0, 0, 200), 0, ALIGN_CENTER);

  set_width_constraint(&dialog_con, 0.4f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 0.2f, UI_CONSTRAINT_RELATIVE);
  draw_quad(&dialog_con, ColorRGBA(135, 124, 124, 255), 8, ALIGN_CENTER);

  UIConstraints text_con = {0};
  set_parent_constraint(&text_con, &dialog_con);
  set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&text_con, -0.04f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&text_con, 1, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("Are you sure you want to quit?", 36.f, text_con, COLOR_WHITE, ALIGN_CENTER);

  UIConstraints btn_con = {0};
  set_parent_constraint(&btn_con, &dialog_con);
  set_width_constraint(&btn_con, 0.12f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&btn_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&btn_con, -0.02f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&btn_con, -0.07f, UI_CONSTRAINT_RELATIVE);
  if (draw_button(&btn_con, mult_color(COLOR_WHITE, 0.9f), "Yes", btn_con.height * 0.20f, ALIGN_BOTTOM_CENTER, BUTTON_STATE_ACTIVE)) {
    zephr_quit();
  }

  set_y_constraint(&btn_con, -0.02f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&btn_con, 0.07f, UI_CONSTRAINT_RELATIVE);
  if (draw_button(&btn_con, mult_color(COLOR_WHITE, 0.9f), "No", btn_con.height * 0.20f, ALIGN_BOTTOM_CENTER, BUTTON_STATE_ACTIVE)) {
    game.quit_dialog = false;
  }
}

void draw_help_dialog(void) {
  UIConstraints dialog_con = {0};
  set_width_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  draw_quad(&dialog_con, ColorRGBA(0, 0, 0, 200), 0, ALIGN_CENTER);

  UIConstraints content_card_con = {0};
  set_parent_constraint(&content_card_con, &dialog_con);
  set_width_constraint(&content_card_con, 0.4f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&content_card_con, 0.3f, UI_CONSTRAINT_RELATIVE);
  draw_quad(&content_card_con, ColorRGBA(135, 124, 124, 255), 8, ALIGN_CENTER);

  UIConstraints text_con = {0};
  set_parent_constraint(&text_con, &content_card_con);
  set_width_constraint(&text_con, 1, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&text_con, 40, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("How to play", 64.f, text_con, COLOR_YELLOW, ALIGN_TOP_CENTER);

  set_y_constraint(&text_con, 30, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&text_con, 0.9f, UI_CONSTRAINT_RELATIVE);
  draw_text("Use the arrow keys to move the tiles.\n"
            "When two tiles with the same number touch, they\n"
            "merge into one!\n\nYour goal is to reach 2048 without filling all the tiles", 30.f, text_con, COLOR_WHITE, ALIGN_CENTER);

  UIConstraints btn_con = {0};
  set_parent_constraint(&btn_con, &content_card_con);
  set_width_constraint(&btn_con, 56, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&btn_con, 1, UI_CONSTRAINT_ASPECT_RATIO);
  set_y_constraint(&btn_con, -24, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_x_constraint(&btn_con, 24, UI_CONSTRAINT_RELATIVE_PIXELS);
  if (draw_icon_button(&btn_con, mult_color(COLOR_WHITE, 0.8f), game.icon_textures[CLOSE_ICON], btn_con.height * 0.20f, ALIGN_TOP_RIGHT, BUTTON_STATE_ACTIVE)) {
    game.help_dialog = false;
  }
}

void draw_game_over(void) {
  UIConstraints dialog_con = {0};
  set_width_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  draw_quad(&dialog_con, ColorRGBA(0, 0, 0, (u8)game.game_over_bg_opacity), 0, ALIGN_CENTER);

  UIConstraints content_card_con = {0};
  set_parent_constraint(&content_card_con, &dialog_con);
  set_width_constraint(&content_card_con, 0.4f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&content_card_con, 0.2f, UI_CONSTRAINT_RELATIVE);
  draw_quad(&content_card_con, ColorRGBA(135, 124, 124, (u8)game.game_over_opacity), 8, ALIGN_CENTER);

  UIConstraints text_con = {0};
  set_parent_constraint(&text_con, &content_card_con);
  set_width_constraint(&text_con, 1, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&text_con, 40, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("Game Over!", 64.f, text_con, ColorRGBA(255, 255, 0, (u8)game.game_over_opacity), ALIGN_TOP_CENTER);

  UIConstraints btn_con = {0};
  const ButtonState btn_state = game.quit_dialog ? BUTTON_STATE_INACTIVE : game.game_over_opacity > 0 ? BUTTON_STATE_ACTIVE : BUTTON_STATE_INACTIVE;
  set_parent_constraint(&btn_con, &content_card_con);
  set_x_constraint(&btn_con, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&btn_con, -24, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&btn_con, 0.10f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&btn_con, 0.3f, UI_CONSTRAINT_ASPECT_RATIO);
  if (draw_button(&btn_con, ColorRGBA(201, 146, 126, 255), "Try Again", 8.f, ALIGN_BOTTOM_CENTER, btn_state)) {
    game_new();
  }
}

void draw_ui(void) {
  ButtonState bg_btns_state = game.quit_dialog || game.help_dialog
    ? BUTTON_STATE_INACTIVE
    : BUTTON_STATE_ACTIVE;
  const int icon_btn_width = 80;
  const int icon_btn_offset = 8;
  const int icon_btns_padding = 10;

  UIConstraints btn_con = {0};
  set_x_constraint(&btn_con, -icon_btn_offset, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&btn_con, icon_btn_offset, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&btn_con, icon_btn_width, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&btn_con, 1, UI_CONSTRAINT_ASPECT_RATIO);

  if (draw_icon_button(&btn_con, ColorRGBA(201, 146, 126, 255), game.icon_textures[HELP_ICON], btn_con.height * 0.15f, ALIGN_TOP_RIGHT, bg_btns_state)) {
    game.help_dialog = true;
  }

  set_x_constraint(&btn_con, -icon_btn_offset - icon_btn_width - icon_btns_padding, UI_CONSTRAINT_RELATIVE_PIXELS);
  if (draw_icon_button(&btn_con, ColorRGBA(201, 146, 126, 255), game.icon_textures[SETTINGS_ICON], btn_con.height * 0.15f, ALIGN_TOP_RIGHT, bg_btns_state)) {
    printf("TODO: settings\n");
  }

  set_x_constraint(&btn_con, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&btn_con, -0.05f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&btn_con, 200, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&btn_con, 65, UI_CONSTRAINT_RELATIVE_PIXELS);
  if (draw_button(&btn_con, ColorRGBA(201, 172, 126, 255), "New Game", btn_con.height * 0.25f, ALIGN_BOTTOM_CENTER, bg_btns_state)) {
    game_new();
  }

  if (game.help_dialog) {
    draw_help_dialog();
  }

  if (game.has_lost) {
    draw_game_over();
  }

  if (game.quit_dialog) {
    draw_quit_dialog();
  }
}

void draw_board(void) {
  // board
  UIConstraints board_con = {0};
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
  UIConstraints text_con = {0};
  set_width_constraint(&text_con, 1.5, UI_CONSTRAINT_RELATIVE_PIXELS);
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

        draw_text(value, get_tile_font_size(*tile), text_con, mult_color(tile_color, 0.5), ALIGN_CENTER);
      }
    }
  }
}

void update_positions(f64 delta_t) {
  if (game.animating) {
    switch (game.last_move_dir) {
      case MOVE_DIR_LEFT:
        for (int i = 0; i < 4; i++) {
          for (int j = 1; j < 4; j++) {
            u8 tiles_to_move = game.board[i][j].tiles_to_move;
            f32 speed = TILE_ANIM_SPEED * tiles_to_move;
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
            f32 speed = TILE_ANIM_SPEED * tiles_to_move;
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
            f32 speed = TILE_ANIM_SPEED * tiles_to_move;
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
            f32 speed = TILE_ANIM_SPEED * tiles_to_move;
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

  if (gameover()) {
    game.has_lost = true;
    game.animating = true;
  }

  if (game.spawning_new_tile) {
    spawn_random_tile();
    game.spawning_new_tile = false;
  }

  if (game.has_lost) {
    game.game_over_opacity = (float)CORE_MIN(game.game_over_opacity + GAMEOVER_ANIM_SPEED * delta_t, 255);
    game.game_over_bg_opacity = (float)CORE_MIN(game.game_over_bg_opacity + GAMEOVER_ANIM_SPEED * delta_t, 200);
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
  bool can_move = !game.quit_dialog && !game.help_dialog && !game.animating;

  if (e.key.mods & ZEPHR_KEY_MOD_CTRL && e.key.code == ZEPHR_KEYCODE_Q) {
    game_attempt_quit();
  } else if (e.key.code == ZEPHR_KEYCODE_ESCAPE) {
    game.quit_dialog = false;
    game.help_dialog = false;
  } else if (e.key.code == ZEPHR_KEYCODE_F11) {
    zephr_toggle_fullscreen();
  } else if (e.key.code == ZEPHR_KEYCODE_UP) {
    if (can_move)
      move_up();
  } else if (e.key.code == ZEPHR_KEYCODE_DOWN) {
    if (can_move)
      move_down();
  } else if (e.key.code == ZEPHR_KEYCODE_LEFT) {
    if (can_move)
      move_left();
  } else if (e.key.code == ZEPHR_KEYCODE_RIGHT) {
    if (can_move)
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
          if (game.quit_dialog) {
            zephr_quit();
          } else {
            game_attempt_quit();
          }
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

    draw_bg();
    draw_board();
    draw_ui();

    zephr_swap_buffers();
  }
}
