#include "game.h"
#include "color.h"
#include "enemy.h"
#include "platform.h"
#include "player.h"
#include "raylib.h"
#include "screen.h"
#include "sound.h"
#include "sprite.h"
#include "weapons.h"
#include <stdio.h>

#define GROUND_HEIGHT 500

static GameState currentState = GAME_MENU;
static GameState previousState = GAME_MENU;
static bool shouldExitGame = false;

int screen_height = SCREEN_HEIGHT;
int screen_width = SCREEN_WIDTH;

char *screen_title = "Simple 2D Platformer";

GameState GetCurrentGameState(void) { return currentState; }
GameState GetPreviousGameState(void) { return previousState; }

TextureLoader *textureLoader;

Camera2D camera = {0};
Music music;

PlayerEntity player;
EnemyEntity enemies[1];

int platformCount = 0;
Platform platforms[6];

void GameExit() { shouldExitGame = true; }

bool GameShouldExit() { return shouldExitGame; }

void GameInit() {

  textureLoader = CreateTextureLoader();
  InitWindow(screen_width, screen_height, screen_title);
  // disable Exit key so you don't accidentally close the game window
  // Must be called after InitWindow
  SetExitKey(KEY_NULL);

  InitSoundSystem();
  SetTargetFPS(60);

  // setup the camera
  camera.zoom = 2.0f;
  camera.offset = (Vector2){screen_width / 2.0f, screen_height / 2.0f};

  // TODO: move this to the stage loader
  // load resources
  int playerTextureId = LoadTextureToLoader(
      textureLoader, "resources/sprites/mega-man-spritesheet.png");
  music = LoadMusicStream("resources/audio/music_stage_1.mp3");
  int projectileTextureId =
      LoadTextureToLoader(textureLoader, "resources/sprites/buster-shot.png");
  // load sound effects
  LoadSoundEffect(SOUND_SHOOT, "resources/audio/sfx-mega-buster.wav");

  // init resources
  player = CreatePlayer(playerTextureId, projectileTextureId);

  // add a few weapons
  player.weapons[WEAPON_FIRE].active = true;
  player.weapons[WEAPON_SPARK].active = true;
  player.weapons[WEAPON_RUSH_JET].active = true;

  EnemyEntity singleEnemy;
  InitEnemy(&singleEnemy, (Vector2){75, 75}, -1);

  // enemies
  enemies[0] = singleEnemy;

  // Static platforms
  platforms[0] = (Platform){
      {
          0,
          GROUND_HEIGHT,
          SCREEN_WIDTH,
          100,
      },
      BROWN,
      PLATFORM_STATIC,
      {0, 0},
      {0, GROUND_HEIGHT},
      {0, GROUND_HEIGHT},
      0,
      false,
  };
  platforms[1] = (Platform){
      {
          200,
          450,
          150,
          20,
      },
      BROWN,
      PLATFORM_STATIC,
      {0, 0},
      {200, 450},
      {200, 450},
      0,
      false,
  };
  platforms[2] = (Platform){
      {600, 200, 100, 20}, BROWN, PLATFORM_STATIC, {0, 0}, {600, 200},
      {600, 200},          0,     false,
  };
  // Moving horizontal platform
  platforms[3] = (Platform){
      {450, 300, 150, 20},
      GREEN,
      PLATFORM_MOVING_HORIZONTAL,
      {0, 0},
      {450, 300},
      {650, 300},
      50,
      false,
  };

  // Moving vertical platform
  platforms[4] = (Platform){
      {300, 400, 200, 20},
      BLUE,
      PLATFORM_MOVING_VERTICAL,
      {0, 0},
      {300, 400},
      {300, 150},
      75,
      false,
  };
  // Moving circular platform
  platforms[5] = (Platform){
      {100, 350, 80, 20},
      PURPLE,
      PLATFORM_MOVING_CIRCULAR,
      {0, 0},
      {100, 350},
      {200, 250},
      1.0f,
      false,
  };

  platformCount = 6;
  // END stage loader
}

void GameCleanup() {
  printf("Cleanup resources\n");
  DestroyTextureLoader(textureLoader);
  // unload
  DestroySoundSystem();
  UnloadMusicStream(music);

  CloseAudioDevice();
  CloseWindow();
}

void SetGameState(GameState newState) {
  if (newState != currentState) {
    previousState = currentState;
    currentState = newState;
  }
}

void UpdateGameState(void) {
  switch (currentState) {
  case GAME_MENU:
    UpdateMenuState();
    break;
  case GAME_LEVEL_SELECT:
    UpdateLevelSelectState();
    break;
  case GAME_LEVEL_PLAYING:
    UpdatePlayingState();
    break;
  case GAME_LEVEL_PAUSED:
    UpdatePausedState();
    break;
  case GAME_GAME_OVER:
    UpdateGameOverState();
    break;
  case GAME_VICTORY:
    UpdateVictoryState();
    break;
  default:
    break;
  }
}

