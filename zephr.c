#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xutil.h>
#include <glad/glx.h>
#include <stb_image.h>

#include "audio.h"
#include "core.h"
#include "timer.h"
#include "ui.h"
#include "zephr.h"

Display *x11_display;
Window x11_window;
Colormap x11_colormap;
GLXContext glx_context;
/* XIC x11_xic; */

ZephrContext *zephr_ctx = NULL;

///////////////////////////
//
// Common
//
///////////////////////////

ZephrKeycode zephr_evdev_scancode_to_zephr_scancode_map[] = {
  ZEPHR_KEYCODE_NULL,
  ZEPHR_KEYCODE_ESCAPE,
  ZEPHR_KEYCODE_1,
  ZEPHR_KEYCODE_2,
  ZEPHR_KEYCODE_3,
  ZEPHR_KEYCODE_4,
  ZEPHR_KEYCODE_5,
  ZEPHR_KEYCODE_6,
  ZEPHR_KEYCODE_7,
  ZEPHR_KEYCODE_8,
  ZEPHR_KEYCODE_9,
  ZEPHR_KEYCODE_0,
  ZEPHR_KEYCODE_MINUS,
  ZEPHR_KEYCODE_EQUALS,
  ZEPHR_KEYCODE_BACKSPACE,
  ZEPHR_KEYCODE_TAB,
  ZEPHR_KEYCODE_Q,
  ZEPHR_KEYCODE_W,
  ZEPHR_KEYCODE_E,
  ZEPHR_KEYCODE_R,
  ZEPHR_KEYCODE_T,
  ZEPHR_KEYCODE_Y,
  ZEPHR_KEYCODE_U,
  ZEPHR_KEYCODE_I,
  ZEPHR_KEYCODE_O,
  ZEPHR_KEYCODE_P,
  ZEPHR_KEYCODE_LEFT_BRACKET,
  ZEPHR_KEYCODE_RIGHT_BRACKET,
  ZEPHR_KEYCODE_ENTER,
  ZEPHR_KEYCODE_LEFT_CTRL,
  ZEPHR_KEYCODE_A,
  ZEPHR_KEYCODE_S,
  ZEPHR_KEYCODE_D,
  ZEPHR_KEYCODE_F,
  ZEPHR_KEYCODE_G,
  ZEPHR_KEYCODE_H,
  ZEPHR_KEYCODE_J,
  ZEPHR_KEYCODE_K,
  ZEPHR_KEYCODE_L,
  ZEPHR_KEYCODE_SEMICOLON,
  ZEPHR_KEYCODE_APOSTROPHE,
  ZEPHR_KEYCODE_GRAVE,
  ZEPHR_KEYCODE_LEFT_SHIFT,
  ZEPHR_KEYCODE_BACKSLASH,
  ZEPHR_KEYCODE_Z,
  ZEPHR_KEYCODE_X,
  ZEPHR_KEYCODE_C,
  ZEPHR_KEYCODE_V,
  ZEPHR_KEYCODE_B,
  ZEPHR_KEYCODE_N,
  ZEPHR_KEYCODE_M,
  ZEPHR_KEYCODE_COMMA,
  ZEPHR_KEYCODE_PERIOD,
  ZEPHR_KEYCODE_SLASH,
  ZEPHR_KEYCODE_RIGHT_SHIFT,
  ZEPHR_KEYCODE_KP_MULTIPLY,
  ZEPHR_KEYCODE_LEFT_ALT,
  ZEPHR_KEYCODE_SPACE,
  ZEPHR_KEYCODE_CAPS_LOCK,
  ZEPHR_KEYCODE_F1,
  ZEPHR_KEYCODE_F2,
  ZEPHR_KEYCODE_F3,
  ZEPHR_KEYCODE_F4,
  ZEPHR_KEYCODE_F5,
  ZEPHR_KEYCODE_F6,
  ZEPHR_KEYCODE_F7,
  ZEPHR_KEYCODE_F8,
  ZEPHR_KEYCODE_F9,
  ZEPHR_KEYCODE_F10,
  ZEPHR_KEYCODE_NUM_LOCK_OR_CLEAR,
  ZEPHR_KEYCODE_SCROLL_LOCK,
  ZEPHR_KEYCODE_KP_7,
  ZEPHR_KEYCODE_KP_8,
  ZEPHR_KEYCODE_KP_9,
  ZEPHR_KEYCODE_KP_MINUS,
  ZEPHR_KEYCODE_KP_4,
  ZEPHR_KEYCODE_KP_5,
  ZEPHR_KEYCODE_KP_6,
  ZEPHR_KEYCODE_KP_PLUS,
  ZEPHR_KEYCODE_KP_1,
  ZEPHR_KEYCODE_KP_2,
  ZEPHR_KEYCODE_KP_3,
  ZEPHR_KEYCODE_KP_0,
  ZEPHR_KEYCODE_KP_PERIOD,
  [85] = ZEPHR_KEYCODE_LANG5, // KEY_ZENKAKUHANKAKU
  ZEPHR_KEYCODE_NON_US_BACKSLASH, // KEY_102ND
  ZEPHR_KEYCODE_F11,
  ZEPHR_KEYCODE_F12,
  ZEPHR_KEYCODE_INTERNATIONAL1, // KEY_RO,
  ZEPHR_KEYCODE_LANG3, // KEY_KATAKANA
  ZEPHR_KEYCODE_LANG4, // KEY_HIRAGANA
  ZEPHR_KEYCODE_INTERNATIONAL4, // KEY_HENKAN
  ZEPHR_KEYCODE_INTERNATIONAL2, // KEY_KATAKANAHIRAGANA
  ZEPHR_KEYCODE_INTERNATIONAL5, // KEY_MUHENKAN
  ZEPHR_KEYCODE_INTERNATIONAL5, // KEY_KPJOCOMMA
  ZEPHR_KEYCODE_KP_ENTER,
  ZEPHR_KEYCODE_RIGHT_CTRL,
  ZEPHR_KEYCODE_KP_DIVIDE,
  ZEPHR_KEYCODE_SYSREQ,
  ZEPHR_KEYCODE_RIGHT_ALT,
  ZEPHR_KEYCODE_NULL, // KEY_LINEFEED
  ZEPHR_KEYCODE_HOME,
  ZEPHR_KEYCODE_UP,
  ZEPHR_KEYCODE_PAGE_UP,
  ZEPHR_KEYCODE_LEFT,
  ZEPHR_KEYCODE_RIGHT,
  ZEPHR_KEYCODE_END,
  ZEPHR_KEYCODE_DOWN,
  ZEPHR_KEYCODE_PAGE_DOWN,
  ZEPHR_KEYCODE_INSERT,
  ZEPHR_KEYCODE_DELETE,
  ZEPHR_KEYCODE_NULL, // KEY_MACRO
  ZEPHR_KEYCODE_MUTE,
  ZEPHR_KEYCODE_VOLUME_DOWN,
  ZEPHR_KEYCODE_VOLUME_UP,
  ZEPHR_KEYCODE_POWER,
  ZEPHR_KEYCODE_KP_EQUALS,
  ZEPHR_KEYCODE_KP_PLUS_MINUS,
  ZEPHR_KEYCODE_PAUSE,
  [121] = ZEPHR_KEYCODE_KP_COMMA,
  ZEPHR_KEYCODE_LANG1, // KEY_HANGUEL
  ZEPHR_KEYCODE_LANG2, // KEY_HANJA
  ZEPHR_KEYCODE_INTERNATIONAL3, // KEY_YEN
  ZEPHR_KEYCODE_LEFT_META,
  ZEPHR_KEYCODE_RIGHT_META,
  ZEPHR_KEYCODE_APPLICATION, // KEY_COMPOSE
  ZEPHR_KEYCODE_STOP,
  ZEPHR_KEYCODE_AGAIN,
  ZEPHR_KEYCODE_NULL, // KEY_PROPS
  ZEPHR_KEYCODE_UNDO,
  ZEPHR_KEYCODE_NULL, // KEY_FRONT
  ZEPHR_KEYCODE_COPY,
  ZEPHR_KEYCODE_NULL, // KEY_OPEN
  ZEPHR_KEYCODE_PASTE,
  ZEPHR_KEYCODE_FIND,
  ZEPHR_KEYCODE_CUT,
  ZEPHR_KEYCODE_HELP,
  ZEPHR_KEYCODE_MENU,
  ZEPHR_KEYCODE_NULL, // CALCULATOR
  ZEPHR_KEYCODE_NULL, // KEY_SETUP
  ZEPHR_KEYCODE_NULL, // SLEEP
  ZEPHR_KEYCODE_NULL, // KEY_WAKEUP
  ZEPHR_KEYCODE_NULL, // KEY_FILE
  ZEPHR_KEYCODE_NULL, // KEY_SENDFILE
  ZEPHR_KEYCODE_NULL, // KEY_DELETEFILE
  ZEPHR_KEYCODE_NULL, // KEY_XFER
  ZEPHR_KEYCODE_NULL, // KEY_PROG1
  ZEPHR_KEYCODE_NULL, // KEY_PROG2
  ZEPHR_KEYCODE_NULL, // WWW
  ZEPHR_KEYCODE_NULL, // KEY_MSDOS
  ZEPHR_KEYCODE_NULL, // KEY_COFFEE
  ZEPHR_KEYCODE_NULL, // KEY_DIRECTION
  ZEPHR_KEYCODE_NULL, // KEY_CYCLEWINDOWS
  ZEPHR_KEYCODE_NULL, // MAIL
  ZEPHR_KEYCODE_NULL, // AC_BOOKMARKS
  ZEPHR_KEYCODE_NULL, // COMPUTER
  ZEPHR_KEYCODE_NULL, // AC_BACK
  ZEPHR_KEYCODE_NULL, // AC_FORWARD
  ZEPHR_KEYCODE_NULL, // KEY_CLOSECD
  ZEPHR_KEYCODE_NULL, // EJECT
  ZEPHR_KEYCODE_NULL, // KEY_EJECTCLOSECD
  ZEPHR_KEYCODE_NULL, // AUDIO_NEXT
  ZEPHR_KEYCODE_NULL, // AUDIO_PLAY
  ZEPHR_KEYCODE_NULL, // AUDIO_PREV
  ZEPHR_KEYCODE_NULL, // AUDIO_STOP
  ZEPHR_KEYCODE_NULL, // KEY_RECORD
  ZEPHR_KEYCODE_NULL, // AUDIO_REWIND
  ZEPHR_KEYCODE_NULL, // KEY_PHONE
  ZEPHR_KEYCODE_NULL, // KEY_ISO
  ZEPHR_KEYCODE_NULL, // KEY_CONFIG
  ZEPHR_KEYCODE_NULL, // AC_HOME
  ZEPHR_KEYCODE_NULL, // AC_REFRESH
  ZEPHR_KEYCODE_NULL, // KEY_EXIT
  ZEPHR_KEYCODE_NULL, // KEY_MOVE
  ZEPHR_KEYCODE_NULL, // KEY_EDIT
  ZEPHR_KEYCODE_NULL, // KEY_SCROLLUP
  ZEPHR_KEYCODE_NULL, // KEY_SCROLLDOWN
  ZEPHR_KEYCODE_KP_LEFT_PAREN,
  ZEPHR_KEYCODE_KP_RIGHT_PAREN,
  ZEPHR_KEYCODE_NULL, // KEY_NEW
  ZEPHR_KEYCODE_NULL, // KEY_REDO
  ZEPHR_KEYCODE_F13,
  ZEPHR_KEYCODE_F14,
  ZEPHR_KEYCODE_F15,
  ZEPHR_KEYCODE_F16,
  ZEPHR_KEYCODE_F17,
  ZEPHR_KEYCODE_F18,
  ZEPHR_KEYCODE_F19,
  ZEPHR_KEYCODE_F20,
  ZEPHR_KEYCODE_F21,
  ZEPHR_KEYCODE_F22,
  ZEPHR_KEYCODE_F23,
  ZEPHR_KEYCODE_F24,
  [200] = ZEPHR_KEYCODE_NULL, // KEY_PLAYCD
  ZEPHR_KEYCODE_NULL, // KEY_PAUSECD
  ZEPHR_KEYCODE_NULL, // KEY_PROG3
  ZEPHR_KEYCODE_NULL, // KEY_PROG4
  [205] = ZEPHR_KEYCODE_NULL, // KEY_SUSPEND
  ZEPHR_KEYCODE_NULL, // KEY_CLOSE
  ZEPHR_KEYCODE_NULL, // KEY_PLAY
  ZEPHR_KEYCODE_NULL, // AUDIO_FASTFORWARD
  ZEPHR_KEYCODE_NULL, // KEY_BASSBOOST
  ZEPHR_KEYCODE_NULL, // KEY_PRINT
  ZEPHR_KEYCODE_NULL, // KEY_HP
  ZEPHR_KEYCODE_NULL, // KEY_CAMERA
  ZEPHR_KEYCODE_NULL, // KEY_SOUND
  ZEPHR_KEYCODE_NULL, // KEY_QUESTION
  ZEPHR_KEYCODE_NULL, // KEY_EMAIL
  ZEPHR_KEYCODE_NULL, // KEY_CHAT
  ZEPHR_KEYCODE_NULL, // AC_SEARCH
  ZEPHR_KEYCODE_NULL, // KEY_CONNECT
  ZEPHR_KEYCODE_NULL, // KEY_FINANCE
  ZEPHR_KEYCODE_NULL, // KEY_SPORT
  ZEPHR_KEYCODE_NULL, // KEY_SHOP
  ZEPHR_KEYCODE_ALT_ERASE,
  ZEPHR_KEYCODE_CANCEL,
  ZEPHR_KEYCODE_NULL, // BRIGHTNESS_DOWN
  ZEPHR_KEYCODE_NULL, // BRIGHNESS_UP
  ZEPHR_KEYCODE_NULL, // KEY_MEDIA
  ZEPHR_KEYCODE_NULL, // DISPLAY_SWITCH
  ZEPHR_KEYCODE_NULL, // KBD_ILLUM_TOGGLE
  ZEPHR_KEYCODE_NULL, // KBD_ILLUM_DOWN
  ZEPHR_KEYCODE_NULL, // KBD_ILLUM_UP
  ZEPHR_KEYCODE_NULL, // KEY_SEND
  ZEPHR_KEYCODE_NULL, // KEY_REPLY
  ZEPHR_KEYCODE_NULL, // KEY_FORWARDEMAIL
  ZEPHR_KEYCODE_NULL, // KEY_SAVE
  ZEPHR_KEYCODE_NULL, // KEY_DOCUMENTS
  ZEPHR_KEYCODE_NULL, // KEY_BATTERY
};

