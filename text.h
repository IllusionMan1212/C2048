#pragma once

#include "core.h"
#include "ui.h"
#include "zephr_math.h"

typedef struct Character {
  Size size;
  Size bearing;
  unsigned int advance;
  Vec2f tex_coords[4]; // the 4 corners of the character quad
} Character;

typedef struct ZephrFont {
  Character characters[128];
  unsigned int atlas_texture_id;
} ZephrFont;

typedef struct TextInstance {
  Vec4f position;
  int tex_coords_index;
  Colorf color;
  float model[4][4];
} GlyphInstance;

typedef struct GlyphInstanceList {
  GlyphInstance *data;
  int size;
  int capacity;
} GlyphInstanceList;

void new_glyph_instance_list(GlyphInstanceList *list, u32 capacity);
int init_fonts(const char *font_path);
Sizef calculate_text_size(const char *text, int font_size);
void draw_text(const char* text, int font_size, UIConstraints constraints, const Color color, Alignment alignment);
void add_text_instance(GlyphInstanceList *batch, const char* text, int font_size, UIConstraints constraints, const Color color, Alignment alignment);
void draw_text_batch(GlyphInstanceList *batch);
