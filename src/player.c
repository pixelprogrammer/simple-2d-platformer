#include "./player.h"
#include "./screen.c"
#include "animation.h"
#include "healthbar.h"
#include "weapons.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

#define MAX_GAMEPADS 4

Vector2 PLAYER_SIZE = {22, 24};

Vector3 PLAYER_PRIMARY_COLOR = (Vector3){0.0f, 0.45f, 0.95f};
Vector3 PLAYER_SECONDARY_COLOR = (Vector3){0.0f, 1.0f, 1.0f};
float PLAYER_SHADER_COLOR_SWAP_TOLERANCE = 0.1f;

const float PLAYER_RUNNING_FRAME_DURATION = 0.25;

void MovePlayer(Player *player, Vector2 newPosition) {
  player->position.x += newPosition.x;
  player->position.y += newPosition.y;
}

AnimationTimeline GetCurrentAnimationTimeline(Player *player) {
  return player->timelines[player->state];
}

void DrawPlayer(Player player, bool debugMode) {
  if (player.sprite.id != 0) {
    AnimationTimeline timeline = GetCurrentAnimationTimeline(&player);
    Vector2 size = {timeline.frame.width, timeline.frame.height};
    Vector2 drawPosition = {player.position.x + timeline.position.x,
                            player.position.y + timeline.position.y};
    Vector2 origin = (Vector2){timeline.frame.width / 2 + timeline.position.x,
                               timeline.frame.height / 2 + timeline.position.y};
    bool useShader = false;
    if (player.currentWeapon != WEAPON_DEFAULT && player.colorShader.id != 0) {
      useShader = true;
    }

    if (useShader) {
      BeginShaderMode(player.colorShader);

      Vector3 primaryTintColor;
      Vector3 secondaryTintColor;

      switch (player.currentWeapon) {
      case WEAPON_LAZER:
        primaryTintColor = (Vector3){1.0f, 0.4f, 0.7f};
        secondaryTintColor = (Vector3){0.0f, 1.0f, 1.0f};
        break;
      case WEAPON_SPARK:
        primaryTintColor = (Vector3){0.6f, 0.2f, 0.8f};
        secondaryTintColor = (Vector3){1.0f, 0.6f, 1.0f};
        break;
      case WEAPON_RUSH_JET:
      case WEAPON_RUSH_JUMP:
        primaryTintColor = (Vector3){1.0f, 0.23f, 0.0f};
        secondaryTintColor = (Vector3){0.7f, 0.7f, 0.7f};
        break;
      case WEAPON_FIRE:
        primaryTintColor = (Vector3){1.0f, 0.23f, 0.0f};
        secondaryTintColor = (Vector3){1.0f, 0.74f, 0.0f};
        break;
      default:
        primaryTintColor = (Vector3){1.0f, 1.0f, 1.0f};
        secondaryTintColor = (Vector3){1.0f, 1.0f, 1.0f};
        break;
      }

      // set the primary color and primary color target
      SetShaderValue(player.colorShader, player.primaryTintColorLoc,
                     &primaryTintColor, SHADER_UNIFORM_VEC3);
      SetShaderValue(player.colorShader, player.primaryTargetColorLoc,
                     &PLAYER_PRIMARY_COLOR, SHADER_UNIFORM_VEC3);

      // set the primary color and primary color target
      SetShaderValue(player.colorShader, player.secondaryTintColorLoc,
                     &secondaryTintColor, SHADER_UNIFORM_VEC3);
      SetShaderValue(player.colorShader, player.secondaryTargetColorLoc,
                     &PLAYER_SECONDARY_COLOR, SHADER_UNIFORM_VEC3);

      SetShaderValue(player.colorShader, player.toleranceLoc,
                     &PLAYER_SHADER_COLOR_SWAP_TOLERANCE, SHADER_UNIFORM_FLOAT);
    }

    DrawAnimatedSprite(&player.sprite, &timeline, drawPosition, size, origin,
                       player.facingRight);

    if (useShader) {
      EndShaderMode();
    }
  } else {
    Vector2 drawPosition = {player.position.x - player.size.x / 2,
                            player.position.y - player.size.y / 2};
    Rectangle playerRect = {drawPosition.x, drawPosition.y, player.size.x,
                            player.size.y};
    DrawRectangleRec(playerRect, player.color);
  }

  if (debugMode) {
    // Rectangle hitBoxRect = {player.position.x + player.hitBox.x,
    //                         player.position.y + player.hitBox.y,
    //                         player.hitBox.width, player.hitBox.height};
    // DrawRectangleLinesEx(hitBoxRect, 1.0f, RED);

    Rectangle collisionBoxRect = GetPlayerPosition(&player);

    DrawRectangleLinesEx(collisionBoxRect, 1.0f, GREEN);
    DrawCircle(collisionBoxRect.x + collisionBoxRect.width / 2,
               collisionBoxRect.y + collisionBoxRect.height / 2, 2, RED);
  }
}