ZephrKeycode zephr_x11_fn_keysym_to_zephr_keycode_map[] = {
  [0x08] = ZEPHR_KEYCODE_BACKSPACE,
  ZEPHR_KEYCODE_TAB,

  [0x0b] = ZEPHR_KEYCODE_CLEAR,
  [0x0d] = ZEPHR_KEYCODE_ENTER,
  [0x13] = ZEPHR_KEYCODE_PAUSE,
  ZEPHR_KEYCODE_SCROLL_LOCK,
  ZEPHR_KEYCODE_SYSREQ,
  [0x1b] = ZEPHR_KEYCODE_ESCAPE,

  [0x50] = ZEPHR_KEYCODE_HOME,
  ZEPHR_KEYCODE_LEFT,
  ZEPHR_KEYCODE_UP,
  ZEPHR_KEYCODE_RIGHT,
  ZEPHR_KEYCODE_DOWN,
  ZEPHR_KEYCODE_PAGE_UP,
  ZEPHR_KEYCODE_PAGE_DOWN,
  ZEPHR_KEYCODE_END,

  [0x60] = ZEPHR_KEYCODE_SELECT,
  ZEPHR_KEYCODE_PRINT_SCREEN,
  ZEPHR_KEYCODE_EXECUTE,
  ZEPHR_KEYCODE_INSERT,

  [0x65] = ZEPHR_KEYCODE_UNDO,
  [0x67] = ZEPHR_KEYCODE_MENU,
  ZEPHR_KEYCODE_FIND,
  ZEPHR_KEYCODE_CANCEL,
  ZEPHR_KEYCODE_HELP,

  [0x7f] = ZEPHR_KEYCODE_NUM_LOCK_OR_CLEAR,
  [0x80] = ZEPHR_KEYCODE_KP_SPACE,
  [0x89] = ZEPHR_KEYCODE_KP_TAB,
  [0x8d] = ZEPHR_KEYCODE_KP_ENTER,
  [0xbd] = ZEPHR_KEYCODE_KP_EQUALS,

  [0xaa] = ZEPHR_KEYCODE_KP_MULTIPLY,
  ZEPHR_KEYCODE_KP_PLUS,

  [0xad] = ZEPHR_KEYCODE_KP_MINUS,
  ZEPHR_KEYCODE_KP_DECIMAL,
  ZEPHR_KEYCODE_KP_DIVIDE,

  [0xb0] = ZEPHR_KEYCODE_KP_0,
  ZEPHR_KEYCODE_KP_1,
  ZEPHR_KEYCODE_KP_2,
  ZEPHR_KEYCODE_KP_3,
  ZEPHR_KEYCODE_KP_4,
  ZEPHR_KEYCODE_KP_5,
  ZEPHR_KEYCODE_KP_6,
  ZEPHR_KEYCODE_KP_7,
  ZEPHR_KEYCODE_KP_8,
  ZEPHR_KEYCODE_KP_9,

  [0xbe] = ZEPHR_KEYCODE_F1,
  ZEPHR_KEYCODE_F2,
  ZEPHR_KEYCODE_F3,
  ZEPHR_KEYCODE_F4,
  ZEPHR_KEYCODE_F5,
  ZEPHR_KEYCODE_F6,
  ZEPHR_KEYCODE_F7,
  ZEPHR_KEYCODE_F8,
  ZEPHR_KEYCODE_F9,
  ZEPHR_KEYCODE_F10,
  ZEPHR_KEYCODE_F11,
  ZEPHR_KEYCODE_F12,
  ZEPHR_KEYCODE_F13,
  ZEPHR_KEYCODE_F14,
  ZEPHR_KEYCODE_F15,
  ZEPHR_KEYCODE_F16,
  ZEPHR_KEYCODE_F17,
  ZEPHR_KEYCODE_F18,
  ZEPHR_KEYCODE_F19,
  ZEPHR_KEYCODE_F20,
  ZEPHR_KEYCODE_F21,
  ZEPHR_KEYCODE_F22,
  ZEPHR_KEYCODE_F23,
  ZEPHR_KEYCODE_F24,

  [0xe1] = ZEPHR_KEYCODE_LEFT_SHIFT,
  ZEPHR_KEYCODE_RIGHT_SHIFT,
  ZEPHR_KEYCODE_LEFT_CTRL,
  ZEPHR_KEYCODE_RIGHT_CTRL,
  ZEPHR_KEYCODE_CAPS_LOCK,
  ZEPHR_KEYCODE_CAPS_LOCK, // TODO: ???
  ZEPHR_KEYCODE_LEFT_META,
  ZEPHR_KEYCODE_RIGHT_META,
  ZEPHR_KEYCODE_LEFT_ALT,
  ZEPHR_KEYCODE_RIGHT_ALT,
  ZEPHR_KEYCODE_LEFT_META, // TODO: ???
  ZEPHR_KEYCODE_RIGHT_META, // TODO: ???

  [0xff] = ZEPHR_KEYCODE_DELETE,
};

