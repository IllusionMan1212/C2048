#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <X11/XKBlib.h>

#include "text.h"
#include "zephr_math.h"

#define COLOR_BLACK (Color){0, 0, 0, 255}
#define COLOR_WHITE (Color){255, 255, 255, 255}
#define COLOR_RED (Color){255, 0, 0, 255}
#define COLOR_GREEN (Color){0, 255, 0, 255}
#define COLOR_BLUE (Color){0, 0, 255, 255}
#define COLOR_YELLOW (Color){255, 255, 0, 255}
#define COLOR_MAGENTA (Color){255, 0, 255, 255}
#define COLOR_CYAN (Color){0, 255, 255, 255}
#define COLOR_ORANGE (Color){255, 128, 0, 255}
#define COLOR_PURPLE (Color){128, 0, 255, 255}

typedef struct ZephrWindow {
  Size size;
  bool is_fullscreen;
} ZephrWindow;

typedef enum ZephrEventType {
  ZEPHR_EVENT_UNKNOWN,
  ZEPHR_EVENT_KEY_PRESSED,
  ZEPHR_EVENT_KEY_RELEASED,
  ZEPHR_EVENT_MOUSE_BUTTON_PRESSED,
  ZEPHR_EVENT_MOUSE_BUTTON_RELEASED,
  ZEPHR_EVENT_MOUSE_SCROLL,
  ZEPHR_EVENT_WINDOW_RESIZED,
  ZEPHR_EVENT_WINDOW_CLOSED,
  ZEPHR_EVENT_MOUSE_MOVED,
} ZephrEventType;

typedef enum ZephrMouseScrollDirection {
  ZEPHR_MOUSE_SCROLL_UP,
  ZEPHR_MOUSE_SCROLL_DOWN,
} ZephrMouseScrollDirection;

typedef enum ZephrMouseButton {
  ZEPHR_MOUSE_BUTTON_LEFT,
  ZEPHR_MOUSE_BUTTON_RIGHT,
  ZEPHR_MOUSE_BUTTON_MIDDLE,
  ZEPHR_MOUSE_BUTTON_BACK,
  ZEPHR_MOUSE_BUTTON_FORWARD,
  ZEPHR_MOUSE_BUTTON_3,
  ZEPHR_MOUSE_BUTTON_4,
  ZEPHR_MOUSE_BUTTON_5,
  ZEPHR_MOUSE_BUTTON_6,
  ZEPHR_MOUSE_BUTTON_7,
} ZephrMouseButton;

typedef enum ZephrCursor {
  ZEPHR_CURSOR_ARROW,
  ZEPHR_CURSOR_IBEAM,
  ZEPHR_CURSOR_CROSSHAIR,
  ZEPHR_CURSOR_HAND,
  ZEPHR_CURSOR_HRESIZE,
  ZEPHR_CURSOR_VRESIZE,

  ZEPHR_CURSOR_COUNT,
} ZephrCursor;


typedef u16 ZephrScancode;
typedef u16 ZephrKeycode;

// these are actually scancodes as documented by the USB HID Usage Tables
// https://usb.org/sites/default/files/documents/hut1_12v2.pdf
enum {
  ZEPHR_KEYCODE_NULL = 0,

  ZEPHR_KEYCODE_A = 4,
  ZEPHR_KEYCODE_B = 5,
  ZEPHR_KEYCODE_C = 6,
  ZEPHR_KEYCODE_D = 7,
  ZEPHR_KEYCODE_E = 8,
  ZEPHR_KEYCODE_F = 9,
  ZEPHR_KEYCODE_G = 10,
  ZEPHR_KEYCODE_H = 11,
  ZEPHR_KEYCODE_I = 12,
  ZEPHR_KEYCODE_J = 13,
  ZEPHR_KEYCODE_K = 14,
  ZEPHR_KEYCODE_L = 15,
  ZEPHR_KEYCODE_M = 16,
  ZEPHR_KEYCODE_N = 17,
  ZEPHR_KEYCODE_O = 18,
  ZEPHR_KEYCODE_P = 19,
  ZEPHR_KEYCODE_Q = 20,
  ZEPHR_KEYCODE_R = 21,
  ZEPHR_KEYCODE_S = 22,
  ZEPHR_KEYCODE_T = 23,
  ZEPHR_KEYCODE_U = 24,
  ZEPHR_KEYCODE_V = 25,
  ZEPHR_KEYCODE_W = 26,
  ZEPHR_KEYCODE_X = 27,
  ZEPHR_KEYCODE_Y = 28,
  ZEPHR_KEYCODE_Z = 29,