void UpdatePlayerState(Player *player, float inputDirection, bool isRunning,
                       bool isShooting) {
  PlayerState newState = player->state;

  if (!player->onGround) {
    newState = isShooting ? PLAYER_JUMPING_SHOOTING : PLAYER_JUMPING;
  } else if (fabs(inputDirection) > 0.1f) {
    if (isRunning) {
      newState = isShooting ? PLAYER_RUNNING_SHOOTING : PLAYER_RUNNING;
    } else {
      newState = isShooting ? PLAYER_RUNNING_SHOOTING : PLAYER_RUNNING;
    }
  } else {
    newState = isShooting ? PLAYER_STANDING_SHOOTING : PLAYER_STANDING;
  }

  if (newState != player->state) {
    PlayerState oldState = player->state;
    bool shouldReset = ShouldPlayerResetAnimationTimeline(player, newState);
    player->state = newState;

    if (shouldReset) {
      ResetAnimationTimeline(&player->timelines[player->state]);
    } else {
      CopyPlayerAnimationTimeline(player, newState, oldState);
    }
  }
}

/*
 * Copies the current frame and current frame time to the new timeline
 * This is to help maintain similar frames between the two states like running
 * and running+shooting
 */
void CopyPlayerAnimationTimeline(Player *player, PlayerState newState,
                                 PlayerState oldState) {
  player->timelines[newState].currentFrameIndex =
      player->timelines[oldState].currentFrameIndex;
  player->timelines[newState].currentTime =
      player->timelines[oldState].currentTime;
}

bool ShouldPlayerResetAnimationTimeline(Player *player, PlayerState newState) {
  if (player->state != PLAYER_RUNNING_SHOOTING &&
      player->state != PLAYER_RUNNING) {
    return true;
  }

  // if player is still running but shooting or not shooting, do not reset the
  // animation timeline so the player maintains their running animation
  if (newState == PLAYER_RUNNING_SHOOTING || newState == PLAYER_RUNNING) {
    return false;
  }

  return true;
}