ZephrKeycode zephr_x11_fn_ex_keysym_to_zephr_keycode_map[] = {
  [0x03] = ZEPHR_KEYCODE_RIGHT_ALT,
  [0xff] = ZEPHR_KEYCODE_NULL,
};

ZephrKeycode zephr_x11_latin1_keysym_to_zephr_keycode_map[] = {
  [0x20] = ZEPHR_KEYCODE_SPACE,
  [0x27] = ZEPHR_KEYCODE_APOSTROPHE,
  [0x2c] = ZEPHR_KEYCODE_COMMA,
  ZEPHR_KEYCODE_MINUS,
  ZEPHR_KEYCODE_PERIOD,
  ZEPHR_KEYCODE_SLASH,
  ZEPHR_KEYCODE_0,
  ZEPHR_KEYCODE_1,
  ZEPHR_KEYCODE_2,
  ZEPHR_KEYCODE_3,
  ZEPHR_KEYCODE_4,
  ZEPHR_KEYCODE_5,
  ZEPHR_KEYCODE_6,
  ZEPHR_KEYCODE_7,
  ZEPHR_KEYCODE_8,
  ZEPHR_KEYCODE_9,

  [0x3b] = ZEPHR_KEYCODE_SEMICOLON,
  [0x3d] = ZEPHR_KEYCODE_EQUALS,

  [0x5b] = ZEPHR_KEYCODE_LEFT_BRACKET,
  ZEPHR_KEYCODE_BACKSLASH,
  ZEPHR_KEYCODE_RIGHT_BRACKET,
  [0x60] = ZEPHR_KEYCODE_GRAVE,
  ZEPHR_KEYCODE_A,
  ZEPHR_KEYCODE_B,
  ZEPHR_KEYCODE_C,
  ZEPHR_KEYCODE_D,
  ZEPHR_KEYCODE_E,
  ZEPHR_KEYCODE_F,
  ZEPHR_KEYCODE_G,
  ZEPHR_KEYCODE_H,
  ZEPHR_KEYCODE_I,
  ZEPHR_KEYCODE_J,
  ZEPHR_KEYCODE_K,
  ZEPHR_KEYCODE_L,
  ZEPHR_KEYCODE_M,
  ZEPHR_KEYCODE_N,
  ZEPHR_KEYCODE_O,
  ZEPHR_KEYCODE_P,
  ZEPHR_KEYCODE_Q,
  ZEPHR_KEYCODE_R,
  ZEPHR_KEYCODE_S,
  ZEPHR_KEYCODE_T,
  ZEPHR_KEYCODE_U,
  ZEPHR_KEYCODE_V,
  ZEPHR_KEYCODE_W,
  ZEPHR_KEYCODE_X,
  ZEPHR_KEYCODE_Y,
  ZEPHR_KEYCODE_Z,

  [0xff] = ZEPHR_KEYCODE_NULL,
};

