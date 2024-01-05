#include <math.h>
#include <stdio.h>

#include <glad/glx.h>
#include <stb_image.h>

#include "shader.h"
#include "zephr.h"

Shader ui_shader;
Shader color_chooser_shader;
unsigned int ui_vao;
unsigned int ui_vbo;

const UIConstraints default_constraints = {
  .scale = {.height = 1, .width = 1},
};

int init_ui(const char* font_path) {
  int res = init_fonts(font_path);
  if (res == -1) {
    printf("[ERROR]: could not initialize freetype library\n");
    return 1;
  } else if (res == -2) {
    printf("[ERROR]: could not load font file: \"%s\"\n", font_path);
    return 1;
  }

  ui_shader = create_shader("shaders/ui.vert", "shaders/ui.frag");
  color_chooser_shader = create_shader("shaders/ui.vert", "shaders/color_chooser.frag");

  glGenVertexArrays(1, &ui_vao);
  glGenBuffers(1, &ui_vbo);

  glBindVertexArray(ui_vao);
  glBindBuffer(GL_ARRAY_BUFFER, ui_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return 0;
}

void set_parent_constraint(UIConstraints *constraints, UIConstraints *parent_constraints) {
  constraints->parent = parent_constraints;
}

void set_x_constraint(UIConstraints *constraints, float value, UIConstraint type) {
  switch (type) {
    case UI_CONSTRAINT_ASPECT_RATIO:
      // no-op. fallback to FIXED
    case UI_CONSTRAINT_FIXED:
      constraints->x = value;
      break;
    case UI_CONSTRAINT_RELATIVE:
      if (constraints->parent) {
        constraints->x = (value * constraints->parent->width);
      } else {
        constraints->x = (value * zephr_ctx->window.size.width);
      }
      break;
    case UI_CONSTRAINT_RELATIVE_PIXELS:
      constraints->x = zephr_ctx->window.size.width / (float)zephr_ctx->screen_size.width * value;
      break;
  }

  if (constraints->parent) {
    constraints->x += constraints->parent->x;
  }
}

void set_y_constraint(UIConstraints *constraints, float value, UIConstraint type) {
  switch (type) {
    case UI_CONSTRAINT_ASPECT_RATIO:
      // no-op. fallback to FIXED
    case UI_CONSTRAINT_FIXED:
      constraints->y = value;
      break;
    case UI_CONSTRAINT_RELATIVE:
      if (constraints->parent) {
        constraints->y = (value * constraints->parent->height);
      } else {
        constraints->y = (value * zephr_ctx->window.size.height);
      }
      break;
    case UI_CONSTRAINT_RELATIVE_PIXELS:
      constraints->y = zephr_ctx->window.size.height / (float)zephr_ctx->screen_size.height * value;
      break;
  }

  if (constraints->parent) {
    constraints->y += constraints->parent->y;
  }
}

void set_width_constraint(UIConstraints *constraints, float value, UIConstraint type) {
  switch (type) {
    case UI_CONSTRAINT_FIXED:
      constraints->width = value;
      break;
    case UI_CONSTRAINT_RELATIVE:
      if (constraints->parent) {
        constraints->width = (value * constraints->parent->width);
      } else {
        constraints->width = (value * zephr_ctx->window.size.width);
      }
      break;
    case UI_CONSTRAINT_RELATIVE_PIXELS:
      constraints->width = zephr_ctx->window.size.width / (float)zephr_ctx->screen_size.width * value;
      break;
    case UI_CONSTRAINT_ASPECT_RATIO:
      constraints->width = constraints->height * value;
      break;
  }
}

void set_height_constraint(UIConstraints *constraints, float value, UIConstraint type) {
  switch (type) {
    case UI_CONSTRAINT_FIXED:
      constraints->height = value;
      break;
    case UI_CONSTRAINT_RELATIVE:
      if (constraints->parent) {
        constraints->height = (value * constraints->parent->height);
      } else {
        constraints->height = (value * zephr_ctx->window.size.height);
      }
      break;
    case UI_CONSTRAINT_RELATIVE_PIXELS:
      constraints->height = zephr_ctx->window.size.height / (float)zephr_ctx->screen_size.height * value;
      break;
    case UI_CONSTRAINT_ASPECT_RATIO:
      constraints->height = constraints->width * value;
      break;
  }
}

void set_rotation_constraint(UIConstraints *constraints, float angle_d) {
  constraints->rotation = angle_d;
}

void apply_constraints(UIConstraints *constraints, Vec2f *pos, Sizef *size) {
  *pos = (Vec2f){constraints->x, constraints->y};
  *size = (Sizef){constraints->width, constraints->height};
}

void apply_constraints_rect(UIConstraints *constraints, Rect *rect) {
  rect->pos = (Vec2f){constraints->x, constraints->y};
  rect->size = (Sizef){constraints->width, constraints->height};
}

void apply_alignment(Alignment align, UIConstraints *constraints, Vec2f *pos, Sizef size) {
  // if we don't have a parent then we're a top level element and we should align against the window
  Sizef parent_size = (Sizef){zephr_ctx->window.size.width, zephr_ctx->window.size.height};
  if (constraints->parent) {
    parent_size = (Sizef){constraints->parent->width, constraints->parent->height};
  }

  switch (align) {
    case ALIGN_TOP_LEFT:
      pos->x = pos->x;
      pos->y = pos->y;
      break;
    case ALIGN_TOP_CENTER:
      pos->x += parent_size.width / 2.f - size.width / 2.f;
      pos->y = pos->y;
      break;
    case ALIGN_TOP_RIGHT:
      pos->x += parent_size.width - size.width;
      pos->y = pos->y;
      break;
    case ALIGN_BOTTOM_LEFT:
      pos->x = pos->x;
      pos->y += parent_size.height - size.height;
      break;
    case ALIGN_BOTTOM_CENTER:
      pos->x += parent_size.width / 2.f - size.width / 2.f;
      pos->y += parent_size.height - size.height;
      break;
    case ALIGN_BOTTOM_RIGHT:
      pos->x += parent_size.width - size.width;
      pos->y += parent_size.height - size.height;
      break;
    case ALIGN_LEFT_CENTER:
      pos->x = pos->x;
      pos->y += parent_size.height / 2.f - size.height / 2.f;
      break;
    case ALIGN_RIGHT_CENTER:
      pos->x += parent_size.width - size.width;
      pos->y += parent_size.height / 2.f - size.height / 2.f;
      break;
    case ALIGN_CENTER:
      pos->x += parent_size.width / 2.f - size.width / 2.f;
      pos->y += parent_size.height / 2.f - size.height / 2.f;
      break;
  }
}

bool inside_rect(Rect *rect, Vec2 *point) {
  /* bool outside_popup = true; */
  /* if (rect != &zephr_ctx->ui.popup_rect) { */
  /*   outside_popup = !inside_rect(&zephr_ctx->ui.popup_rect, point); */
  /* } else { */
  /*   outside_popup = false; */
  /* } */

  if ((point->x >= rect->pos.x && point->x <= rect->pos.x + rect->size.width &&
      point->y >= rect->pos.y && point->y <= rect->pos.y + rect->size.height)) {
    return true;
  }
  return false;
}

void draw_quad(UIConstraints *constraints, const Color color, float border_radius, Alignment align) {
  use_shader(ui_shader);

  set_vec4f(ui_shader, "aColor", color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
  set_float(ui_shader, "uiWidth", constraints->width);
  set_float(ui_shader, "uiHeight", constraints->height);
  set_float(ui_shader, "borderRadius", border_radius);
  set_mat4f(ui_shader, "projection", (float *)zephr_ctx->projection.m);

  Vec2f pos = { 0.f, 0.f };
  Sizef size = { 0.f, 0.f };

  apply_constraints(constraints, &pos, &size);
  apply_alignment(align, constraints, &pos, size);

  // set the positions after applying alignment so children can use them
  constraints->x = pos.x;
  constraints->y = pos.y;

  Matrix4x4 model = identity();
  apply_translation(&model, (Vec2f){-size.width / 2.f, -size.height / 2.f});
  apply_scale(&model, constraints->scale);
  apply_translation(&model, (Vec2f){size.width / 2.f, size.height / 2.f});

  apply_translation(&model, (Vec2f){-size.width / 2.f, -size.height / 2.f});
  apply_rotation(&model, to_radians(constraints->rotation));
  apply_translation(&model, (Vec2f){size.width / 2.f, size.height / 2.f});

  apply_translation(&model, pos);

  set_mat4f(ui_shader, "model", (float *)model.m);

  glBindVertexArray(ui_vao);

  float vertices[6][4] = {
    // bottom left tri
    {0,              0 + size.height, 0.0, 1.0},
    {0,              0,               0.0, 0.0},
    {0 + size.width, 0,               1.0, 0.0},

    // top right tri
    {0,              0 + size.height, 0.0, 1.0},
    {0 + size.width, 0,               1.0, 0.0},
    {0 + size.width, 0 + size.height, 1.0, 1.0},
  };

  glBindBuffer(GL_ARRAY_BUFFER, ui_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
}

void draw_circle(UIConstraints *constraints, const Color color, Alignment align) {
  float radius = 0.f;

  if (constraints->width > constraints->height) {
    radius = constraints->height / 2.f;
  } else {
    radius = constraints->width / 2.f;
  }
  draw_quad(constraints, color, radius, align);
}

void draw_triangle(UIConstraints *constraints, const Color color, Alignment align) {
  use_shader(ui_shader);

  set_vec4f(ui_shader, "aColor", color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
  set_float(ui_shader, "borderRadius", 0.f);
  set_mat4f(ui_shader, "projection", (float *)zephr_ctx->projection.m);

  Vec2f pos = { 0.f, 0.f };
  Sizef size = { 0.f, 0.f };

  apply_constraints(constraints, &pos, &size);
  apply_alignment(align, constraints, &pos, size);

  // set the positions after applying alignment so children can use them
  constraints->x = pos.x;
  constraints->y = pos.y;

  Matrix4x4 model = identity();
  apply_translation(&model, (Vec2f){-size.width / 2.f, -size.height / 2.f});
  apply_scale(&model, constraints->scale);
  apply_translation(&model, (Vec2f){size.width / 2.f, size.height / 2.f});

  apply_translation(&model, (Vec2f){-size.width / 2.f, -size.height / 2.f});
  apply_rotation(&model, to_radians(constraints->rotation));
  apply_translation(&model, (Vec2f){size.width / 2.f, size.height / 2.f});

  apply_translation(&model, pos);

  set_mat4f(ui_shader, "model", (float *)model.m);

  glBindVertexArray(ui_vao);

  float vertices[3][4] = {
    {0 + size.width,     0,               0.0, 0.0},
    {0,                  0,               0.0, 0.0},
    {0 + size.width / 2, 0 + size.height, 0.0, 0.0},
  };

  glBindBuffer(GL_ARRAY_BUFFER, ui_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glBindVertexArray(0);
}

void draw_texture(UIConstraints *constraints, const TextureId texture_id, const Color color, f32 radius, Alignment align) {
  use_shader(ui_shader);

  set_vec4f(ui_shader, "aColor", color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
  set_float(ui_shader, "uiWidth", constraints->width);
  set_float(ui_shader, "uiHeight", constraints->height);
  set_float(ui_shader, "borderRadius", radius);
  set_bool(ui_shader, "hasTexture", true);

  set_mat4f(ui_shader, "projection", (float *)zephr_ctx->projection.m);

  Vec2f pos = { 0.f, 0.f };
  Sizef size = { 0.f, 0.f };

  apply_constraints(constraints, &pos, &size);
  apply_alignment(align, constraints, &pos, size);

  // set the positions after applying alignment so children can use them
  constraints->x = pos.x;
  constraints->y = pos.y;

  Matrix4x4 model = identity();
  apply_translation(&model, (Vec2f){-size.width / 2.f, -size.height / 2.f});
  apply_scale(&model, constraints->scale);
  apply_translation(&model, (Vec2f){size.width / 2.f, size.height / 2.f});

  apply_translation(&model, (Vec2f){-size.width / 2.f, -size.height / 2.f});
  apply_rotation(&model, to_radians(constraints->rotation));
  apply_translation(&model, (Vec2f){size.width / 2.f, size.height / 2.f});

  apply_translation(&model, pos);

  set_mat4f(ui_shader, "model", (float *)model.m);

  glBindVertexArray(ui_vao);

  float vertices[6][4] = {
    // bottom left tri
    {0,              0 + size.height, 0.0, 1.0},
    {0,              0,               0.0, 0.0},
    {0 + size.width, 0,               1.0, 0.0},

    // top right tri
    {0,              0 + size.height, 0.0, 1.0},
    {0 + size.width, 0,               1.0, 0.0},
    {0 + size.width, 0 + size.height, 1.0, 1.0},
  };

  glBindBuffer(GL_ARRAY_BUFFER, ui_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
  set_bool(ui_shader, "hasTexture", false);
}

bool draw_button(UIConstraints *constraints, const Color color, const char *text, f32 radius, Alignment align, ButtonState state) {
  Color text_color = COLOR_BLACK;
  text_color.a = color.a;
  Rect rect = {0};

  apply_constraints(constraints, &rect.pos, &rect.size);
  apply_alignment(align, constraints, &rect.pos, rect.size);

  bool is_hovered = inside_rect(&rect, &zephr_ctx->mouse.pos);
  bool left_mouse_pressed = zephr_ctx->mouse.pressed && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT;

  if (is_hovered && state == BUTTON_STATE_ACTIVE) {
    zephr_set_cursor(ZEPHR_CURSOR_HAND);

    if (left_mouse_pressed) {
      Color new_color = mult_color(color, 0.8f);
      draw_quad(constraints, new_color, radius, align);
    } else {
      Color new_color = mult_color(color, 0.9f);
      draw_quad(constraints, new_color, radius, align);
    }
  } else if (state == BUTTON_STATE_DISABLED) {
    if (is_hovered) {
      zephr_set_cursor(ZEPHR_CURSOR_DISABLED);
    }

    Color new_color = color;
    new_color.a = 100;
    text_color.a = 100;
    draw_quad(constraints, new_color, radius, align);
  } else {
    draw_quad(constraints, color, radius, align);
  }

  if (text) {
    UIConstraints text_constraints = default_constraints;

    set_parent_constraint(&text_constraints, constraints);
    set_x_constraint(&text_constraints, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
    set_y_constraint(&text_constraints, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
    set_width_constraint(&text_constraints, 1.0, UI_CONSTRAINT_RELATIVE_PIXELS);

    u8 font_size = (u8)((constraints->width) / (text_constraints.width * 0.1 * 65.f));

    // TODO: need to adjust the font size based on the text height

    draw_text(text, font_size, text_constraints, text_color, ALIGN_CENTER);
  }

  if (is_hovered && state == BUTTON_STATE_ACTIVE && zephr_ctx->mouse.released && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT) {
    return true;
  }

  return false;
}

bool draw_icon_button(UIConstraints *constraints, Color btn_color, const TextureId icon_tex_id, f32 radius, Alignment align, ButtonState state) {
  CORE_DEBUG_ASSERT(icon_tex_id, "draw_icon_button() requires that you provide an icon texture");

  Rect rect = {0};
  Color icon_color = COLOR_WHITE;

  apply_constraints(constraints, &rect.pos, &rect.size);
  apply_alignment(align, constraints, &rect.pos, rect.size);

  bool is_hovered = inside_rect(&rect, &zephr_ctx->mouse.pos);
  bool left_mouse_pressed = zephr_ctx->mouse.pressed && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT;

  if (is_hovered && state == BUTTON_STATE_ACTIVE) {
    zephr_set_cursor(ZEPHR_CURSOR_HAND);

    if (left_mouse_pressed) {
      btn_color = mult_color(btn_color, 0.8f);
      icon_color = mult_color(icon_color, 0.8f);
    } else {
      btn_color = mult_color(btn_color, 0.9f);
      icon_color = mult_color(icon_color, 0.9f);
    }
  } else if (state == BUTTON_STATE_DISABLED) {
    if (is_hovered) {
      zephr_set_cursor(ZEPHR_CURSOR_DISABLED);
    }

    icon_color.a = 100;
    btn_color.a = 100;
  }

  draw_quad(constraints, btn_color, radius, align);

  UIConstraints icon_constraints = default_constraints;

  set_parent_constraint(&icon_constraints, constraints);
  set_x_constraint(&icon_constraints, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&icon_constraints, 0, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&icon_constraints, constraints->width * 0.8f, UI_CONSTRAINT_FIXED);
  set_height_constraint(&icon_constraints, constraints->height * 0.8f, UI_CONSTRAINT_FIXED);
  draw_texture(&icon_constraints, icon_tex_id, icon_color, 0, ALIGN_CENTER);

  if (is_hovered && state == BUTTON_STATE_ACTIVE && zephr_ctx->mouse.released && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT) {
    return true;
  }

  return false;
}

void draw_color_picker_slider(UIConstraints *constraints, Alignment align) {
  use_shader(color_chooser_shader);

  set_bool(color_chooser_shader, "isSlider", true);
  set_mat4f(color_chooser_shader, "projection", (float *)zephr_ctx->projection.m);

  Vec2f pos = { 0.f, 0.f };
  Sizef size = { 0.f, 0.f };

  apply_constraints(constraints, &pos, &size);
  apply_alignment(align, constraints, &pos, size);

  // set the positions after applying alignment so children can use them
  constraints->x = pos.x;
  constraints->y = pos.y;

  Matrix4x4 model = identity();
  apply_translation(&model, pos);

  set_mat4f(color_chooser_shader, "model", (float *)model.m);

  glBindVertexArray(ui_vao);

  float vertices[6][4] = {
    // bottom left tri
    {0,              0 + size.height, 0.0, 1.0},
    {0,              0,               0.0, 0.0},
    {0 + size.width, 0,               1.0, 0.0},

    // top right tri
    {0,              0 + size.height, 0.0, 1.0},
    {0 + size.width, 0,               1.0, 0.0},
    {0 + size.width, 0 + size.height, 1.0, 1.0},
  };

  glBindBuffer(GL_ARRAY_BUFFER, ui_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
  set_bool(color_chooser_shader, "isSlider", false);
}

void draw_color_picker_canvas(UIConstraints *constraints, f32 slider_percentage, Alignment align) {
  use_shader(color_chooser_shader);

  set_bool(color_chooser_shader, "isSlider", false);
  set_float(color_chooser_shader, "sliderPercentage", slider_percentage);
  set_mat4f(color_chooser_shader, "projection", (float *)zephr_ctx->projection.m);

  Vec2f pos = { 0.f, 0.f };
  Sizef size = { 0.f, 0.f };

  apply_constraints(constraints, &pos, &size);
  apply_alignment(align, constraints, &pos, size);

  // set the positions after applying alignment so children can use them
  constraints->x = pos.x;
  constraints->y = pos.y;

  Matrix4x4 model = identity();
  apply_translation(&model, pos);

  set_mat4f(color_chooser_shader, "model", (float *)model.m);

  glBindVertexArray(ui_vao);

  float vertices[6][4] = {
    // bottom left tri
    {0,              0 + size.height, 0.0, 1.0},
    {0,              0,               0.0, 0.0},
    {0 + size.width, 0,               1.0, 0.0},

    // top right tri
    {0,              0 + size.height, 0.0, 1.0},
    {0 + size.width, 0,               1.0, 0.0},
    {0 + size.width, 0 + size.height, 1.0, 1.0},
  };

  glBindBuffer(GL_ARRAY_BUFFER, ui_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
}

// TODO: consume mouse events on the popup
void draw_color_picker_popup(UIConstraints *picker_button_con) {
  UIConstraints popup_con = default_constraints;
  set_parent_constraint(&popup_con, picker_button_con);
  set_x_constraint(&popup_con, 0, UI_CONSTRAINT_FIXED);
  set_y_constraint(&popup_con, picker_button_con->height + 20, UI_CONSTRAINT_FIXED);
  set_width_constraint(&popup_con, 450, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&popup_con, 480, UI_CONSTRAINT_RELATIVE_PIXELS);
  draw_quad(&popup_con, mult_color(COLOR_WHITE, 0.94f), 8, ALIGN_TOP_LEFT);

  UIConstraints color_slider_con = default_constraints;
  set_parent_constraint(&color_slider_con, &popup_con);
  set_x_constraint(&color_slider_con, -0.04f, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&color_slider_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&color_slider_con, 400 * 0.9f, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_width_constraint(&color_slider_con, 0.1f, UI_CONSTRAINT_ASPECT_RATIO);
  draw_color_picker_slider(&color_slider_con, ALIGN_TOP_RIGHT);

  static f32 slider_selection = 0;
  static Vec2f canvas_pos = {0};

  if (inside_rect(&(Rect){{color_slider_con.x, color_slider_con.y}, {color_slider_con.width, color_slider_con.height}}, &zephr_ctx->mouse.pos) &&
      zephr_ctx->mouse.pressed && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT) {
    slider_selection = (zephr_ctx->mouse.pos.y - color_slider_con.y) / color_slider_con.height;
  }

  UIConstraints triangle_con = default_constraints;
  set_parent_constraint(&triangle_con, &color_slider_con);
  set_x_constraint(&triangle_con, -11, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_y_constraint(&triangle_con, slider_selection, UI_CONSTRAINT_RELATIVE);
  set_rotation_constraint(&triangle_con, -90);
  set_width_constraint(&triangle_con, 14, UI_CONSTRAINT_RELATIVE_PIXELS);
  set_height_constraint(&triangle_con, 0.5f, UI_CONSTRAINT_ASPECT_RATIO);
  triangle_con.y += -(triangle_con.height / 2.f);
  draw_triangle(&triangle_con, COLOR_BLACK, ALIGN_TOP_LEFT);

  set_x_constraint(&triangle_con, 1, UI_CONSTRAINT_RELATIVE);
  triangle_con.x -= triangle_con.height / 2;
  set_rotation_constraint(&triangle_con, 90);
  draw_triangle(&triangle_con, COLOR_BLACK, ALIGN_TOP_LEFT);

  set_x_constraint(&color_slider_con, 0.04f, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&color_slider_con, 0.05f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&color_slider_con, popup_con.width * 0.8f, UI_CONSTRAINT_FIXED);
  draw_color_picker_canvas(&color_slider_con, slider_selection, ALIGN_TOP_LEFT);

  if (inside_rect(&(Rect){{color_slider_con.x, color_slider_con.y}, {color_slider_con.width, color_slider_con.height}}, &zephr_ctx->mouse.pos) &&
      zephr_ctx->mouse.pressed && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT) {
    f32 x = (zephr_ctx->mouse.pos.x - color_slider_con.x) / color_slider_con.width;
    f32 y = (zephr_ctx->mouse.pos.y - color_slider_con.y) / color_slider_con.height;

    canvas_pos.x = x;
    canvas_pos.y = y;
  }

  set_parent_constraint(&triangle_con, &color_slider_con);
  set_x_constraint(&triangle_con, canvas_pos.x, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&triangle_con, canvas_pos.y, UI_CONSTRAINT_RELATIVE);
  triangle_con.x -= triangle_con.height;
  triangle_con.y -= triangle_con.height * 2;
  set_rotation_constraint(&triangle_con, 0);
  draw_triangle(&triangle_con, COLOR_WHITE, ALIGN_TOP_LEFT);

  triangle_con.x += triangle_con.height * 2;
  triangle_con.y += triangle_con.height * 2;
  set_rotation_constraint(&triangle_con, 90);
  draw_triangle(&triangle_con, COLOR_WHITE, ALIGN_TOP_LEFT);

  triangle_con.x -= triangle_con.height * 2;
  triangle_con.y += triangle_con.height * 2;
  set_rotation_constraint(&triangle_con, 180);
  draw_triangle(&triangle_con, COLOR_WHITE, ALIGN_TOP_LEFT);

  triangle_con.x -= triangle_con.height * 2;
  triangle_con.y -= triangle_con.height * 2;
  set_rotation_constraint(&triangle_con, 270);
  draw_triangle(&triangle_con, COLOR_WHITE, ALIGN_TOP_LEFT);

  UIConstraints button_con = default_constraints;
  set_parent_constraint(&button_con, &popup_con);
  set_x_constraint(&button_con, 0.04f, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&button_con, -0.05f, UI_CONSTRAINT_RELATIVE);
  set_width_constraint(&button_con, 0.43f, UI_CONSTRAINT_RELATIVE);
  set_height_constraint(&button_con, 0.3f, UI_CONSTRAINT_ASPECT_RATIO);
  if (draw_button(&button_con, *zephr_ctx->ui.popup_revert_color, "Revert", 4, ALIGN_BOTTOM_LEFT, BUTTON_STATE_ACTIVE)) {
    zephr_ctx->ui.popup_open = false;
  }

  Color selected_color = hsv2rgb(slider_selection * 360.f, canvas_pos.x, 1.0f - canvas_pos.y);

  set_x_constraint(&button_con, -0.04f, UI_CONSTRAINT_RELATIVE);
  set_y_constraint(&button_con, -0.05f, UI_CONSTRAINT_RELATIVE);
  if (draw_button(&button_con, selected_color, "Apply", 4, ALIGN_BOTTOM_RIGHT, BUTTON_STATE_ACTIVE)) {
    *zephr_ctx->ui.popup_revert_color = selected_color;
    zephr_ctx->ui.popup_open = false;
  }

  zephr_ctx->ui.popup_rect = (Rect){{popup_con.x, popup_con.y}, {popup_con.width, popup_con.height}};
}

void draw_color_picker(UIConstraints *constraints, Color *color, Alignment align, ButtonState state) {
  Rect rect = {0};
  Color display_color = *color;

  UIConstraints con = *constraints;

  apply_constraints(&con, &rect.pos, &rect.size);
  apply_alignment(align, &con, &rect.pos, rect.size);

  bool is_hovered = inside_rect(&rect, &zephr_ctx->mouse.pos);
  bool left_mouse_pressed = zephr_ctx->mouse.pressed && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT;

  if (is_hovered && state == BUTTON_STATE_ACTIVE) {
    zephr_set_cursor(ZEPHR_CURSOR_HAND);
    con.scale.height *= 1.08f;
    con.scale.width *= 1.08f;

    if (left_mouse_pressed) {
      display_color = mult_color(display_color, 0.8f);
    } else {
      display_color = mult_color(display_color, 0.9f);
    }
  } else if (state == BUTTON_STATE_DISABLED) {
    if (is_hovered) {
      zephr_set_cursor(ZEPHR_CURSOR_DISABLED);
    }

    display_color.a = 100;
  }

  draw_quad(&con, display_color, con.width * 0.08f, align);

  if (is_hovered && state == BUTTON_STATE_ACTIVE && zephr_ctx->mouse.released && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT) {
    zephr_ctx->ui.popup_open = true;
    zephr_ctx->ui.popup_parent_constraints = con;
    zephr_ctx->ui.popup_revert_color = color;
    /* zephr_ctx->ui.popup_rect = (Rect){rect.pos, rect.size}; */
    /* draw_color_picker_overlay(constraints); */
  }

  /* if (is_hovered && state == BUTTON_STATE_ACTIVE && zephr_ctx->mouse.released && zephr_ctx->mouse.button == ZEPHR_MOUSE_BUTTON_LEFT) { */
  /*   return true; */
  /* } */

  /* return false; */
}


/* enum Element { */
/*   Rect, */
/*   Circle, */
/* }; */

/* struct UIElement { */
/*   Vec2 pos; */
/*   Size size; */
/*   Color *color; */
/*   enum Element type; */
/* }; */

/* struct UIText { */
/*   Vec2 pos; */
/*   float scale; */
/*   Color *color; */
/*   const char* text; */
/* }; */

/* struct UIElement *create_element(Vec2 pos, Size size, Color *color, enum Element type) { */
/*   struct UIElement *element = malloc(sizeof(struct UIElement)); */
/*   element->pos = pos; */
/*   element->size = size; */
/*   element->color = color; */
/*   element->type = type; */
/*   return element; */
/* } */

