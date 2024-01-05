#pragma once

#include "core.h"

typedef struct {
    float m[4][4];
} Matrix4x4;

typedef struct Size {
  int width;
  int height;
} Size;

typedef struct Sizef {
  float width;
  float height;
} Sizef;

typedef struct Vec2 {
  int x;
  int y;
} Vec2;

typedef struct Vec2f {
  float x;
  float y;
} Vec2f;

typedef struct Vec4f {
  float x;
  float y;
  float z;
  float w;
} Vec4f;

typedef struct Color {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
} Color;

typedef struct Colorf {
  f32 r;
  f32 g;
  f32 b;
  f32 a;
} Colorf;

float to_radians(float degrees);
Matrix4x4 identity(void);
void mult_4x4(Matrix4x4 *multiplicant, Matrix4x4 *multiplier);
void apply_scale(Matrix4x4 *matrix, Sizef scale);
void apply_rotation(Matrix4x4 *model, float angle);
void apply_translation(Matrix4x4 *matrix, Vec2f pos);
Matrix4x4 orthographic_projection_2d(float left, float right, float bottom, float top);
Color mult_color(Color color, float amount);
Color hsv2rgb(float h, float s, float v);