///////////////////////////
//
// X11
//
///////////////////////////

ZephrKeycode x11_keysym_to_zephr_keycode_map(KeySym sym) {
  if (sym >= 0xff00 && sym <= 0xffff) {
    return zephr_x11_fn_keysym_to_zephr_keycode_map[sym - 0xff00];
  } else if (sym >= 0xfe00 && sym <= 0xfeff) {
    return zephr_x11_fn_ex_keysym_to_zephr_keycode_map[sym - 0xfe00];
  } else if (sym >= 0x100 && sym <= 0x1ff) {
    return ZEPHR_KEYCODE_NULL;
    /* return zephr_x11_latin2_keysym_to_zephr_keycode_map[sym - 0x100]; */
  } else if (sym <= 0xff) {
    return zephr_x11_latin1_keysym_to_zephr_keycode_map[sym];
  } else {
    return ZEPHR_KEYCODE_NULL;
  }
}

// update internal keyboard map when the keyboard layout changes during runtime
/* void x11_keyboard_map_update() { */
/*   // reset the keycodes to map directly to the scancodes */
/*   for (int kc = 0; kc < ZEPHR_KEYCODE_COUNT; kc++) { */
/*     zephr_ctx.keyboard.scancode_to_keycode[kc] = kc; */
/*     zephr_ctx.keyboard.keycode_to_scancode[kc] = kc; */
/*   } */

/*   XkbStateRec state; */
/*   XkbGetUpdatedMap(x11_display, XkbAllClientInfoMask, zephr_ctx.xkb); */

/*   unsigned int group = 0; */
/*   if (XkbGetState(x11_display, XkbUseCoreKbd, &state) == Success) { */
/*     group = state.group; */
/*   } */

/*   int first_keycode = zephr_ctx.xkb->min_key_code; */
/*   int last_keycode = zephr_ctx.xkb->max_key_code; */
/*   // we don't map any more keys in evdev after 236 */
/*   if (last_keycode > 236) { */
/*     last_keycode = 236; */
/*   } */

/*   // some x11 magic: https://youtu.be/Y7yIwWIaD-Q?t=12993 */
/*   // last_keycode - first_keycode exclusive because the first keycode is a NULL keycode */
/*   // evdev scancodes are 8 less than x11 keycodes */
/*   XkbGetKeySyms(x11_display, first_keycode, last_keycode - first_keycode, zephr_ctx.xkb); */
/*   XkbClientMapRec *client_map = zephr_ctx.xkb->map; */
/*   for (int x11keycode = 8; x11keycode < last_keycode; x11keycode++) { */
/*     XkbSymMapRec *sym_map = &client_map->key_sym_map[x11keycode]; */
/*     int sym_start_idx = sym_map->offset; */
/*     // since we do not want any key modifiers, our shift is 0 */
/*     int shift = 0; */
/*     int sym_idx = sym_start_idx + (sym_map->width * group) + shift; */
/*     KeySym sym = client_map->syms[sym_idx]; */

/*     ZephrScancode scancode = zephr_evdev_scancode_to_zephr_scancode_map[x11keycode]; */
/*     ZephrKeycode keycode = x11_keysym_to_zephr_keycode_map(sym); */

