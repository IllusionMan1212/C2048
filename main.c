#include <stdio.h>

#include "game.h"
#include "zephr.h"

const char *font_path = "assets/fonts/Rubik/Rubik-VariableFont_wght.ttf";
/* const char *font_path = "assets/fonts/Roboto/Roboto-Bold.ttf"; */
const char *game_icon_path = "assets/icon-128x128.png";
const char *title = "C2048";

int main(int argc, char *argv[]) {
  CORE_UNUSED(argc);
  CORE_UNUSED(argv);

  zephr_init(font_path, game_icon_path, title, (Size){1200, 800}, true);
  /* zephr_toggle_fullscreen(); */

  game_loop();

  zephr_deinit();

  return 0;
}
