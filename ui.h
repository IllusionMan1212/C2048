#pragma once

#include "texture.h"
#include "zephr_math.h"

typedef u32 UiIdHash;

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
  Sizef scale;
  struct UIConstraints *parent;
} UIConstraints;

extern const UIConstraints default_constraints;

typedef struct Rect {
  Vec2f pos;
  Sizef size;
} Rect;

typedef struct UI {
  bool popup_open;
  Rect popup_rect;
  UIConstraints popup_parent_constraints;
  Color *popup_revert_color;
  UiIdHash active_element;
} UI;

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
bool inside_rect(Rect *rect, Vec2 *point);
void draw_quad(UIConstraints *constraints, const Color color, float border_radius, Alignment align);
void draw_circle(UIConstraints *constraints, const Color color, Alignment align);
void draw_triangle(UIConstraints *constraints, const Color color, Alignment align);
bool draw_button_with_location(const char* file, int line, UIConstraints *constraints, Color color, const char *text, f32 radius, Alignment align, ButtonState state);
bool draw_icon_button_with_location(const char* fie, int line, UIConstraints *constraints, Color color, const TextureId icon_tex_id, f32 radius, Alignment align, ButtonState state);
void draw_color_picker_popup(UIConstraints *picker_button_con);
void draw_color_picker(UIConstraints *constraints, Color *color, Alignment align, ButtonState state);

#define draw_button(constraints, color, text, radius, align, state) draw_button_with_location(__FILE__, __LINE__, constraints, color, text, radius, align, state)
#define draw_icon_button(constraints, color, icon_tex_id, radius, align, state) draw_icon_button_with_location(__FILE__, __LINE__, constraints, color, icon_tex_id, radius, align, state)
