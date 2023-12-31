#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "timer.h"
#include "zephr.h"

#define TILE_ANIM_SPEED 2.5f
#define TILE_SPAWN_SPEED 8.0f
#define GAMEOVER_ANIM_SPEED 175.f

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


Color get_tile_color(u16 value) {
  switch (value) {
    case 2:
      return game.palette.tile_colors[0];
    case 4:
      return game.palette.tile_colors[1];
    case 8:
      return game.palette.tile_colors[2];
    case 16:
      return game.palette.tile_colors[3];
    case 32:
      return game.palette.tile_colors[4];
    case 64:
      return game.palette.tile_colors[5];
    case 128:
      return game.palette.tile_colors[6];
    case 256:
      return game.palette.tile_colors[7];
    case 512:
      return game.palette.tile_colors[8];
    case 1024:
      return game.palette.tile_colors[9];
    case 2048:
      return game.palette.tile_colors[10];
    default:
      return game.palette.tile_colors[10];
  }
}

u8 get_tile_font_size(u16 value) {
  switch (value) {
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

u16 get_spawned_tile_value(void) {
  // 90% chance of spawning a 2, 10% chance of spawning a 4
  if (rand() % 10 < 9) {
    return 2;
  } else {
    return 4;
  }
}

void spawn_new_tile_with_value(u8 x, u8 y, u16 value) {
  game.board[x][y].value = value;
  game.board[x][y].new_value = value;
}

void spawn_new_tile(u8 x, u8 y) {
  const u16 val = get_spawned_tile_value();

  game.board[x][y].value = val;
  game.board[x][y].new_value = val;
}

Vec2 get_random_available_tile_coords(void) {
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

  return (Vec2){x, y};
}

void spawn_random_tile(void) {
  Vec2 coords = get_random_available_tile_coords();
  spawn_new_tile(coords.x, coords.y);
}

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
        game.spawning_tile_coords = get_random_available_tile_coords();
        game.spawning_tile_value = get_spawned_tile_value();
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
        game.spawning_tile_coords = get_random_available_tile_coords();
        game.spawning_tile_value = get_spawned_tile_value();
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
        game.spawning_tile_coords = get_random_available_tile_coords();
        game.spawning_tile_value = get_spawned_tile_value();
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
        game.spawning_tile_coords = get_random_available_tile_coords();
        game.spawning_tile_value = get_spawned_tile_value();
      }
    }
  }
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

void reset_game(void) {
  game.score = 0;
  game.animating = false;
  game.spawning_new_tile = false;
  game.quit_dialog = false;
  game.help_dialog = false;
  game.settings_dialog = false;
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
  game.settings_dialog = false;
  game.quit_dialog = true;
}

void reset_palette(void) {
  game.palette = (ColorPalette){
    .bg_color = ColorRGBA(248, 250, 230, 255),
      .board_bg_color = ColorRGBA(140, 154, 147, 255),

      .tile_colors[0] = ColorRGBA(238, 228, 218, 255), // 2
      .tile_colors[1] = ColorRGBA(237, 224, 200, 255), // 4
      .tile_colors[2] = ColorRGBA(242, 177, 121, 255), // 8
      .tile_colors[3] = ColorRGBA(245, 149, 99, 255),  // 16
      .tile_colors[4] = ColorRGBA(246, 124, 95, 255),  // 32
      .tile_colors[5] = ColorRGBA(246, 94, 59, 255),   // 64
      .tile_colors[6] = ColorRGBA(237, 207, 114, 255), // 128
      .tile_colors[7] = ColorRGBA(237, 204, 97, 255),  // 256
      .tile_colors[8] = ColorRGBA(237, 200, 80, 255),  // 512
      .tile_colors[9] = ColorRGBA(237, 197, 63, 255),  // 1024
      .tile_colors[10] = ColorRGBA(237, 194, 46, 255), // 2048
  };
}

