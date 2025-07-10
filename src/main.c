#include "animation.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

#include "./platform.h"
#include "./player.h"
#include "./screen.c"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define GROUND_HEIGHT 500

int screen_height = SCREEN_HEIGHT;
int screen_width = SCREEN_WIDTH;
char *screen_title = "Simple 2D Platformer";

int main(int argc, char *argv[]) {
  bool debugMode = false;

  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
      debugMode = true;
    }
  }

  InitWindow(screen_width, screen_height, screen_title);
  SetTargetFPS(60);

  // load resources
  Texture2D megaManSprite =
      LoadTexture("resources/sprites/mega-man-spritesheet.png");
  AnimationTimeline playerAnimations[PLAYER_TOTAL_STATES];

  // init resources
  Player player = CreatePlayer(megaManSprite);

  printf("Player sprite id: [%d]", player.sprite.id);

  Platform platforms[] = {// Static ground platform
                          {{0, GROUND_HEIGHT, SCREEN_WIDTH, 100},
                           BROWN,
                           PLATFORM_STATIC,
                           {0, 0},
                           {0, GROUND_HEIGHT},
                           {0, GROUND_HEIGHT},
                           0,
                           false},
                          // Static platforms
                          {{200, 450, 150, 20},
                           BROWN,
                           PLATFORM_STATIC,
                           {0, 0},
                           {200, 450},
                           {200, 450},
                           0,
                           false},
                          {{600, 200, 100, 20},
                           BROWN,
                           PLATFORM_STATIC,
                           {0, 0},
                           {600, 200},
                           {600, 200},
                           0,
                           false},
                          // Moving horizontal platform
                          {{450, 300, 150, 20},
                           GREEN,
                           PLATFORM_MOVING_HORIZONTAL,
                           {0, 0},
                           {450, 300},
                           {650, 300},
                           50,
                           false},
                          // Moving vertical platform
                          {{300, 400, 200, 20},
                           BLUE,
                           PLATFORM_MOVING_VERTICAL,
                           {0, 0},
                           {300, 400},
                           {300, 150},
                           75,
                           false},
                          // Moving circular platform
                          {{100, 350, 80, 20},
                           PURPLE,
                           PLATFORM_MOVING_CIRCULAR,
                           {0, 0},
                           {100, 350},
                           {200, 250},
                           1.0f,
                           false}};

  int platformCount = sizeof(platforms) / sizeof(platforms[0]);

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    // Systems functions
    UpdatePlatforms(platforms, platformCount, deltaTime);
    UpdatePlayer(&player, deltaTime);
    CheckPlayerCollisions(&player, platforms, platformCount);

    // Rendering functions
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // draw platforms
    for (int i = 0; i < platformCount; i++) {
      DrawRectangleRec(platforms[i].rect, platforms[i].color);
    }

    DrawPlayer(player, debugMode);
    DrawText("Use gamepad D-pad/left stick to move, A button to jump, Left "
             "Trigger to run",
             10, 10, 20, DARKGRAY);
    DrawText("Keyboard: Arrow keys to move, Space to jump, Shift to run", 10,
             35, 20, DARKGRAY);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