void UpdatePlayer(Player *player, float deltaTime) {
  bool gamepadConnected = IsGamepadAvailable(player->gamepadId);
  float inputDirection = 0.0f;

  if (!gamepadConnected) {
    // try to detect a controller
    for (int i = 0; i < MAX_GAMEPADS; i++) {
      if (IsGamepadAvailable(i) &&
          IsGamepadButtonDown(i, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        // set the gamepadId
        player->gamepadId = i;
        break;
      }
    }
  }
  if (gamepadConnected) {
    float axisX =
        GetGamepadAxisMovement(player->gamepadId, GAMEPAD_AXIS_LEFT_X);
    if (fabs(axisX) > 0.1f) {
      inputDirection = axisX;
    }

    if (IsGamepadButtonDown(player->gamepadId, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ||
        GetGamepadAxisMovement(player->gamepadId, GAMEPAD_AXIS_LEFT_X) <
            -0.1f) {
      inputDirection = -1.0f;
    }
    if (IsGamepadButtonDown(player->gamepadId,
                            GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ||
        GetGamepadAxisMovement(player->gamepadId, GAMEPAD_AXIS_LEFT_X) > 0.1f) {
      inputDirection = 1.0f;
    }
  }

  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    inputDirection = -1.0f;
  }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    inputDirection = 1.0f;
  }

  bool isRunning =
      IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT) ||
      IsGamepadButtonDown(player->gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_UP);

  bool isShooting =
      IsKeyDown(KEY_X) || IsKeyDown(KEY_Z) ||
      IsGamepadButtonDown(player->gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);

  // change weapon/colormode
  // TODO: Change weapon
  if (IsKeyPressed(KEY_C) ||
      IsGamepadButtonPressed(player->gamepadId,
                             GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
    ChangeNextWeapon(player);
  }

  float maxSpeed = isRunning ? PLAYER_RUN_SPEED : PLAYER_SPEED;

  if (inputDirection != 0.0f) {
    player->velocity.x += inputDirection * PLAYER_ACCELERATION * deltaTime;
    player->facingRight = (inputDirection > 0);

    if (player->velocity.x > maxSpeed) {
      player->velocity.x = maxSpeed;
    } else if (player->velocity.x < -maxSpeed) {
      player->velocity.x = -maxSpeed;
    }
  } else {
    if (player->velocity.x > 0) {
      player->velocity.x -= PLAYER_FRICTION * deltaTime;
      if (player->velocity.x < 0)
        player->velocity.x = 0;
    } else if (player->velocity.x < 0) {
      player->velocity.x += PLAYER_FRICTION * deltaTime;
      if (player->velocity.x > 0)
        player->velocity.x = 0;
    }
  }

  bool jumpKeyPressed =
      IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) ||
      IsGamepadButtonPressed(player->gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
  bool jumpKeyDown =
      IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) ||
      IsGamepadButtonDown(player->gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);

  if (jumpKeyPressed && player->onGround) {
    player->velocity.y = JUMP_SPEED;
    player->isJumping = true;
  }

  if (player->isJumping && !jumpKeyDown &&
      player->velocity.y < MIN_JUMP_SPEED) {
    player->velocity.y *= JUMP_RELEASE_FACTOR;
    player->isJumping = false;
  }

  player->velocity.y += GRAVITY * deltaTime;

  Vector2 moveVelocity = {player->velocity.x * deltaTime,
                          player->velocity.y * deltaTime};
  MovePlayer(player, moveVelocity);

  // keep player within screen bounds
  if (player->position.x + player->collisionBox.x < 0) {
    player->position.x = -player->collisionBox.x;
  }
  if (player->position.x + player->collisionBox.x + player->collisionBox.width >
      screen_width) {
    player->position.x =
        screen_width - player->collisionBox.x - player->collisionBox.width;
  }

  UpdatePlayerState(player, inputDirection, isRunning, isShooting);

  PlayAnimationTimeline(&player->timelines[player->state], deltaTime);

  player->onGround = false;
}

Rectangle GetPlayerPosition(Player *player) {
  return (Rectangle){player->position.x + player->collisionBox.x,
                     player->position.y + player->collisionBox.y,
                     player->collisionBox.width, player->collisionBox.height};
}

