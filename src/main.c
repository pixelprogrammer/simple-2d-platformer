// override raylib defaults

#include "game.h"
#include "raylib.h"
#include <string.h>

int main(int argc, char *argv[]) {
  bool debugMode = false;

  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
      debugMode = true;
    }
  }

  GameInit();

  while (!WindowShouldClose() && !GameShouldExit()) {
    UpdateGameState();
    DrawGameState();
  }

  GameCleanup();
  return 0;
}
