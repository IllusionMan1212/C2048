#include <math.h>

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

Color mul_color(Color color, float amount) {
  Color result = color;

  result.r = CORE_CLAMP((u8)((float)result.r * amount), 0, 255);
  result.g = CORE_CLAMP((u8)((float)result.g * amount), 0, 255);
  result.b = CORE_CLAMP((u8)((float)result.b * amount), 0, 255);

  return result;
}