void CheckPlayerCollisions(Player *player, Platform platforms[],
                           int platformCount) {
  Rectangle playerRect = GetPlayerPosition(player);
  static int groundedPlatformIndex = -1;

  for (int i = 0; i < platformCount; i++) {
    if (CheckCollisionRecs(playerRect, platforms[i].rect)) {

      // Calculate overlap amounts
      float overlapLeft =
          (playerRect.x + playerRect.width) - platforms[i].rect.x;
      float overlapRight =
          (platforms[i].rect.x + platforms[i].rect.width) - playerRect.x;
      float overlapTop =
          (playerRect.y + playerRect.height) - platforms[i].rect.y;
      float overlapBottom =
          (platforms[i].rect.y + platforms[i].rect.height) - playerRect.y;

      // Find minimum overlap to determine collision direction
      if (overlapTop < overlapBottom && overlapTop < overlapLeft &&
          overlapTop < overlapRight) {
        // Collision from top (player landing on platform)
        if (player->velocity.y > 0) {
          player->position.y = platforms[i].rect.y + player->collisionBox.y;
          player->velocity.y = 0;
          player->onGround = true;
          player->isJumping = false;
          groundedPlatformIndex = i;
        }
      } else if (overlapBottom < overlapTop && overlapBottom < overlapLeft &&
                 overlapBottom < overlapRight) {
        // Collision from bottom (player hitting platform from below)
        if (player->velocity.y < 0) {
          player->position.y = platforms[i].rect.y + platforms[i].rect.height -
                               player->collisionBox.y;
          player->velocity.y = 0;
        }
      } else if (overlapLeft < overlapRight) {
        // Collision from left (player moving right into platform)
        if (player->velocity.x > 0) {
          player->position.x = platforms[i].rect.x + player->collisionBox.x;
          player->velocity.x = 0;
        }
      } else {
        // Collision from right (player moving left into platform)
        if (player->velocity.x < 0) {
          player->position.x = platforms[i].rect.x + platforms[i].rect.width -
                               player->collisionBox.x;
          player->velocity.x = 0;
        }
      }
    }
  }

  // Move player with platform if grounded to a moving platform
  if (player->onGround && groundedPlatformIndex >= 0 &&
      groundedPlatformIndex < platformCount) {
    Platform *groundedPlatform = &platforms[groundedPlatformIndex];
    if (groundedPlatform->type != PLATFORM_STATIC) {
      // Move player with the platform
      player->position.x += groundedPlatform->velocity.x * GetFrameTime();
      player->position.y += groundedPlatform->velocity.y * GetFrameTime();
    }
  }

  // Reset grounded platform if player is no longer on ground
  if (!player->onGround) {
    groundedPlatformIndex = -1;
  }
}

