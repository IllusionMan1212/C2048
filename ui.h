#pragma once

#include "texture.h"
#include "zephr_math.h"

typedef struct Rect {
  Vec2f pos;
  Sizef size;
} Rect;

typedef enum Alignment {
  ALIGN_TOP_LEFT,
  ALIGN_TOP_CENTER,
  ALIGN_TOP_RIGHT,
  ALIGN_LEFT_CENTER,
  ALIGN_CENTER,
  ALIGN_RIGHT_CENTER,
  ALIGN_BOTTOM_LEFT,
  ALIGN_BOTTOM_CENTER,
  ALIGN_BOTTOM_RIGHT,
} Alignment;

typedef enum UIConstraint {
  UI_CONSTRAINT_FIXED,
  UI_CONSTRAINT_RELATIVE,
  UI_CONSTRAINT_RELATIVE_PIXELS,
  UI_CONSTRAINT_ASPECT_RATIO,
} UIConstraint;

typedef struct UIConstraints {
  float x;
  float y;
  float width;
  float height;
  float rotation;
  struct UIConstraints *parent;
} UIConstraints;

typedef enum ButtonState {
  BUTTON_STATE_ACTIVE,
  BUTTON_STATE_INACTIVE,
  BUTTON_STATE_DISABLED,
} ButtonState;

int init_ui(const char* font_path);
void set_parent_constraint(UIConstraints *constraints, UIConstraints *parent_constraints);
void set_x_constraint(UIConstraints *constraints, float value, UIConstraint type);
void set_y_constraint(UIConstraints *constraints, float value, UIConstraint type);
void set_width_constraint(UIConstraints *constraints, float value, UIConstraint type);
void set_height_constraint(UIConstraints *constraints, float value, UIConstraint type);
void set_rotation_constraint(UIConstraints *constraints, float angle_d);
void apply_constraints(UIConstraints *constraints, Vec2f *pos, Sizef *size);
void apply_alignment(Alignment align, UIConstraints *constraints, Vec2f *pos, Sizef size);
void draw_quad(UIConstraints *constraints, const Color color, float border_radius, Alignment align);
void draw_circle(UIConstraints *constraints, const Color color, Alignment align);
void draw_triangle(UIConstraints *constraints, const Color color, Alignment align);
bool draw_button(UIConstraints *constraints, Color color, const char *text, f32 radius, Alignment align, ButtonState state);
bool draw_icon_button(UIConstraints *constraints, Color color, const TextureId icon_tex_id, f32 radius, Alignment align, ButtonState state);