  ZEPHR_KEYCODE_1 = 30,
  ZEPHR_KEYCODE_2 = 31,
  ZEPHR_KEYCODE_3 = 32,
  ZEPHR_KEYCODE_4 = 33,
  ZEPHR_KEYCODE_5 = 34,
  ZEPHR_KEYCODE_6 = 35,
  ZEPHR_KEYCODE_7 = 36,
  ZEPHR_KEYCODE_8 = 37,
  ZEPHR_KEYCODE_9 = 38,
  ZEPHR_KEYCODE_0 = 39,

  ZEPHR_KEYCODE_ENTER = 40,
  ZEPHR_KEYCODE_ESCAPE = 41,
  ZEPHR_KEYCODE_BACKSPACE = 42,
  ZEPHR_KEYCODE_TAB = 43,
  ZEPHR_KEYCODE_SPACE = 44,

  ZEPHR_KEYCODE_MINUS = 45,
  ZEPHR_KEYCODE_EQUALS = 46,
  ZEPHR_KEYCODE_LEFT_BRACKET = 47,
  ZEPHR_KEYCODE_RIGHT_BRACKET = 48,
  ZEPHR_KEYCODE_BACKSLASH = 49,
  ZEPHR_KEYCODE_NON_US_HASH = 50,
  ZEPHR_KEYCODE_SEMICOLON = 51,
  ZEPHR_KEYCODE_APOSTROPHE = 52,
  ZEPHR_KEYCODE_GRAVE = 53,
  ZEPHR_KEYCODE_COMMA = 54,
  ZEPHR_KEYCODE_PERIOD = 55,
  ZEPHR_KEYCODE_SLASH = 56,

  ZEPHR_KEYCODE_CAPS_LOCK = 57,

  ZEPHR_KEYCODE_F1 = 58,
  ZEPHR_KEYCODE_F2 = 59,
  ZEPHR_KEYCODE_F3 = 60,
  ZEPHR_KEYCODE_F4 = 61,
  ZEPHR_KEYCODE_F5 = 62,
  ZEPHR_KEYCODE_F6 = 63,
  ZEPHR_KEYCODE_F7 = 64,
  ZEPHR_KEYCODE_F8 = 65,
  ZEPHR_KEYCODE_F9 = 66,
  ZEPHR_KEYCODE_F10 = 67,
  ZEPHR_KEYCODE_F11 = 68,
  ZEPHR_KEYCODE_F12 = 69,

  ZEPHR_KEYCODE_PRINT_SCREEN = 70,
  ZEPHR_KEYCODE_SCROLL_LOCK = 71,
  ZEPHR_KEYCODE_PAUSE = 72,
  ZEPHR_KEYCODE_INSERT = 73,
  ZEPHR_KEYCODE_HOME = 74,
  ZEPHR_KEYCODE_PAGE_UP = 75,
  ZEPHR_KEYCODE_DELETE = 76,
  ZEPHR_KEYCODE_END = 77,
  ZEPHR_KEYCODE_PAGE_DOWN = 78,
  ZEPHR_KEYCODE_RIGHT = 79,
  ZEPHR_KEYCODE_LEFT = 80,
  ZEPHR_KEYCODE_DOWN = 81,
  ZEPHR_KEYCODE_UP = 82,

  ZEPHR_KEYCODE_NUM_LOCK_OR_CLEAR = 83,
  ZEPHR_KEYCODE_KP_DIVIDE = 84,
  ZEPHR_KEYCODE_KP_MULTIPLY = 85,
  ZEPHR_KEYCODE_KP_MINUS = 86,
  ZEPHR_KEYCODE_KP_PLUS = 87,
  ZEPHR_KEYCODE_KP_ENTER = 88,
  ZEPHR_KEYCODE_KP_1 = 89,
  ZEPHR_KEYCODE_KP_2 = 90,
  ZEPHR_KEYCODE_KP_3 = 91,
  ZEPHR_KEYCODE_KP_4 = 92,
  ZEPHR_KEYCODE_KP_5 = 93,
  ZEPHR_KEYCODE_KP_6 = 94,
  ZEPHR_KEYCODE_KP_7 = 95,
  ZEPHR_KEYCODE_KP_8 = 96,
  ZEPHR_KEYCODE_KP_9 = 97,
  ZEPHR_KEYCODE_KP_0 = 98,
  ZEPHR_KEYCODE_KP_PERIOD = 99,

