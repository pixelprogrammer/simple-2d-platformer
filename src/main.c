// override raylib defaults

#include "healthbar.h"
#include "raylib.h"
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

  // load sound effects
  LoadSoundEffect(SOUND_SHOOT, "resources/audio/sfx-mega-buster.wav");

  // init resources
  Player player = CreatePlayer(megaManSprite, projectileTexture);

  // add a few weapons
  player.weapons[WEAPON_FIRE].active = true;
  player.weapons[WEAPON_SPARK].active = true;
  player.weapons[WEAPON_RUSH_JET].active = true;

  // add the 2D camera
  Camera2D camera = {0};
  camera.target = (Vector2){player.position.x, player.position.y};
  camera.zoom = 1.0f;
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

  PlayMusicStream(music);

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    UpdateMusicStream(music);

    // get camera bounds
    Rectangle bounds = GetBoundsRect(camera);

    // Systems functions
    UpdatePlayer(&player, deltaTime);
    UpdatePlatforms(platforms, platformCount, deltaTime);
    UpdateProjectiles(&player.projectiles, deltaTime);

    CheckPlayerCollisions(&player, platforms, platformCount);
    CheckProjectileCollisions(&player.projectiles, bounds);

    // update camera target
    camera.target = (Vector2){player.position.x, player.position.y};

    // Rendering functions
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // start camera operations
    BeginMode2D(camera);

    // draw platforms
    for (int i = 0; i < platformCount; i++) {
      DrawRectangleRec(platforms[i].rect, platforms[i].color);
    }

    DrawPlayer(player, debugMode);

    // draw projectiles
    if (player.projectiles.length > 0) {
      for (int i = 0; i < player.projectiles.length; i++) {
        DrawSprite(player.projectiles.projectiles[i].sprite);
      }
    }

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
    DrawText("Keyboard: Arrow keys to move, Space to jump, Shift to run, C to "
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