/*     zephr_ctx.keyboard.scancode_to_keycode[scancode] = keycode; */
/*     zephr_ctx.keyboard.keycode_to_scancode[keycode] = scancode; */
/*   } */
/* } */

void x11_resize_window(void) {
  XWindowAttributes win_attrs;
  XGetWindowAttributes(x11_display, x11_window, &win_attrs);
  glViewport(0, 0, win_attrs.width, win_attrs.height);
}

// BUG: fullscreen doesn't work on gnome. may or may not be related to the window being
// opened as a dialog window
void x11_go_fullscreen(void) {
  XEvent xev;
  Atom wm_state = XInternAtom(x11_display, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(x11_display, "_NET_WM_STATE_FULLSCREEN", False);
  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = x11_window;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
  xev.xclient.data.l[1] = fullscreen;
  xev.xclient.data.l[2] = 0;
  XSendEvent(x11_display, DefaultRootWindow(x11_display), False,
    SubstructureNotifyMask | SubstructureRedirectMask, &xev);
}

void x11_return_fullscreen(void) {
  XEvent xev;
  Atom wm_state = XInternAtom(x11_display, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(x11_display, "_NET_WM_STATE_FULLSCREEN", False);
  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = x11_window;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 0; // _NET_WM_STATE_REMOVE
  xev.xclient.data.l[1] = fullscreen;
  xev.xclient.data.l[2] = 0;
  XSendEvent(x11_display, DefaultRootWindow(x11_display), False,
    SubstructureNotifyMask | SubstructureRedirectMask, &xev);
}

void x11_toggle_fullscreen(bool fullscreen) {
  if (fullscreen) {
    x11_return_fullscreen();
  } else {
    x11_go_fullscreen();
  }
}

// TODO: currently the icon shows on gnome's sidebar correctly but not in gnome-system-monitor.
// investigate.
void x11_assign_window_icon(const char *icon_path) {
  int icon_width, icon_height;
  unsigned char *icon_data = stbi_load(icon_path, &icon_width, &icon_height, NULL, STBI_rgb_alpha);
  CORE_ASSERT(icon_data, "Failed to load icon image\n");

  u32 target_size = 2 + icon_width * icon_height;

  u64 *data = alloca(target_size * sizeof(u64));
  u64 *target = data;

  // first two elements are width and height
  *target++ = icon_width;
  *target++ = icon_height;

  for (int i = 0; i < icon_width * icon_height; i++) {
    *target++ = (icon_data[i * 4] << 16) | (icon_data[i * 4 + 1] << 8) | (icon_data[i * 4 + 2] << 0) | (icon_data[i * 4 + 3] << 24);
  }

  Atom net_wm_icon = XInternAtom(x11_display, "_NET_WM_ICON", False);

  XChangeProperty(x11_display, x11_window, net_wm_icon, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)data, target_size);

  stbi_image_free(icon_data);
}

int x11_create_window(const char* title, int window_width, int window_height, const char *icon_path) {
  x11_display = XOpenDisplay(NULL);
  CORE_ASSERT(x11_display, "Cannot open x11 display connection\n");

  int screen_num = DefaultScreen(x11_display);
  Window root = RootWindow(x11_display, screen_num);
  Visual *visual = DefaultVisual(x11_display, screen_num);

  x11_colormap = XCreateColormap(x11_display, root, visual, AllocNone);

  XSetWindowAttributes attributes;
  attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
    StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
  attributes.colormap = x11_colormap;

  Screen *screen = DefaultScreenOfDisplay(x11_display);

  int window_start_x = screen->width / 2 - window_width / 2;
  int window_start_y = screen->height / 2 - window_height / 2;

  x11_window = XCreateWindow(x11_display, root, window_start_x, window_start_y, window_width, window_height, 0,
      DefaultDepth(x11_display, screen_num), InputOutput, visual,
      CWColormap | CWEventMask, &attributes);

  if (icon_path) {
    x11_assign_window_icon(icon_path);
  }

  // Hints to the WM that the window is a dialog window which makes it a floating
  // window in tiling WMs (only tested on DWM though)
  // Of course this is only a hint and the WM can ignore it
  Atom net_wm_window_type = XInternAtom(x11_display, "_NET_WM_WINDOW_TYPE", False);
  Atom net_wm_window_type_dialog = XInternAtom(x11_display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
  XChangeProperty(x11_display, x11_window, net_wm_window_type, XA_ATOM, 32, PropModeReplace, (unsigned char*)&net_wm_window_type_dialog, 1);

  Atom wm_delete_window = XInternAtom(x11_display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(x11_display, x11_window, &wm_delete_window, 1);
  zephr_ctx->window_delete_atom = wm_delete_window;

  // set window name
  {
    Atom UTF8_STRING = XInternAtom(x11_display, "UTF8_STRING", False);
    XStoreName(x11_display, x11_window, title);
    XTextProperty text_property;
    text_property.value = (unsigned char *)title;
    text_property.format = 8;
    text_property.encoding = UTF8_STRING;
    text_property.nitems = strlen(title);
    XSetWMName(x11_display, x11_window, &text_property);
    Atom net_wm_name = XInternAtom(x11_display, "_NET_WM_NAME", False);
    Atom wm_class = XInternAtom(x11_display, "WM_CLASS", False);
    XChangeProperty(x11_display, x11_window, net_wm_name, UTF8_STRING, 8, PropModeReplace, (unsigned char *)title, strlen(title));
    XChangeProperty(x11_display, x11_window, wm_class, XA_STRING, 8, PropModeReplace, (unsigned char *)title, strlen(title));

    XClassHint *class_hint = XAllocClassHint();

    if (class_hint)
    {
      class_hint->res_name = class_hint->res_class = (char *)title;
      XSetClassHint(x11_display, x11_window, class_hint);
      XFree(class_hint);
    }
  }

  XMapWindow(x11_display, x11_window);

  CORE_ASSERT(x11_window, "Failed to create window\n");

  int glx_version = gladLoaderLoadGLX(x11_display, screen_num);

  if (!glx_version) {
    printf("[FATAL] Failed to load GLX\n");
    return 1;
  }

  printf("[INFO] Loaded GLX %d.%d\n",
      GLAD_VERSION_MAJOR(glx_version), GLAD_VERSION_MINOR(glx_version));

  GLint visual_attributes[] = {
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_DEPTH_SIZE, 24,
    GLX_DOUBLEBUFFER, 1,
    GLX_SAMPLES, 4, // MSAA
    None
  };

  int num_fbc = 0;
  GLXFBConfig *fbc =
    glXChooseFBConfig(x11_display, screen_num, visual_attributes, &num_fbc);

  GLint context_attributes[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
    GLX_CONTEXT_MINOR_VERSION_ARB, 3,
    GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    None
  };

  glx_context = glXCreateContextAttribsARB(x11_display, fbc[0], NULL, 1,
      context_attributes);

  if (!glx_context) {
    printf("[FATAL] Failed to create GLX context\n");
    return 1;
  }

  glXMakeCurrent(x11_display, x11_window, glx_context);

  int gl_version = gladLoaderLoadGL();

  if (!gl_version) {
    printf("[FATAL] Failed to load GL\n");
    return 1;
  }
  printf("[INFO] Loaded GL %d.%d\n",
      GLAD_VERSION_MAJOR(gl_version), GLAD_VERSION_MINOR(gl_version));

  XWindowAttributes win_attrs;
  XGetWindowAttributes(x11_display, x11_window, &win_attrs);

  glad_glXSwapIntervalEXT(x11_display, x11_window, 1);
  // we enable blending for text
  glEnable(GL_BLEND);
  glEnable(GL_MULTISAMPLE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glViewport(0, 0, win_attrs.width, win_attrs.height);
  x11_resize_window();

  XFree(fbc);

  return 0;
}

int x11_init(const char* title, int window_width, int window_height, const char *icon_path) {
  int res = x11_create_window(title, window_width, window_height, icon_path);

	/* // loads the XMODIFIERS environment variable to see what IME to use */
	/* XSetLocaleModifiers(""); */
	/* XIM xim = XOpenIM(x11_display, 0, 0, 0); */
	/* if(!xim){ */
	/* 	// fallback to internal input method */
	/* 	XSetLocaleModifiers("@im=none"); */
	/* 	xim = XOpenIM(x11_display, 0, 0, 0); */
	/* } */
	/* zephr_ctx.xim = xim; */

  /* // XIC */
  /* // create an input context that is used by the whole window */
  /* // you are supposed to have one of these per text field, but this should do */
  /* x11_xic = XCreateIC(zephr_ctx.xim, */
      /* XNInputStyle, XIMPreeditNothing | XIMStatusNothing, */
      /* XNClientWindow, x11_window, XNFocusWindow, x11_window, */
      /* NULL); */

  /* XSetICFocus(x11_xic); */

  /* // TODO: do we need to flush?? */
  /* XFlush(x11_display); */

  // XKB
  /* { */
  /*   XAutoRepeatOn(x11_display); */
  /*   int major = 1, minor = 0; */
  /*   CORE_ASSERT(XkbQueryExtension(x11_display, NULL, NULL, NULL, &major, &minor), "XKB not supported"); */

  /*   zephr_ctx.xkb = XkbGetMap(x11_display, XkbAllClientInfoMask, XkbUseCoreKbd); */

  /*   int repeat = 0; */
  /*   XkbSetDetectableAutoRepeat(x11_display, True, &repeat); */

  /*   XkbSelectEvents(x11_display, XkbUseCoreKbd, XkbMapNotifyMask, XkbMapNotifyMask); */

  /*   x11_keyboard_map_update(); */
  /* } */

  return res;
}

void x11_close(void) {
  glXMakeCurrent(x11_display, 0, 0);
  glXDestroyContext(x11_display, glx_context);

  XDestroyWindow(x11_display, x11_window);
  XFreeColormap(x11_display, x11_colormap);
  XCloseDisplay(x11_display);

  gladLoaderUnloadGLX();
}

void x11_get_screen_size(int *width, int *height) {
  Screen *screen = DefaultScreenOfDisplay(x11_display);

  *width = screen->width;
  *height = screen->height;
}

void x11_make_window_non_resizable(int width, int height) {
  XSizeHints *size_hints = XAllocSizeHints();
  size_hints->flags = PMinSize | PMaxSize;
  size_hints->min_width = width;
  size_hints->min_height = height;
  size_hints->max_width = width;
  size_hints->max_height = height;
  XSetWMNormalHints(x11_display, x11_window, size_hints);
  XFree(size_hints);
}

////////////////////////////
//
// Zephr
//
///////////////////////////


void zephr_init(const char* font_path, const char* icon_path, const char* window_title, Size window_size) {
  zephr_ctx = calloc(1, sizeof(ZephrContext));

  // TODO: should I initalize the audio here or let the game handle that??
  int res = audio_init();
  CORE_ASSERT(res == 0, "Failed to initialize audio");

  res = x11_init(window_title, window_size.width, window_size.height, icon_path);
  CORE_ASSERT(res == 0, "Failed to initialize X11");

  res = init_ui(font_path);
  CORE_ASSERT(res == 0, "Failed to initialize UI");

  zephr_ctx->window.size = window_size;
  zephr_ctx->projection = orthographic_projection_2d(0.f, window_size.width, window_size.height, 0.f);

  zephr_ctx->cursors[ZEPHR_CURSOR_ARROW] = XCreateFontCursor(x11_display, XC_left_ptr);
  zephr_ctx->cursors[ZEPHR_CURSOR_IBEAM] = XCreateFontCursor(x11_display, XC_xterm);
  zephr_ctx->cursors[ZEPHR_CURSOR_CROSSHAIR] = XCreateFontCursor(x11_display, XC_crosshair);
  zephr_ctx->cursors[ZEPHR_CURSOR_HAND] = XCreateFontCursor(x11_display, XC_hand1);
  zephr_ctx->cursors[ZEPHR_CURSOR_HRESIZE] = XCreateFontCursor(x11_display, XC_sb_h_double_arrow);
  zephr_ctx->cursors[ZEPHR_CURSOR_VRESIZE] = XCreateFontCursor(x11_display, XC_sb_v_double_arrow);

  // non-standard cursors
  zephr_ctx->cursors[ZEPHR_CURSOR_DISABLED] = XcursorLibraryLoadCursor(x11_display, "crossed_circle");

  x11_get_screen_size(&zephr_ctx->screen_size.width, &zephr_ctx->screen_size.height);
  start_internal_timer();
}

void zephr_deinit(void) {
  x11_close();
  audio_close();
}

bool zephr_should_quit(void) {
  CORE_DEBUG_ASSERT(zephr_ctx, "Zephr context not initialized");

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  zephr_ctx->cursor = ZEPHR_CURSOR_ARROW;
  zephr_ctx->mouse.released = false;

  audio_update();

  return zephr_ctx->should_quit;
}

// This MUST be called at the end of the frame
void zephr_swap_buffers(void) {
  CORE_DEBUG_ASSERT(zephr_ctx, "Zephr context not initialized");

  if (zephr_ctx->ui.popup_open) {
    draw_color_picker_popup(&zephr_ctx->ui.popup_parent_constraints);
  }

  glXSwapBuffers(x11_display, x11_window);
  XDefineCursor(x11_display, x11_window, zephr_ctx->cursors[zephr_ctx->cursor]);
}

Size zephr_get_window_size(void) {
  return zephr_ctx->window.size;
}

// This MUST be called after calling zephr_init()
void zephr_make_window_non_resizable(void) {
  CORE_DEBUG_ASSERT(zephr_ctx, "Zephr context not initialized");

  x11_make_window_non_resizable(zephr_ctx->window.size.width, zephr_ctx->window.size.height);
}

void zephr_toggle_fullscreen(void) {
  x11_toggle_fullscreen(zephr_ctx->window.is_fullscreen);

  zephr_ctx->window.is_fullscreen = !zephr_ctx->window.is_fullscreen;
}

void zephr_quit(void) {
  zephr_ctx->should_quit = true;
}

Color ColorRGBA(u8 r, u8 g, u8 b, u8 a) {
  return (Color){ r, g, b, a };
}

ZephrKeyMod zephr_x11_mods_to_zephr_mods(XKeyEvent xkey) {
  ZephrKeyMod zephr_mods = 0;

  if (xkey.state & ShiftMask) {
    if (XLookupKeysym(&xkey, 0) == XK_Shift_L) {
      zephr_mods |= ZEPHR_KEY_MOD_LEFT_SHIFT;
    } else if (XLookupKeysym(&xkey, 0) == XK_Shift_R) {
      zephr_mods |= ZEPHR_KEY_MOD_RIGHT_SHIFT;
    } else {
      zephr_mods |= ZEPHR_KEY_MOD_SHIFT;
    }
  }
  if (xkey.state & ControlMask){
    if (XLookupKeysym(&xkey, 0) == XK_Control_L) {
      zephr_mods |= ZEPHR_KEY_MOD_LEFT_CTRL;
    } else if (XLookupKeysym(&xkey, 0) == XK_Control_R) {
      zephr_mods |= ZEPHR_KEY_MOD_RIGHT_CTRL;
    } else {
      zephr_mods |= ZEPHR_KEY_MOD_CTRL;
    }
  }
  if (xkey.state & Mod1Mask) {
    if (XLookupKeysym(&xkey, 0) == XK_Alt_L) {
      zephr_mods |= ZEPHR_KEY_MOD_LEFT_ALT;
    } else if (XLookupKeysym(&xkey, 0) == XK_Alt_R) {
      zephr_mods |= ZEPHR_KEY_MOD_RIGHT_ALT;
    } else {
      zephr_mods |= ZEPHR_KEY_MOD_ALT;
    }
  }
  if (xkey.state & Mod4Mask) {
    if (XLookupKeysym(&xkey, 0) == XK_Super_L) {
      zephr_mods |= ZEPHR_KEY_MOD_LEFT_META;
    } else if (XLookupKeysym(&xkey, 0) == XK_Super_R) {
      zephr_mods |= ZEPHR_KEY_MOD_RIGHT_META;
    } else {
      zephr_mods |= ZEPHR_KEY_MOD_META;
    }
  }
  if (xkey.state & LockMask) {
    zephr_mods |= ZEPHR_KEY_MOD_CAPS_LOCK;
  }
  if (xkey.state & Mod2Mask) {
    zephr_mods |= ZEPHR_KEY_MOD_NUM_LOCK;
  }

  return zephr_mods;
}

bool zephr_iter_events(ZephrEvent *event_out) {
  XEvent xev;

  while (XPending(x11_display)) {
    XNextEvent(x11_display, &xev);

    if (xev.type == ConfigureNotify) {
      XConfigureEvent xce = xev.xconfigure;

      if (xce.width != zephr_ctx->window.size.width || xce.height != zephr_ctx->window.size.height) {
        zephr_ctx->window.size = (Size){ .width = xce.width, .height = xce.height };
        zephr_ctx->projection = orthographic_projection_2d(0.f, xce.width, xce.height, 0.f);
        x11_resize_window();

        event_out->type = ZEPHR_EVENT_WINDOW_RESIZED;
        event_out->window.width = xce.width;
        event_out->window.height = xce.height;

        return true;
      }
    } else if (xev.type == DestroyNotify) {
      // window destroy event
      event_out->type = ZEPHR_EVENT_WINDOW_CLOSED;
      return true;
    } else if (xev.type == ClientMessage) {
      // window close event
      if ((Atom)xev.xclient.data.l[0] == zephr_ctx->window_delete_atom) {
        event_out->type = ZEPHR_EVENT_WINDOW_CLOSED;
        return true;
      }
    } else if (xev.type == KeyPress) {
      XKeyEvent xke = xev.xkey;

      u32 evdev_keycode = xke.keycode - 8;
      ZephrScancode scancode = zephr_evdev_scancode_to_zephr_scancode_map[evdev_keycode];

      event_out->type = ZEPHR_EVENT_KEY_PRESSED;
      event_out->key.code = scancode;
      event_out->key.mods = zephr_x11_mods_to_zephr_mods(xke);

      /* { */
      /*   // remove the control modifier as it causes control codes to be returned */
      /*   xev.xkey.state &= ~ControlMask; */

      /*   char string[4] = {0}; */
      /*   KeySym keysym = 0; */
      /*   u8 string_length = Xutf8LookupString(x11_xic, &xev.xkey, string, sizeof(string), &keysym, NULL); */

				/* // do not send any keys like ctrl, shift, function, arrow, escape, return, backspace. */
				/* // instead, send regular key events. */
				/* if (string_length && !(keysym >= 0xfd00 && keysym <= 0xffff)) { */
					/* os_event_queue_virt_key_input_utf8(string, string_length); */
				/* } */
      /* } */

      /* // an X11 keycode is conceptually the same as our keycode. */
			/* // they are both used to represent a physical key. */
			/* // map evdev enumeration to our keycode */
			/* u32 evdev_keycode = xev.xkey.keycode - 8; */
			/* ZephrKeycode keycode; */
			/* if (evdev_keycode < CORE_ARRAY_COUNT(os_linux_keyboard_evdev_keycode_to_keycode_map)) { */
				/* keycode = os_linux_keyboard_evdev_keycode_to_keycode_map[evdev_keycode]; */
			/* } else { */
				/* keycode = ZEPHR_KEYCODE_NULL; */
			/* } */
			/* os_event_queue_virt_key_changed(xev.type == KeyPress, keycode); */

      return true;
    } else if (xev.type == KeyRelease) {
      XKeyEvent xke = xev.xkey;

      unsigned int evdev_keycode = xke.keycode - 8;
      ZephrScancode scancode = zephr_evdev_scancode_to_zephr_scancode_map[evdev_keycode];

      event_out->type = ZEPHR_EVENT_KEY_RELEASED;
      event_out->key.code = scancode;
      event_out->key.mods = zephr_x11_mods_to_zephr_mods(xke);

      return true;
      
    } else if (xev.type == ButtonPress) {
      event_out->type = ZEPHR_EVENT_MOUSE_BUTTON_PRESSED;
      event_out->mouse.position = (Vec2){ .x = xev.xbutton.x, .y = xev.xbutton.y };
      zephr_ctx->mouse.pressed = true;

      switch (xev.xbutton.button) {
        case Button1:
          event_out->mouse.button = ZEPHR_MOUSE_BUTTON_LEFT;
          zephr_ctx->mouse.button = ZEPHR_MOUSE_BUTTON_LEFT;
          break;
        case Button2:
          event_out->mouse.button = ZEPHR_MOUSE_BUTTON_MIDDLE;
          zephr_ctx->mouse.button = ZEPHR_MOUSE_BUTTON_MIDDLE;
          break;
        case Button3:
          event_out->mouse.button = ZEPHR_MOUSE_BUTTON_RIGHT;
          zephr_ctx->mouse.button = ZEPHR_MOUSE_BUTTON_RIGHT;
          break;
        case Button4:
          event_out->type = ZEPHR_EVENT_MOUSE_SCROLL;
          event_out->mouse.scroll_direction = ZEPHR_MOUSE_SCROLL_UP;
          break;
        case Button5:
          event_out->type = ZEPHR_EVENT_MOUSE_SCROLL;
          event_out->mouse.scroll_direction = ZEPHR_MOUSE_SCROLL_DOWN;
          break;
        case 8: // Back
          event_out->mouse.button = ZEPHR_MOUSE_BUTTON_BACK;
          zephr_ctx->mouse.button = ZEPHR_MOUSE_BUTTON_BACK;
          break;
        case 9: // Forward
          event_out->mouse.button = ZEPHR_MOUSE_BUTTON_FORWARD;
          zephr_ctx->mouse.button = ZEPHR_MOUSE_BUTTON_FORWARD;
          break;
        default:
          printf("[WARN] Unknown mouse button pressed: %d\n", xev.xbutton.button);
          break;
      }
      
      return true;
    } else if (xev.type == ButtonRelease) {
      event_out->type = ZEPHR_EVENT_MOUSE_BUTTON_RELEASED;
      event_out->mouse.position = (Vec2){ .x = xev.xbutton.x, .y = xev.xbutton.y };
      zephr_ctx->mouse.released = true;
      zephr_ctx->mouse.pressed = false;

      switch (xev.xbutton.button) {
        case Button1:
          event_out->mouse.button = ZEPHR_MOUSE_BUTTON_LEFT;
          if (!inside_rect(&zephr_ctx->ui.popup_rect, &event_out->mouse.position)) {
            zephr_ctx->ui.popup_open = false;
          }
          break;
        case Button2:
          event_out->mouse.button = ZEPHR_MOUSE_BUTTON_MIDDLE;
          if (!inside_rect(&zephr_ctx->ui.popup_rect, &event_out->mouse.position)) {
            zephr_ctx->ui.popup_open = false;
          }
          break;
        case Button3:
          event_out->mouse.button = ZEPHR_MOUSE_BUTTON_RIGHT;
          if (!inside_rect(&zephr_ctx->ui.popup_rect, &event_out->mouse.position)) {
            zephr_ctx->ui.popup_open = false;
          }
          break;
      }

      return true;
    } else if (xev.type == MappingNotify) {
      // input device mapping changed
      if (xev.xmapping.request != MappingKeyboard) {
        break;
      }
      XRefreshKeyboardMapping(&xev.xmapping);
      /* x11_keyboard_map_update(); */
      break;
    } else if (xev.type == MotionNotify) {
      event_out->type = ZEPHR_EVENT_MOUSE_MOVED;
      event_out->mouse.position = (Vec2){ .x = xev.xmotion.x, .y = xev.xmotion.y };
      zephr_ctx->mouse.pos = event_out->mouse.position;
      return true;
    }
  }

  return false;
}

void zephr_set_cursor(ZephrCursor cursor) {
  if (zephr_ctx->cursor == cursor) {
    return;
  }

  zephr_ctx->cursor = cursor;
}

/* bool zephr_keyboard_scancode_is_pressed(ZephrScancode scancode) { */
/*   return core_bitset_is_set(zephr_ctx.keyboard.scancode_is_pressed_bitset, scancode); */
/* } */

/* bool zephr_keyboard_scancode_has_been_pressed(ZephrScancode scancode) { */
/*   return core_bitset_is_set(zephr_ctx.keyboard.scancode_has_been_pressed_bitset, scancode); */
/* } */

/* bool zephr_keyboard_scancode_has_been_released(ZephrScancode scancode) { */
/*   return core_bitset_is_set(zephr_ctx.keyboard.scancode_has_been_released_bitset, scancode); */
/* } */


/* bool zephr_keyboard_keycode_is_pressed(ZephrKeycode keycode) { */
/*   return core_bitset_is_set(zephr_ctx.keyboard.keycode_is_pressed_bitset, keycode); */
/* } */

/* bool zephr_keyboard_keycode_has_been_pressed(ZephrKeycode keycode) { */
/*   return core_bitset_is_set(zephr_ctx.keyboard.keycode_has_been_pressed_bitset, keycode); */
/* } */

/* bool zephr_keyboard_keycode_has_been_released(ZephrKeycode keycode) { */
/*   return core_bitset_is_set(zephr_ctx.keyboard.keycode_has_been_released_bitset, keycode); */
/* } */


/* ZephrScancode zephr_keyboard_keycode_to_scancode(ZephrKeycode keycode) { */
/*   return zephr_ctx.keyboard.keycode_to_scancode[keycode]; */
/* } */

/* ZephrKeycode zephr_keyboard_scancode_to_keycode(ZephrScancode scancode) { */
/*   return zephr_ctx.keyboard.scancode_to_keycode[scancode]; */
/* } */