  ZEPHR_KEYCODE_NON_US_BACKSLASH = 100,
  ZEPHR_KEYCODE_APPLICATION = 101,
  ZEPHR_KEYCODE_POWER = 102,
  ZEPHR_KEYCODE_KP_EQUALS = 103,
  ZEPHR_KEYCODE_F13 = 104,
  ZEPHR_KEYCODE_F14 = 105,
  ZEPHR_KEYCODE_F15 = 106,
  ZEPHR_KEYCODE_F16 = 107,
  ZEPHR_KEYCODE_F17 = 108,
  ZEPHR_KEYCODE_F18 = 109,
  ZEPHR_KEYCODE_F19 = 110,
  ZEPHR_KEYCODE_F20 = 111,
  ZEPHR_KEYCODE_F21 = 112,
  ZEPHR_KEYCODE_F22 = 113,
  ZEPHR_KEYCODE_F23 = 114,
  ZEPHR_KEYCODE_F24 = 115,
  ZEPHR_KEYCODE_EXECUTE = 116,
  ZEPHR_KEYCODE_HELP = 117,
  ZEPHR_KEYCODE_MENU = 118,
  ZEPHR_KEYCODE_SELECT = 119,
  ZEPHR_KEYCODE_STOP = 120,
  ZEPHR_KEYCODE_AGAIN = 121,
  ZEPHR_KEYCODE_UNDO = 122,
  ZEPHR_KEYCODE_CUT = 123,
  ZEPHR_KEYCODE_COPY = 124,
  ZEPHR_KEYCODE_PASTE = 125,
  ZEPHR_KEYCODE_FIND = 126,
  ZEPHR_KEYCODE_MUTE = 127,
  ZEPHR_KEYCODE_VOLUME_UP = 128,
  ZEPHR_KEYCODE_VOLUME_DOWN = 129,
  ZEPHR_KEYCODE_KP_COMMA = 133,
  ZEPHR_KEYCODE_KP_EQUALSAS400 = 134,

  ZEPHR_KEYCODE_INTERNATIONAL1 = 135,
  ZEPHR_KEYCODE_INTERNATIONAL2 = 136,
  ZEPHR_KEYCODE_INTERNATIONAL3 = 137,
  ZEPHR_KEYCODE_INTERNATIONAL4 = 138,
  ZEPHR_KEYCODE_INTERNATIONAL5 = 139,
  ZEPHR_KEYCODE_INTERNATIONAL6 = 140,
  ZEPHR_KEYCODE_INTERNATIONAL7 = 141,
  ZEPHR_KEYCODE_INTERNATIONAL8 = 142,
  ZEPHR_KEYCODE_INTERNATIONAL9 = 143,
  ZEPHR_KEYCODE_LANG1 = 144,
  ZEPHR_KEYCODE_LANG2 = 145,
  ZEPHR_KEYCODE_LANG3 = 146,
  ZEPHR_KEYCODE_LANG4 = 147,
  ZEPHR_KEYCODE_LANG5 = 148,
  ZEPHR_KEYCODE_LANG6 = 149,
  ZEPHR_KEYCODE_LANG7 = 150,
  ZEPHR_KEYCODE_LANG8 = 151,
  ZEPHR_KEYCODE_LANG9 = 152,

  ZEPHR_KEYCODE_ALT_ERASE = 153,
  ZEPHR_KEYCODE_SYSREQ = 154,
  ZEPHR_KEYCODE_CANCEL = 155,
  ZEPHR_KEYCODE_CLEAR = 156,
  ZEPHR_KEYCODE_PRIOR = 157,
  ZEPHR_KEYCODE_ENTER_2 = 158,
  ZEPHR_KEYCODE_SEPARATOR = 159,
  ZEPHR_KEYCODE_OUT = 160,
  ZEPHR_KEYCODE_OPER = 161,
  ZEPHR_KEYCODE_CLEARAGAIN = 162,
  ZEPHR_KEYCODE_CRSEL = 163,
  ZEPHR_KEYCODE_EXSEL = 164,

