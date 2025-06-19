#include "./player.h"
#include "./screen.c"
#include "animation.h"
#include <math.h>
#include <raylib.h>

void MovePlayer(Player *player, Vector2 newPosition) {
  player->position.x += newPosition.x;
  player->position.y += newPosition.y;
  player->rect.x = player->position.x;
  player->rect.y = player->position.y;
}

AnimationTimeline GetCurrentAnimationTimeline(Player *player) {
  return player->timelines[player->state];
}

void DrawPlayer(Player player) {
  if (player.sprite.id != 0) {
    AnimationTimeline timeline = GetCurrentAnimationTimeline(&player);

    Vector2 size = {player.rect.width, player.rect.height};

    DrawAnimatedSprite(&player.sprite, &timeline, player.position, size,
                       player.facingRight);
    // int currentFrame = GetCurrentAnimationFrame(&timeline);
    //
    // Rectangle sourceRec = {
    //     currentFrame * timeline.frame.width + timeline.frame.x,
    //     timeline.frame.y,
    //     player.facingRight ? timeline.frame.width : -timeline.frame.width,
    //     timeline.frame.height};
    // Rectangle destRec = {player.position.x, player.position.y,
    //                      player.rect.width, player.rect.height};
    // Vector2 origin = {0, 0};
    // DrawTexturePro(player.sprite, sourceRec, destRec, origin, 0.0f, WHITE);
  } else {
    DrawRectangleRec(player.rect, player.color);
  }
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

  // if (player->sprite.id != 0 && player->totalFrames > 1) {
  //   player->frameTimer += deltaTime;
  //   if (player->frameTimer >= player->frameSpeed) {
  //     player->currentFrame++;
  //     if (player->currentFrame >= player->totalFrames) {
  //       player->currentFrame = 0;
  //     }
  //     player->frameTimer = 0.0f;
  //   }
  // }

  Vector2 moveVelocity = {player->velocity.x * deltaTime,
                          player->velocity.y * deltaTime};
  MovePlayer(player, moveVelocity);

  // player->position.x += player->velocity.x * deltaTime;
  // player->position.y += player->velocity.y * deltaTime;
  //
  // player->rect.x = player->position.x;
  // player->rect.y = player->position.y;

  if (player->position.x < 0) {
    player->position.x = 0;
    player->rect.x = 0;
  }
  if (player->position.x > screen_width - player->rect.width) {
    player->position.x = screen_width - player->rect.width;
    player->rect.x = screen_width - player->rect.width;
  }

  PlayAnimationTimeline(&player->timelines[player->state], deltaTime);

  player->onGround = false;
}

void CheckPlayerCollisions(Player *player, Platform platforms[],
                           int platformCount) {
  for (int i = 0; i < platformCount; i++) {
    if (CheckCollisionRecs(player->rect, platforms[i].rect)) {

      // Calculate overlap amounts
      float overlapLeft =
          (player->rect.x + player->rect.width) - platforms[i].rect.x;
      float overlapRight =
          (platforms[i].rect.x + platforms[i].rect.width) - player->rect.x;
      float overlapTop =
          (player->rect.y + player->rect.height) - platforms[i].rect.y;
      float overlapBottom =
          (platforms[i].rect.y + platforms[i].rect.height) - player->rect.y;

      // Find minimum overlap to determine collision direction
      if (overlapTop < overlapBottom && overlapTop < overlapLeft &&
          overlapTop < overlapRight) {
        // Collision from top (player landing on platform)
        if (player->velocity.y > 0) {
          player->position.y = platforms[i].rect.y - player->rect.height;
          player->velocity.y = 0;
          player->onGround = true;
          player->isJumping = false;
        }
      } else if (overlapBottom < overlapTop && overlapBottom < overlapLeft &&
                 overlapBottom < overlapRight) {
        // Collision from bottom (player hitting platform from below)
        if (player->velocity.y < 0) {
          player->position.y = platforms[i].rect.y + platforms[i].rect.height;
          player->velocity.y = 0;
        }
      } else if (overlapLeft < overlapRight) {
        // Collision from left (player moving right into platform)
        if (player->velocity.x > 0) {
          player->position.x = platforms[i].rect.x - player->rect.width;
          player->velocity.x = 0;
        }
      } else {
        // Collision from right (player moving left into platform)
        if (player->velocity.x < 0) {
          player->position.x = platforms[i].rect.x + platforms[i].rect.width;
          player->velocity.x = 0;
        }
      }

      // Update rect position after collision resolution
      player->rect.x = player->position.x;
      player->rect.y = player->position.y;
    }
  }
}

Player CreatePlayer(Texture2D sprite) {
  Player player = {.position = {100, 400},
                   .velocity = {0, 0},
                   .rect = {100, 400, 21, 25},
                   .onGround = false,
                   .isJumping = false,
                   .color = BLUE,
                   .sprite = sprite,
                   .timelines = {
                       [PLAYER_STANDING] = {.loop = true,
                                            .frame = {2, 40, 21, 25},
                                            .frameGap = 4},
                       [PLAYER_STANDING_SHOOTING] = {.loop = true},
                       [PLAYER_JUMPING] = {},
                       [PLAYER_JUMPING_SHOOTING] = {},
                       [PLAYER_RUNNING] = {.loop = true},
                       [PLAYER_RUNNING_SHOOTING] = {},
                       [PLAYER_SLIDING] = {},
                       [PLAYER_CLIMBING] = {},
                       [PLAYER_CLIMBING_SHOOTING] = {},

                   }};

  // add the frames to each timeline
  // Standing and blinking
  AddAnimationFrame(&player.timelines[PLAYER_STANDING], 0, 5);
  AddAnimationFrame(&player.timelines[PLAYER_STANDING], 1, 0.05);
  AddAnimationFrame(&player.timelines[PLAYER_STANDING], 0, 0.15);
  AddAnimationFrame(&player.timelines[PLAYER_STANDING], 1, 0.05);

  return player;
}