Player CreatePlayer(Texture2D sprite) {
  Player player = {
      .position = {111, 412},
      .prevPosition = {111, 412},
      .size = PLAYER_SIZE,
      .origin = {0, 0},
      .velocity = {0, 0},
      .collisionBox = {-PLAYER_SIZE.x / 2, -PLAYER_SIZE.y / 2, PLAYER_SIZE.x,
                       PLAYER_SIZE.y},
      .hitBox = {PLAYER_SIZE.x / 2, PLAYER_SIZE.y / 2, PLAYER_SIZE.x,
                 PLAYER_SIZE.y},
      .onGround = false,
      .isJumping = false,
      .color = BLUE,
      .sprite = sprite,
      .currentWeapon = WEAPON_DEFAULT,
      .weapons = CreateWeaponsArray(),
      .gamepadId = -1,
      .healthbar = CreateHealthBar(
          (Vector2){50, 100}, PLAYER_HEALTHBAR_MAX_HEALTH,
          PLAYER_HEALTHBAR_SEGMENT_WIDTH, PLAYER_HEALTHBAR_SEGMENT_HEIGHT),
      .timelines = {
          [PLAYER_STANDING] = {.loop = true,
                               .position = {-11, -12},
                               .frame = {0, 0, 40, 40},
                               .frameGap = 0},
          [PLAYER_STANDING_SHOOTING] = {.loop = true,
                                        .position = {-11, -12},
                                        .frame = {80, 0, 40, 40},
                                        .frameGap = 0},
          [PLAYER_JUMPING] = {.loop = true,
                              .position = {-13, -15},
                              .frame = {0, 120, 40, 40},
                              .frameGap = 0},
          [PLAYER_JUMPING_SHOOTING] = {.loop = true,
                                       .position = {-13, -15},
                                       .frame = {0, 160, 40, 40},
                                       .frameGap = 0},
          [PLAYER_RUNNING] = {.loop = true,
                              .position = {-11, -11},
                              .frame = {0, 40, 40, 40},
                              .frameGap = 0},
          [PLAYER_RUNNING_SHOOTING] = {.loop = true,
                                       .position = {-11, -11},
                                       .frame = {0, 80, 40, 40},
                                       .frameGap = 0},
          [PLAYER_SLIDING] = {},
          [PLAYER_CLIMBING] = {},
          [PLAYER_CLIMBING_SHOOTING] = {},

      }};

  // add the frames to each timeline
  // Standing and blinking
  AddAnimationFrame(&player.timelines[PLAYER_STANDING], 0, 5);
  AddAnimationFrame(&player.timelines[PLAYER_STANDING], 1, 0.05);
  AddAnimationFrame(&player.timelines[PLAYER_STANDING], 0, 0.25);
  AddAnimationFrame(&player.timelines[PLAYER_STANDING], 1, 0.05);

  // Standing and Shooting
  AddAnimationFrame(&player.timelines[PLAYER_STANDING_SHOOTING], 0, 0);

  // Running
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING], 0,
                    PLAYER_RUNNING_FRAME_DURATION);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING], 1,
                    PLAYER_RUNNING_FRAME_DURATION);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING], 2,
                    PLAYER_RUNNING_FRAME_DURATION);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING], 1,
                    PLAYER_RUNNING_FRAME_DURATION);

  // Running and Shooting
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING_SHOOTING], 0,
                    PLAYER_RUNNING_FRAME_DURATION);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING_SHOOTING], 1,
                    PLAYER_RUNNING_FRAME_DURATION);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING_SHOOTING], 2,
                    PLAYER_RUNNING_FRAME_DURATION);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING_SHOOTING], 1,
                    PLAYER_RUNNING_FRAME_DURATION);

  // Jumping

  // load shader values
  player.colorShader = LoadShader(0, "resources/shaders/color_tint.glsl");
  player.primaryTintColorLoc =
      GetShaderLocation(player.colorShader, "primaryTintColor");
  player.primaryTargetColorLoc =
      GetShaderLocation(player.colorShader, "primaryTargetColor");
  player.secondaryTintColorLoc =
      GetShaderLocation(player.colorShader, "secondaryTintColor");
  player.secondaryTargetColorLoc =
      GetShaderLocation(player.colorShader, "secondaryTargetColor");
  player.toleranceLoc = GetShaderLocation(player.colorShader, "tolerance");

  return player;
}

const char *PlayerStateToString(PlayerState state) {
  switch (state) {
  case PLAYER_STANDING:
    return "STANDING";
  case PLAYER_STANDING_SHOOTING:
    return "STANDING_SHOOTING";
  case PLAYER_JUMPING:
    return "JUMPING";
  case PLAYER_JUMPING_SHOOTING:
    return "JUMPING_SHOOTING";
  case PLAYER_RUNNING:
    return "RUNNING";
  case PLAYER_RUNNING_SHOOTING:
    return "RUNNING_SHOOTING";
  case PLAYER_SLIDING:
    return "SLIDING";
  case PLAYER_CLIMBING:
    return "CLIMBING";
  case PLAYER_CLIMBING_SHOOTING:
    return "CLIMBING_SHOOTING";
  default:
    return "UNKNOWN";
  }
}

void ChangeNextWeapon(Player *player) {
  for (int i = player->currentWeapon + 1; i <= WEAPON_TOTAL; i++) {
    if (player->weapons[i].active) {
      player->currentWeapon = i;
      return;
    }
  }

  player->currentWeapon = WEAPON_DEFAULT;
}

Weapon *GetCurrentWeapon(Player *player, Weapon *weapons[WEAPON_TOTAL]) {
  return weapons[player->currentWeapon];
}