  ZEPHR_KEYCODE_KP_00 = 176,
  ZEPHR_KEYCODE_KP_000 = 177,
  ZEPHR_KEYCODE_THOUSANDSSEPARATOR = 178,
  ZEPHR_KEYCODE_DECIMALSEPARATOR = 179,
  ZEPHR_KEYCODE_CURRENCYUNIT = 180,
  ZEPHR_KEYCODE_CURRENCYSUBUNIT = 181,
  ZEPHR_KEYCODE_KP_LEFT_PAREN = 182,
  ZEPHR_KEYCODE_KP_RIGHT_PAREN = 183,
  ZEPHR_KEYCODE_KP_LEFT_BRACE = 184,
  ZEPHR_KEYCODE_KP_RIGHT_BRACE = 185,
  ZEPHR_KEYCODE_KP_TAB = 186,
  ZEPHR_KEYCODE_KP_BACKSPACE = 187,
  ZEPHR_KEYCODE_KP_A = 188,
  ZEPHR_KEYCODE_KP_B = 189,
  ZEPHR_KEYCODE_KP_C = 190,
  ZEPHR_KEYCODE_KP_D = 191,
  ZEPHR_KEYCODE_KP_E = 192,
  ZEPHR_KEYCODE_KP_F = 193,
  ZEPHR_KEYCODE_KP_XOR = 194,
  ZEPHR_KEYCODE_KP_POWER = 195,
  ZEPHR_KEYCODE_KP_PERCENT = 196,
  ZEPHR_KEYCODE_KP_LESS = 197,
  ZEPHR_KEYCODE_KP_GREATER = 198,
  ZEPHR_KEYCODE_KP_AMPERSAND = 199,
  ZEPHR_KEYCODE_KP_DBLAMPERSAND = 200,
  ZEPHR_KEYCODE_KP_VERTICALBAR = 201,
  ZEPHR_KEYCODE_KP_DBLVERTICALBAR = 202,
  ZEPHR_KEYCODE_KP_COLON = 203,
  ZEPHR_KEYCODE_KP_HASH = 204,
  ZEPHR_KEYCODE_KP_SPACE = 205,
  ZEPHR_KEYCODE_KP_AT = 206,
  ZEPHR_KEYCODE_KP_EXCLAM = 207,
  ZEPHR_KEYCODE_KP_MEMSTORE = 208,
  ZEPHR_KEYCODE_KP_MEMRECALL = 209,
  ZEPHR_KEYCODE_KP_MEMCLEAR = 210,
  ZEPHR_KEYCODE_KP_MEMADD = 211,
  ZEPHR_KEYCODE_KP_MEMSUBTRACT = 212,
  ZEPHR_KEYCODE_KP_MEMMULTIPLY = 213,
  ZEPHR_KEYCODE_KP_MEMDIVIDE = 214,
  ZEPHR_KEYCODE_KP_PLUS_MINUS = 215,
  ZEPHR_KEYCODE_KP_CLEAR = 216,
  ZEPHR_KEYCODE_KP_CLEARENTRY = 217,
  ZEPHR_KEYCODE_KP_BINARY = 218,
  ZEPHR_KEYCODE_KP_OCTAL = 219,
  ZEPHR_KEYCODE_KP_DECIMAL = 220,
  ZEPHR_KEYCODE_KP_HEXADECIMAL = 221,

  ZEPHR_KEYCODE_LEFT_CTRL = 224,
  ZEPHR_KEYCODE_LEFT_SHIFT = 225,
  ZEPHR_KEYCODE_LEFT_ALT = 226,
  ZEPHR_KEYCODE_LEFT_META = 227,
  ZEPHR_KEYCODE_RIGHT_CTRL = 228,
  ZEPHR_KEYCODE_RIGHT_SHIFT = 229,
  ZEPHR_KEYCODE_RIGHT_ALT = 230,
  ZEPHR_KEYCODE_RIGHT_META = 231,

  /** Not a key. Marks the number of scancodes. */
  ZEPHR_KEYCODE_COUNT = 512,
};