void DrawGameState(void) {
  BeginDrawing();

  switch (currentState) {
  case GAME_MENU:
    DrawMenuState();
    break;
  case GAME_LEVEL_SELECT:
    DrawLevelSelectState();
    break;
  case GAME_LEVEL_PLAYING:
    DrawPlayingState();
    break;
  case GAME_LEVEL_PAUSED:
    DrawPausedState();
    break;
  case GAME_GAME_OVER:
    DrawGameOverState();
    break;
  case GAME_VICTORY:
    DrawVictoryState();
    break;
  default:
    break;
  }

  EndDrawing();
}

void UpdateMenuState(void) {
  if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
    SetGameState(GAME_LEVEL_SELECT);
  }
  if (IsKeyPressed(KEY_ESCAPE)) {
    GameExit();
  }
}

void UpdateLevelSelectState(void) {
  if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {

    // TODO: load the world data
    SetGameState(GAME_LEVEL_PLAYING);
  }
  if (IsKeyPressed(KEY_ESCAPE)) {
    SetGameState(GAME_MENU);
  }
}

void UpdatePlayingState(void) {
  if (IsKeyPressed(KEY_ESCAPE)) {
    SetGameState(GAME_LEVEL_PAUSED);
  }

  float deltaTime = GetFrameTime();
  UpdateMusicStream(music);

  // get camera bounds
  Rectangle cameraBounds = GetCameraWorldRectangle(camera);

  // Systems functions
  PlayerControllerSystem(&player.controller, &player.actions);

  UpdatePlayer(&player, deltaTime);
  UpdatePlatforms(platforms, platformCount, deltaTime);
  UpdateProjectiles(&player.projectiles, deltaTime);

  CheckPlayerCollisions(&player, platforms, platformCount);
  CheckProjectileCollisions(&player.projectiles, cameraBounds);

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
}

void UpdatePausedState(void) {
  if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
    SetGameState(GAME_LEVEL_PLAYING);
  }
  if (IsKeyPressed(KEY_Q)) {
    SetGameState(GAME_MENU);
  }
}

void UpdateGameOverState(void) {
  if (IsKeyPressed(KEY_R)) {
    SetGameState(GAME_LEVEL_PLAYING);
  }
  if (IsKeyPressed(KEY_ESCAPE)) {
    SetGameState(GAME_MENU);
  }
}

void UpdateVictoryState(void) {
  if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
    SetGameState(GAME_LEVEL_SELECT);
  }
  if (IsKeyPressed(KEY_ESCAPE)) {
    SetGameState(GAME_MENU);
  }
}

void DrawMenuState(void) {
  ClearBackground(BLACK);
  DrawText("MAIN MENU", 100, 200, 40, WHITE);
  DrawText("Press ENTER to start", 100, 300, 20, GRAY);
  DrawText("Press ESC to quit", 100, 330, 20, GRAY);
}

void DrawLevelSelectState(void) {
  ClearBackground(DARKBLUE);
  DrawText("LEVEL SELECT", 100, 200, 40, WHITE);
  DrawText("Press ENTER to play", 100, 300, 20, GRAY);
  DrawText("Press ESC to go back", 100, 330, 20, GRAY);
}

void DrawPlayingState(void) {
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
      Texture2D texture =
          GetTextureFromLoader(textureLoader, player.projectileTextureId);

      DrawSprite(player.projectiles.projectiles[i].sprite, &texture);
    }
  }

  Texture2D playerTexture =
      GetTextureFromLoader(textureLoader, player.spriteTextureId);
  DrawPlayer(player, &playerTexture, false);

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

  // DrawText("PLAYING", 100, 50, 30, WHITE);
  // DrawText("Press ESC to pause", 100, 100, 20, GRAY);
}

void DrawPausedState(void) {
  DrawPlayingState();
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
  DrawText("PAUSED", 100, 200, 40, WHITE);
  DrawText("Press ESC or P to resume", 100, 300, 20, GRAY);
  DrawText("Press Q to quit to menu", 100, 330, 20, GRAY);
}

void DrawGameOverState(void) {
  ClearBackground(BLACK);
  DrawText("GAME OVER", 100, 200, 40, WHITE);
  DrawText("Press R to restart", 100, 300, 20, GRAY);
  DrawText("Press ESC to go to menu", 100, 330, 20, GRAY);
}

void DrawVictoryState(void) {
  ClearBackground(DARKGREEN);
  DrawText("VICTORY!", 100, 200, 40, WHITE);
  DrawText("Press ENTER to continue", 100, 300, 20, GRAY);
  DrawText("Press ESC to go to menu", 100, 330, 20, GRAY);
}
