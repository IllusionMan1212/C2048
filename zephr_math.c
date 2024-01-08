#include <math.h>

#include "zephr.h"
#include "zephr_math.h"

float to_radians(float degrees) {
  return degrees * ((float)M_PI / 180.f);
}

Matrix4x4 orthographic_projection_2d(float left, float right, float bottom, float top) {
    Matrix4x4 result = identity();

    result.m[0][0] = 2.0f / (right - left);
    result.m[3][0] = -(right + left) / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[3][1] = -(top + bottom) / (top - bottom);
    result.m[2][2] = -1.0f;
    result.m[3][3] = 1.0f;

    return result;
}

Matrix4x4 identity(void) {
    Matrix4x4 result = {0};

    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;

    return result;
}

void mult_4x4(Matrix4x4 *multiplicant, Matrix4x4 *multiplier) {
  Matrix4x4 result = identity();

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result.m[i][j] = 0;
      for (int k = 0; k < 4; k++) {
        result.m[i][j] += multiplicant->m[i][k] * multiplier->m[k][j];
      }
    }
  }

  *multiplicant = result;
}

void apply_scale(Matrix4x4 *matrix, Sizef scale) {
  Matrix4x4 temp = identity();

  temp.m[0][0] = scale.width;
  temp.m[1][1] = scale.height;

  mult_4x4(matrix, &temp);
}

void apply_rotation(Matrix4x4 *model, float angle) {
  Matrix4x4 temp = identity();

  temp.m[0][0] = (float)cos(angle);
  temp.m[0][1] = (float)sin(angle);
  temp.m[1][0] = -(float)sin(angle);
  temp.m[1][1] = (float)cos(angle);

  mult_4x4(model, &temp);
}

void apply_translation(Matrix4x4 *matrix, Vec2f pos) {
  Matrix4x4 temp = identity();

  temp.m[3][0] = pos.x;
  temp.m[3][1] = pos.y;

  mult_4x4(matrix, &temp);
}

Color mult_color(Color color, float amount) {
  Color result = color;

  result.r = CORE_CLAMP((i32)((float)result.r * amount), 0, 255);
  result.g = CORE_CLAMP((i32)((float)result.g * amount), 0, 255);
  result.b = CORE_CLAMP((i32)((float)result.b * amount), 0, 255);

  return result;
}

Color hsv2rgb(float h, float s, float v) {
  float c = v * s;
  float x = (float)(c * (1 - fabs(fmod(h / 60.0, 2) - 1)));
  float m = v - c;

  float r, g, b;

  if (h >= 0 && h < 60) {
    r = c, g = x, b = 0;
  } else if (h >= 60 && h < 120) {
    r = x, g = c, b = 0;
  } else if (h >= 120 && h < 180) {
    r = 0, g = c, b = x;
  } else if (h >= 180 && h < 240) {
    r = 0, g = x, b = c;
  } else if (h >= 240 && h < 300) {
    r = x, g = 0, b = c;
  } else {
    r = c, g = 0, b = x;
  }

  return (Color){(u8)((r + m) * 255), (u8)((g + m) * 255), (u8)((b + m) * 255), 255};
}

f32 get_srgb(u8 c) {
  return (f32)(c / 255.f <= 0.03928f
      ? c / 255.f / 12.92f
      : pow((c / 255.f + 0.055f) / 1.055f, 2.4f));
}

f32 get_luminance(Color *color) {
  return ((0.2126f * (f32)get_srgb(color->r)) + (0.7152f * (f32)get_srgb(color->g)) + (0.0722f * (f32)get_srgb(color->b))) / 255.f;
}

f32 get_contrast(Color *fg, Color *bg) {
  f32 l1 = get_luminance(fg);
  f32 l2 = get_luminance(bg);

  return (CORE_MAX(l1, l2) + 0.05f) / (CORE_MIN(l1, l2) + 0.05f);
}

Color determine_color_contrast(Color *bg) {
  f32 white_contrast = get_contrast(bg, &COLOR_WHITE);
  f32 black_contrast = get_contrast(bg, &COLOR_BLACK);

  return white_contrast > black_contrast ? COLOR_WHITE : COLOR_BLACK;
}
