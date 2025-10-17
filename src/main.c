// override raylib defaults

#include "color.h"
#include "enemy.h"
#include "healthbar.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

#include "./platform.h"
#include "./player.h"
#include "./screen.h"
#include "sound.h"
#include "sprite.h"
#include "weapons.h"

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
  InitSoundSystem();
  SetTargetFPS(60);

  // load resources
  Texture2D megaManSprite =
      LoadTexture("resources/sprites/mega-man-spritesheet.png");
  Music music = LoadMusicStream("resources/audio/music_stage_1.mp3");
  Texture2D projectileTexture =
      LoadTexture("resources/sprites/buster-shot.png");
  Texture2D enemySprite;
  // load sound effects
  LoadSoundEffect(SOUND_SHOOT, "resources/audio/sfx-mega-buster.wav");

  // init resources
  PlayerEntity player = CreatePlayer(megaManSprite, projectileTexture);

  // add a few weapons
  player.weapons[WEAPON_FIRE].active = true;
  player.weapons[WEAPON_SPARK].active = true;
  player.weapons[WEAPON_RUSH_JET].active = true;

  EnemyEntity singleEnemy;
  InitEnemy(&singleEnemy, (Vector2){75, 75}, enemySprite);

  // enemies
  EnemyEntity enemies[] = {singleEnemy};

  // add the 2D camera
  Camera2D camera = {0};
  camera.target =
      (Vector2){player.moveable.position.x, player.moveable.position.y};
  camera.zoom = 2.0f;
  camera.offset = (Vector2){screen_width / 2.0f, screen_height / 2.0f};

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

  // PlayMusicStream(music);

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();
    UpdateMusicStream(music);

    // get camera bounds
    Rectangle bounds = GetBoundsRect(camera);

    // Systems functions
    PlayerControllerSystem(&player.controller, &player.actions);

    UpdatePlayer(&player, deltaTime);
    UpdatePlatforms(platforms, platformCount, deltaTime);
    UpdateProjectiles(&player.projectiles, deltaTime);

    CheckPlayerCollisions(&player, platforms, platformCount);
    CheckProjectileCollisions(&player.projectiles, bounds);

    for (int i = 0; i < 1; i++) {
      UpdateEnemy(&enemies[i], &player, deltaTime);
      if (enemies[i].active) {
        CheckPlayerHurt(&player, enemies[i].hitbox);
      }
      CheckProjectileToEnemyCollisions(&enemies[i], &player.projectiles);
    }

    // update camera target
    camera.target = (Vector2){
        player.moveable.position.x,
        player.moveable.position.y,
    };

    // Rendering functions
    BeginDrawing();
    ClearBackground(COLOR_CORNFLOWER_BLUE);

    // start camera operations
    BeginMode2D(camera);

    // draw platforms
    for (int i = 0; i < platformCount; i++) {
      DrawRectangleRec(platforms[i].rect, platforms[i].color);
    }

    // draw enemies
    for (int i = 0; i < 1; i++) {
      DrawEnemy(enemies[i]);
    }

    // draw projectiles
    if (player.projectiles.length > 0) {
      for (int i = 0; i < player.projectiles.length; i++) {
        DrawSprite(player.projectiles.projectiles[i].sprite);
      }
    }

    DrawPlayer(player, debugMode);

    EndMode2D();
    // end camera mode

    // Draw UI
    DrawHealthBar(player.healthbar);
    if (player.currentWeapon > 0) {
      DrawHealthBar(player.weapons[player.currentWeapon].healthBar);
    }

    DrawText("Use gamepad D-pad/left stick to move, A button to jump, Y "
             "button to run and X button to shoot",
             10, 10, 20, DARKGRAY);
    DrawText("Keyboard: Arrow keys to move, Space to jump, C to "
             "cycle colors",
             10, 35, 20, DARKGRAY);
    // End Draw UI
    EndDrawing();
  }

  // unload
  DestroySoundSystem();
  UnloadMusicStream(music);

  CloseAudioDevice();
  CloseWindow();
  return 0;
}
