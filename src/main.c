#include "animation.h"
#include "raylib.h"
#include <stdio.h>

#include "./player.h"
#include "./screen.c"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define GROUND_HEIGHT 500

int screen_height = SCREEN_HEIGHT;
int screen_width = SCREEN_WIDTH;
char *screen_title = "Simple 2D Platformer";

int main(void) {

  InitWindow(screen_width, screen_height, screen_title);
  SetTargetFPS(60);

  // load resources
  Texture2D megaManSprite =
      LoadTexture("resources/sprites/mega-man-spritesheet-bw.png");
  AnimationTimeline playerAnimations[PLAYER_TOTAL_STATES];

  // init resources
  Player player = CreatePlayer(megaManSprite);

  printf("Player sprite id: [%d]", player.sprite.id);

  Platform platforms[] = {{{0, GROUND_HEIGHT, SCREEN_WIDTH, 100}, GREEN},
                          {{200, 450, 150, 20}, BROWN},
                          {{450, 300, 150, 20}, BROWN},
                          {{600, 200, 100, 20}, BROWN},
                          {{300, 150, 200, 20}, BROWN}};

  int platformCount = sizeof(platforms) / sizeof(platforms[0]);

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    // Systems functions
    UpdatePlayer(&player, deltaTime);
    CheckPlayerCollisions(&player, platforms, platformCount);

    // Rendering functions
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // draw platforms
    for (int i = 0; i < platformCount; i++) {
      DrawRectangleRec(platforms[i].rect, platforms[i].color);
    }

    DrawPlayer(player);
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