void game_init(void) {
  srand(time(NULL));

  game.icon_textures[HELP_ICON] = load_texture("assets/icons/help.png");
  game.icon_textures[SETTINGS_ICON] = load_texture("assets/icons/settings.png");
  game.icon_textures[CLOSE_ICON] = load_texture("assets/icons/close.png");

  reset_palette();

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
  UIConstraints board_con = default_constraints;

  set_width_constraint(&board_con, window_size.width, UI_CONSTRAINT_FIXED);
  set_height_constraint(&board_con, window_size.height, UI_CONSTRAINT_FIXED);

  // bg
  UiStyle style = {
    .bg_color = game.palette.bg_color,
    .align = ALIGN_TOP_LEFT
  };
  draw_quad(&board_con, style);

  GlyphInstanceList batch;
  new_glyph_instance_list(&batch, 32);

  // 2048 text
  UIConstraints text_con = default_constraints;

  set_y_constraint(&text_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&text_con, 1.5, UI_CONSTRAINT_RELATIVE_PIXELS);

  add_text_instance(&batch, "2048", 100, text_con, ColorRGBA(34, 234, 82, 155), ALIGN_TOP_CENTER);

  // score
  char score[24];
  sprintf(score, "Score   %d", game.score);

  set_parent_constraint(&text_con, NULL);
  set_x_constraint(&text_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&text_con, 0.8f, UI_CONSTRAINT_RELATIVE);
  add_text_instance(&batch, score, 40, text_con, COLOR_BLACK, ALIGN_TOP_LEFT);

  draw_text_batch(&batch);
}

void draw_quit_dialog(void) {
  UIConstraints dialog_con = default_constraints;
  set_x_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_width_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  UiStyle style = {
    .bg_color = ColorRGBA(0, 0, 0, 200),
    .align = ALIGN_CENTER,
  };
  draw_quad(&dialog_con, style);

  set_width_constraint(&dialog_con, 0.4f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 0.2f, UI_CONSTRAINT_RELATIVE);
  style = (UiStyle){
    .bg_color = ColorRGBA(135, 124, 124, 255),
    .border_radius = 8,
    .align = ALIGN_CENTER,
  };
  draw_quad(&dialog_con, style);

  UIConstraints text_con = default_constraints;
  set_parent_constraint(&text_con, &dialog_con);
  set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&text_con, -0.25f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&text_con, 1, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("Are you sure you want to quit?", 36.f, text_con, COLOR_WHITE, ALIGN_CENTER);

  UIConstraints btn_con = default_constraints;
  set_parent_constraint(&btn_con, &dialog_con);
  set_width_constraint(&btn_con, 0.25f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&btn_con, 0.25f, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&btn_con, -0.10f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&btn_con, -0.20f, UI_CONSTRAINT_RELATIVE);
  style = (UiStyle){
    .bg_color = mult_color(COLOR_WHITE, 0.9f),
    .fg_color = COLOR_BLACK,
    .border_radius = btn_con.height * 0.2f,
    .align = ALIGN_BOTTOM_CENTER,
  };
  if (draw_button(&btn_con, "Yes", style, BUTTON_STATE_ACTIVE)) {
    zephr_quit();
  }

  set_y_constraint(&btn_con, -0.10f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&btn_con, 0.20f, UI_CONSTRAINT_RELATIVE);
  if (draw_button(&btn_con, "No", style, BUTTON_STATE_ACTIVE)) {
    game.quit_dialog = false;
  }
}

