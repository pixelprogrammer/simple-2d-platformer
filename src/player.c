#include "./player.h"
#include "./screen.c"
#include "animation.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

Vector2 PLAYER_SIZE = {22, 24};

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

    DrawAnimatedSprite(&player.sprite, &timeline, drawPosition, size, origin,
                       player.facingRight);
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
    player->state = newState;
    ResetAnimationTimeline(&player->timelines[player->state]);
  }

  printf("Player State [%s]", PlayerStateToString(player->state));
}

void UpdatePlayer(Player *player, float deltaTime) {
  bool gamepadConnected = IsGamepadAvailable(0);
  float inputDirection = 0.0f;

  if (gamepadConnected) {
    float axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    if (fabs(axisX) > 0.1f) {
      inputDirection = axisX;
    }

    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ||
        GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) < -0.1f) {
      inputDirection = -1.0f;
    }
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ||
        GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) > 0.1f) {
      inputDirection = 1.0f;
    }
  }

  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    inputDirection = -1.0f;
  }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    inputDirection = 1.0f;
  }

  bool isRunning = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT) ||
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);

  bool isShooting = IsKeyDown(KEY_X) || IsKeyDown(KEY_Z) ||
                    IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP);

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
      IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
  bool jumpKeyDown = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) ||
                     IsKeyDown(KEY_W) ||
                     IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);

  if (gamepadConnected) {
    jumpKeyPressed = jumpKeyPressed ||
                     IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    jumpKeyDown =
        jumpKeyDown || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
  }

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
        printf("Collision from top");
        // Collision from top (player landing on platform)
        if (player->velocity.y > 0) {
          player->position.y = platforms[i].rect.y + player->collisionBox.y;
          player->velocity.y = 0;
          player->onGround = true;
          player->isJumping = false;
        }
      } else if (overlapBottom < overlapTop && overlapBottom < overlapLeft &&
                 overlapBottom < overlapRight) {
        printf("Collision from bottom");
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
}

Player CreatePlayer(Texture2D sprite) {
  Player player = {.position = {111, 412},
                   .prevPosition = {111, 412},
                   .size = PLAYER_SIZE,
                   .origin = {0, 0},
                   .velocity = {0, 0},
                   .collisionBox = {-PLAYER_SIZE.x / 2, -PLAYER_SIZE.y / 2,
                                    PLAYER_SIZE.x, PLAYER_SIZE.y},
                   .hitBox = {PLAYER_SIZE.x / 2, PLAYER_SIZE.y / 2,
                              PLAYER_SIZE.x, PLAYER_SIZE.y},
                   .onGround = false,
                   .isJumping = false,
                   .color = BLUE,
                   .sprite = sprite,
                   .timelines = {
                       [PLAYER_STANDING] = {.loop = true,
                                            .position = {-11, -12},
                                            .frame = {102, 8, 22, 24},
                                            .frameGap = 8},
                       [PLAYER_STANDING_SHOOTING] =
                           {
                               .loop = true,
                           },
                       [PLAYER_JUMPING] = {.loop = true,
                                           .position = {-13, -15},
                                           .frame = {264, 3, 30, 30},
                                           .frameGap = 0},
                       [PLAYER_JUMPING_SHOOTING] = {.loop = true,
                                                    .position = {-13, -15},
                                                    .frame = {145, 39, 30, 30},
                                                    .frameGap = 0},
                       [PLAYER_RUNNING] = {.loop = true,
                                           .position = {-11, -12},
                                           .frame = {188, 8, 22, 24},
                                           .frameGap = 3},
                       [PLAYER_RUNNING_SHOOTING] = {},
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

  // Running
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING], 0, 0.25);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING], 1, 0.25);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING], 2, 0.25);
  AddAnimationFrame(&player.timelines[PLAYER_RUNNING], 1, 0.25);

  // Running and Shooting

  // Jumping

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