typedef enum ZephrKeyMod {
	ZEPHR_KEY_MOD_NONE =        0x0000,

	ZEPHR_KEY_MOD_LEFT_SHIFT =  0x0001,
	ZEPHR_KEY_MOD_RIGHT_SHIFT = 0x0002,
	ZEPHR_KEY_MOD_SHIFT =       0x0003,

	ZEPHR_KEY_MOD_LEFT_CTRL =   0x0040,
	ZEPHR_KEY_MOD_RIGHT_CTRL =  0x0080,
	ZEPHR_KEY_MOD_CTRL =        0x00C0,

	ZEPHR_KEY_MOD_LEFT_ALT =    0x0100,
	ZEPHR_KEY_MOD_RIGHT_ALT =   0x0200,
	ZEPHR_KEY_MOD_ALT =         0x0300,

	ZEPHR_KEY_MOD_LEFT_META =   0x0400,
	ZEPHR_KEY_MOD_RIGHT_META =  0x0800,
	ZEPHR_KEY_MOD_META =        0x0C00,

	ZEPHR_KEY_MOD_CAPS_LOCK =   0x1000,
	ZEPHR_KEY_MOD_NUM_LOCK =    0x2000,
} ZephrKeyMod;

typedef struct ZephrKeyboard {
  ZephrKeycode scancode_to_keycode[ZEPHR_KEYCODE_COUNT];
  ZephrScancode keycode_to_scancode[ZEPHR_KEYCODE_COUNT];
  /* u64 scancode_is_pressed_bitset[CORE_DIV_ROUND_UP(ZEPHR_KEYCODE_COUNT, 64)]; */
  /* u64 scancode_has_been_pressed_bitset[CORE_DIV_ROUND_UP(ZEPHR_KEYCODE_COUNT, 64)]; */
  /* u64 scancode_has_been_released_bitset[CORE_DIV_ROUND_UP(ZEPHR_KEYCODE_COUNT, 64)]; */
  /* u64 keycode_is_pressed_bitset[CORE_DIV_ROUND_UP(ZEPHR_KEYCODE_COUNT, 64)]; */
  /* u64 keycode_has_been_pressed_bitset[CORE_DIV_ROUND_UP(ZEPHR_KEYCODE_COUNT, 64)]; */
  /* u64 keycode_has_been_released_bitset[CORE_DIV_ROUND_UP(ZEPHR_KEYCODE_COUNT, 64)]; */
} ZephrKeyboard;

typedef struct ZephrEvent {
  ZephrEventType type;

  union {
    struct {
      bool is_pressed;
      bool is_repeat;
      ZephrScancode scancode;
      ZephrKeycode code;
      ZephrKeyMod mods;
    } key;

    struct {
      ZephrMouseButton button;
      Vec2 position;
      ZephrMouseScrollDirection scroll_direction;
    } mouse;

    struct {
      u32 width;
      u32 height;
    } window;
  };
} ZephrEvent;
/* CORE_DEFINE_STACK(ZephrEvent); */

typedef struct ZephrMouse {
  Vec2 pos;
  bool pressed;
  bool released;
  ZephrMouseButton button;
} ZephrMouse;

typedef struct ZephrContext {
  Atom window_delete_atom;
  bool should_quit;
  Size screen_size;
  ZephrWindow window;
  ZephrFont font;
  ZephrMouse mouse;
  ZephrCursor cursor;
  ZephrCursor cursors[ZEPHR_CURSOR_COUNT];
  /* ZephrKeyboard keyboard; */
  /* XkbDescPtr xkb; */
  /* XIM xim; */
  /* CoreStack(ZephrEvent) event_queue; */
  /* u32 event_queue_cursor; */

  Matrix4x4 projection;
} ZephrContext;

void zephr_init(const char* font_path, const char* icon_path, const char* window_title, Size window_size);
void zephr_deinit(void);
bool zephr_should_quit(void);
void zephr_swap_buffers(void);
Size zephr_get_window_size(void);
void zephr_make_window_non_resizable(void);
void zephr_toggle_fullscreen(void);
void zephr_quit(void);
Color ColorRGBA(u8 r, u8 g, u8 b, u8 a);
bool zephr_iter_events(ZephrEvent *event_out);
void zephr_set_cursor(ZephrCursor cursor);

/* bool zephr_keyboard_keycode_is_pressed(ZephrKeycode keycode); */
/* bool zephr_keyboard_keycode_has_been_pressed(ZephrKeycode keycode); */
/* bool zephr_keyboard_keycode_has_been_released(ZephrKeycode keycode); */

/* ZephrScancode zephr_keyboard_keycode_to_scancode(ZephrKeycode keycode); */
/* ZephrKeycode zephr_keyboard_scancode_to_keycode(ZephrScancode scancode); */

extern ZephrContext *zephr_ctx;