void draw_help_dialog(void) {
  UIConstraints dialog_con = default_constraints;
  set_width_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  UiStyle style = {
    .bg_color = ColorRGBA(0, 0, 0, 200),
    .align = ALIGN_CENTER,
  };
  draw_quad(&dialog_con, style);

  UIConstraints content_card_con = default_constraints;
  set_parent_constraint(&content_card_con, &dialog_con);
  set_width_constraint(&content_card_con, 0.4f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&content_card_con, 0.33f, UI_CONSTRAINT_RELATIVE);
  style = (UiStyle){
    .bg_color = ColorRGBA(135, 124, 124, 255),
    .border_radius = 8,
    .align = ALIGN_CENTER,
  };
  draw_quad(&content_card_con, style);

  UIConstraints text_con = default_constraints;
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

  UIConstraints btn_con = default_constraints;
  set_parent_constraint(&btn_con, &content_card_con);
  set_width_constraint(&btn_con, 56, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&btn_con, 1, UI_CONSTRAINT_ASPECT_RATIO);
  set_y_constraint(&btn_con, -24, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_x_constraint(&btn_con, 24, UI_CONSTRAINT_RELATIVE_PIXELS);
  style = (UiStyle){
    .bg_color = mult_color(COLOR_WHITE, 0.8f),
    .fg_color = ColorRGBA(232, 28, 36, 255),
    .border_radius = btn_con.height * 0.2f,
    .align = ALIGN_TOP_RIGHT,
  };
  if (draw_icon_button(&btn_con, game.icon_textures[CLOSE_ICON], style, BUTTON_STATE_ACTIVE)) {
    game.help_dialog = false;
  }
}

void draw_settings_dialog(void) {
  UIConstraints dialog_con = default_constraints;
  set_width_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  UiStyle style = {
    .bg_color = ColorRGBA(0, 0, 0, 200),
    .align = ALIGN_CENTER,
  };
  draw_quad(&dialog_con, style);

  UIConstraints content_card_con = default_constraints;
  set_parent_constraint(&content_card_con, &dialog_con);
  set_width_constraint(&content_card_con, 0.55f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&content_card_con, 0.6f, UI_CONSTRAINT_RELATIVE);
  style = (UiStyle){
    .bg_color = ColorRGBA(135, 124, 124, 255),
    .border_radius = 8,
    .align = ALIGN_CENTER,
  };
  draw_quad(&content_card_con, style);

  UIConstraints text_con = default_constraints;
  set_parent_constraint(&text_con, &content_card_con);
  set_width_constraint(&text_con, 1, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&text_con, 40, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("Settings", 64.f, text_con, COLOR_YELLOW, ALIGN_TOP_CENTER);

  set_x_constraint(&text_con, 30, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&text_con, 150, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("Color Palette", 40.f, text_con, COLOR_WHITE, ALIGN_TOP_LEFT);

  const char *bg_color_text = "Background: ";
  const u8 font_size = 30.f;
  set_x_constraint(&text_con, 30, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&text_con, 220, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&text_con, 0.9f, UI_CONSTRAINT_RELATIVE);
  draw_text(bg_color_text, font_size, text_con, COLOR_WHITE, ALIGN_TOP_LEFT);

  Sizef text_size = calculate_text_size(bg_color_text, font_size);
  UIConstraints color_picker_con = default_constraints;
  set_parent_constraint(&color_picker_con, &content_card_con);
  set_x_constraint(&color_picker_con, 30 + text_size.width + 20, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&color_picker_con, 220 - text_size.height / 2.f + 6, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&color_picker_con, 100, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&color_picker_con, 40, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_color_picker(&color_picker_con, &game.palette.bg_color, ALIGN_TOP_LEFT, BUTTON_STATE_ACTIVE);

  set_y_constraint(&text_con, 270, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("Board: ", font_size, text_con, COLOR_WHITE, ALIGN_TOP_LEFT);

  set_x_constraint(&color_picker_con, 30 + text_size.width + 20, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&color_picker_con, 270 - text_size.height / 2.f + 6, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&color_picker_con, 100, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&color_picker_con, 40, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_color_picker(&color_picker_con, &game.palette.board_bg_color, ALIGN_TOP_LEFT, BUTTON_STATE_ACTIVE);

  set_y_constraint(&text_con, 320, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("Tiles: ", font_size, text_con, COLOR_WHITE, ALIGN_TOP_LEFT);

  const u8 tile_size = 60;
  for (u8 i = 0; i < 11; i++) {
    set_x_constraint(&color_picker_con, 30 + text_size.width + 20 + (i * (tile_size + 10)), UI_CONSTRAINT_RELATIVE_PIXELS);
    set_y_constraint(&color_picker_con, 320 - text_size.height / 2.f + 6, UI_CONSTRAINT_RELATIVE_PIXELS);
    set_width_constraint(&color_picker_con, tile_size, UI_CONSTRAINT_RELATIVE_PIXELS);
    set_height_constraint(&color_picker_con, tile_size, UI_CONSTRAINT_RELATIVE_PIXELS);
    draw_color_picker_with_id(i, &color_picker_con, &game.palette.tile_colors[i], ALIGN_TOP_LEFT, BUTTON_STATE_ACTIVE);

    u16 tile_val = (u16)pow(2, i + 1);
    char text[16];
    sprintf(text, "%d", tile_val);
    const f32 font_size = get_tile_font_size(tile_val) * 0.5f;
    const Color tile_color = get_tile_color(tile_val);
    set_parent_constraint(&text_con, &color_picker_con);
    set_x_constraint(&text_con, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
    set_y_constraint(&text_con, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
    draw_text(text, (u8)font_size, text_con, mult_color(tile_color, 0.5f), ALIGN_CENTER);
  }

  UIConstraints btn_con = default_constraints;
  set_parent_constraint(&btn_con, &content_card_con);
  set_x_constraint(&btn_con, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&btn_con, 400, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&btn_con, 150, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&btn_con, 0.35f, UI_CONSTRAINT_ASPECT_RATIO);
  style = (UiStyle){
    .bg_color = ColorRGBA(201, 146, 126, 255),
    .fg_color = COLOR_BLACK,
    .border_radius = 8,
    .align = ALIGN_TOP_CENTER,
  };
  if (draw_button(&btn_con, "Reset", style, BUTTON_STATE_ACTIVE)) {
    reset_palette();
  }

  set_width_constraint(&btn_con, 56, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&btn_con, 1, UI_CONSTRAINT_ASPECT_RATIO);
  set_y_constraint(&btn_con, -24, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_x_constraint(&btn_con, 24, UI_CONSTRAINT_RELATIVE_PIXELS);
  style = (UiStyle){
    .bg_color = mult_color(COLOR_WHITE, 0.8f),
    .fg_color = ColorRGBA(232, 28, 36, 255),
    .border_radius = btn_con.height * 0.2f,
    .align = ALIGN_TOP_RIGHT,
  };
  if (draw_icon_button(&btn_con, game.icon_textures[CLOSE_ICON], style, BUTTON_STATE_ACTIVE)) {
    game.settings_dialog = false;
  }
}

void draw_game_over(void) {
  UIConstraints dialog_con = default_constraints;
  set_width_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&dialog_con, 1.f, UI_CONSTRAINT_RELATIVE);
  set_x_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&dialog_con, 0, UI_CONSTRAINT_FIXED);
  UiStyle style = {
    .bg_color = ColorRGBA(0, 0, 0, (u8)game.game_over_bg_opacity),
    .align = ALIGN_CENTER,
  };
  draw_quad(&dialog_con, style);

  UIConstraints content_card_con = default_constraints;
  set_parent_constraint(&content_card_con, &dialog_con);
  set_width_constraint(&content_card_con, 0.3f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&content_card_con, 0.2f, UI_CONSTRAINT_RELATIVE);
  style = (UiStyle){
    .bg_color = ColorRGBA(135, 124, 124, (u8)game.game_over_opacity),
    .border_radius = 8,
    .align = ALIGN_CENTER,
  };
  draw_quad(&content_card_con, style);

  UIConstraints text_con = default_constraints;
  set_parent_constraint(&text_con, &content_card_con);
  set_width_constraint(&text_con, 1, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&text_con, 40, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_text("Game Over!", 64.f, text_con, ColorRGBA(255, 255, 0, (u8)game.game_over_opacity), ALIGN_TOP_CENTER);

  UIConstraints btn_con = default_constraints;
  const ButtonState btn_state = game.quit_dialog ? BUTTON_STATE_INACTIVE : game.game_over_opacity > 0 ? BUTTON_STATE_ACTIVE : BUTTON_STATE_INACTIVE;
  set_parent_constraint(&btn_con, &content_card_con);
  set_x_constraint(&btn_con, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&btn_con, -24, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&btn_con, 0.30f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&btn_con, 0.3f, UI_CONSTRAINT_ASPECT_RATIO);
  style = (UiStyle){
    .bg_color = ColorRGBA(201, 146, 126, (u8)game.game_over_opacity),
    .border_radius = 8,
    .align = ALIGN_BOTTOM_CENTER,
  };
  if (draw_button(&btn_con, "Try Again", style, btn_state)) {
    reset_game();
  }
}

void draw_ui(void) {
  ButtonState bg_btns_state = game.quit_dialog || game.help_dialog || game.settings_dialog || game.has_lost
    ? BUTTON_STATE_INACTIVE
    : BUTTON_STATE_ACTIVE;
  const int icon_btn_width = 80;
  const int icon_btn_offset = 8;
  const int icon_btns_padding = 10;

  UIConstraints btn_con = default_constraints;
  set_x_constraint(&btn_con, -icon_btn_offset, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&btn_con, icon_btn_offset, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&btn_con, icon_btn_width, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&btn_con, 1, UI_CONSTRAINT_ASPECT_RATIO);

  UiStyle style = {
    .bg_color = ColorRGBA(201, 146, 126, 255),
    .fg_color = ColorRGBA(40, 92, 100, 255),
    .border_radius = btn_con.height * 0.15f,
    .align = ALIGN_TOP_RIGHT,
  };
  if (draw_icon_button(&btn_con, game.icon_textures[HELP_ICON], style, bg_btns_state)) {
    game.help_dialog = true;
  }

  set_x_constraint(&btn_con, -icon_btn_offset - icon_btn_width - icon_btns_padding, UI_CONSTRAINT_RELATIVE_PIXELS);
  if (draw_icon_button(&btn_con, game.icon_textures[SETTINGS_ICON], style, bg_btns_state)) {
    game.settings_dialog = true;
  }

  set_x_constraint(&btn_con, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&btn_con, -0.05f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&btn_con, 200, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&btn_con, 65, UI_CONSTRAINT_RELATIVE_PIXELS);
  style = (UiStyle) {
    .bg_color = ColorRGBA(201, 172, 126, 255),
    .fg_color = COLOR_BLACK,
    .border_radius = btn_con.height * 0.25f,
    .align = ALIGN_BOTTOM_CENTER,
  };
  if (draw_button(&btn_con, "New Game", style, bg_btns_state)) {
    reset_game();
  }

  if (game.settings_dialog) {
    draw_settings_dialog();
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
  UIConstraints board_con = default_constraints;
  set_y_constraint(&board_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&board_con, 0.6f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&board_con, 1, UI_CONSTRAINT_ASPECT_RATIO);
  UiStyle style = {
    .bg_color = game.palette.board_bg_color,
    .border_radius = board_con.width * 0.02f,
    .align = ALIGN_CENTER,
  };
  draw_quad(&board_con, style);

  // empty tiles
  const float tile_padding = board_con.width * 0.02f;
  const float tile_height = board_con.height * 0.225f;
  const float tile_board_radius = tile_height * 0.15f;

  UIConstraints empty_tile_con = default_constraints;
  set_parent_constraint(&empty_tile_con, &board_con);
  set_height_constraint(&empty_tile_con, tile_height, UI_CONSTRAINT_FIXED);
  set_width_constraint(&empty_tile_con, 1, UI_CONSTRAINT_ASPECT_RATIO);

  style = (UiStyle){
    .bg_color = mult_color(game.palette.board_bg_color, 0.85f),
    .border_radius = tile_board_radius,
    .align = ALIGN_TOP_LEFT,
  };
  for (int i = 0; i < 4; i++) {
    set_y_constraint(&empty_tile_con, (tile_height + tile_padding) * i + tile_padding, UI_CONSTRAINT_FIXED);
    for (int j = 0; j < 4; j++) {
      set_x_constraint(&empty_tile_con, (tile_height + tile_padding) * j + tile_padding, UI_CONSTRAINT_FIXED);
      draw_quad(&empty_tile_con, style);
    }
  }

  // filled tiles
  UIConstraints text_con = default_constraints;
  set_width_constraint(&text_con, 1.5, UI_CONSTRAINT_RELATIVE_PIXELS);
  UIConstraints tile_con = default_constraints;
  set_parent_constraint(&tile_con, &board_con);
  set_height_constraint(&tile_con, tile_height, UI_CONSTRAINT_FIXED);
  set_width_constraint(&tile_con, 1, UI_CONSTRAINT_ASPECT_RATIO);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      Tile *tile = &game.board[i][j];
      Color tile_color = get_tile_color(tile->value);
      UiStyle style = {
        .bg_color = tile_color,
        .border_radius = tile_board_radius,
        .align = ALIGN_TOP_LEFT,
      };
      char value[16];
      sprintf(value, "%d", tile->value);
      f32 tile_y_pos = (f32)((tile_height + tile_padding) * i + tile_padding + game.board[i][j].anim_y_offset_relative * board_con.height);
      f32 tile_x_pos = (f32)((tile_height + tile_padding) * j + tile_padding + game.board[i][j].anim_x_offset_relative * board_con.height);

      set_y_constraint(&tile_con, tile_y_pos, UI_CONSTRAINT_FIXED);
      set_x_constraint(&tile_con, tile_x_pos, UI_CONSTRAINT_FIXED);
      if (tile->value > 0) {
        draw_quad(&tile_con, style);

        // value text
        set_parent_constraint(&text_con, &tile_con);
        set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
        set_y_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);

        draw_text(value, get_tile_font_size(tile->value), text_con, mult_color(tile_color, 0.5), ALIGN_CENTER);
      }
    }
  }

  if (game.spawning_new_tile) {
    // spawning tile
    Color tile_color = get_tile_color(game.spawning_tile_value);
    UiStyle style = {
      .bg_color = tile_color,
      .border_radius = tile_board_radius,
      .align = ALIGN_TOP_LEFT,
    };
    f32 tile_x_pos = (f32)((tile_height + tile_padding) * game.spawning_tile_coords.y + tile_padding) + tile_height / 2 - game.spawning_tile_scale * tile_height / 2;
    f32 tile_y_pos = (f32)((tile_height + tile_padding) * game.spawning_tile_coords.x + tile_padding) + tile_height / 2 - game.spawning_tile_scale * tile_height / 2;
    set_x_constraint(&tile_con, tile_x_pos, UI_CONSTRAINT_FIXED);
    set_y_constraint(&tile_con, tile_y_pos, UI_CONSTRAINT_FIXED);
    set_height_constraint(&tile_con, tile_height * game.spawning_tile_scale, UI_CONSTRAINT_FIXED);
    set_width_constraint(&tile_con, 1, UI_CONSTRAINT_ASPECT_RATIO);
    draw_quad(&tile_con, style);
    // value text
    set_parent_constraint(&text_con, &tile_con);
    set_x_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);
    set_y_constraint(&text_con, 0, UI_CONSTRAINT_FIXED);

    char value[16];
    sprintf(value, "%d", game.spawning_tile_value);
    draw_text(value, (u8)(get_tile_font_size(game.spawning_tile_value) * game.spawning_tile_scale), text_con, mult_color(tile_color, 0.5), ALIGN_CENTER);
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
                if (dest->new_value == src->value * 2) {
                  dest->merged = false;
                }

                src->value = src->new_value;
                dest->value = dest->new_value;

                src->tiles_to_move = 0;
                src->anim_x_offset_relative = 0;
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
                if (dest->new_value == src->value * 2) {
                  dest->merged = false;
                }

                src->value = src->new_value;
                dest->value = dest->new_value;

                src->tiles_to_move = 0;
                src->anim_x_offset_relative = 0;
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
                if (dest->new_value == src->value * 2) {
                  dest->merged = false;
                }

                src->value = src->new_value;
                dest->value = dest->new_value;

                src->tiles_to_move = 0;
                src->anim_y_offset_relative = 0;
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
                if (dest->new_value == src->value * 2) {
                  dest->merged = false;
                }

                src->value = src->new_value;
                dest->value = dest->new_value;

                src->tiles_to_move = 0;
                src->anim_y_offset_relative = 0;
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
    game.spawning_tile_scale += (float)(TILE_SPAWN_SPEED * delta_t);

    if (game.spawning_tile_scale >= 1.0f) {
      spawn_new_tile_with_value(game.spawning_tile_coords.x, game.spawning_tile_coords.y, game.spawning_tile_value);
      game.spawning_tile_coords = (Vec2){0, 0};
      game.spawning_tile_value = 0;
      game.spawning_new_tile = false;
      game.animating = false;
      game.spawning_tile_scale = 0.0f;
    }
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
  bool can_move = !game.quit_dialog && !game.help_dialog && !game.settings_dialog && !game.animating;

  if (e.key.mods & ZEPHR_KEY_MOD_CTRL && e.key.code == ZEPHR_KEYCODE_Q) {
    game_attempt_quit();
  } else if (e.key.code == ZEPHR_KEYCODE_ESCAPE) {
    game.quit_dialog = false;
    game.help_dialog = false;
    game.settings_dialog = false;
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
