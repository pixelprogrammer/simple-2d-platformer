#include "./player.h"
#include "./screen.c"
#include "actions.h"
#include "animation.h"
#include "enemy.h"
#include "fallable.h"
#include "healthbar.h"
#include "screen.h"
#include "sound.h"
#include "stunnable.h"
#include "weapons.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

#define MAX_GAMEPADS 4
#define MAX_BUSTER_SHOTS 3

Vector2 PLAYER_SIZE = {22, 24};

Vector3 PLAYER_PRIMARY_COLOR = (Vector3){0.0f, 0.45f, 0.95f};
Vector3 PLAYER_SECONDARY_COLOR = (Vector3){0.0f, 1.0f, 1.0f};
float PLAYER_SHADER_COLOR_SWAP_TOLERANCE = 0.1f;

void MovePlayer(PlayerEntity *player, float deltaTime) {
  SystemMoveEntity(&player->moveable, deltaTime);

  // update players collisionbox to player position
  if (player->moveable.position.x + player->collisionBox.x < 0) {
    player->moveable.position.x = -player->collisionBox.x;
  }

  // keep player within screen bounds
  if (player->moveable.position.x + player->collisionBox.x +
          player->collisionBox.width >
      SCREEN_WIDTH) {
    player->moveable.position.x =
        SCREEN_WIDTH - player->collisionBox.x - player->collisionBox.width;
  }
}

AnimationTimeline GetCurrentAnimationTimeline(PlayerEntity *player) {
  return player->timelines[player->state];
}

void DrawPlayer(PlayerEntity player, bool debugMode) {
  bool shouldDraw = true;
  if (player.stun.invincibilityTime > 0) {
    int iFrameTime = player.stun.invincibilityTime * 1000;
    int iFrame = iFrameTime % 200;

    if (iFrame < 100) {
      shouldDraw = false;
    }
  }

  if (shouldDraw) {

    if (player.sprite.id != 0) {

      AnimationTimeline timeline = GetCurrentAnimationTimeline(&player);
      Vector2 size = {timeline.frame.width, timeline.frame.height};
      Vector2 drawPosition = {player.moveable.position.x + timeline.position.x,
                              player.moveable.position.y + timeline.position.y};
      Vector2 origin =
          (Vector2){timeline.frame.width / 2 + timeline.position.x,
                    timeline.frame.height / 2 + timeline.position.y};
      bool useShader = false;
      if (player.currentWeapon != WEAPON_BUSTER && player.colorShader.id != 0) {
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
                       &PLAYER_SHADER_COLOR_SWAP_TOLERANCE,
                       SHADER_UNIFORM_FLOAT);
      }

      DrawAnimatedSprite(&player.sprite, &timeline, drawPosition, size, origin,
                         player.facingRight);

      if (useShader) {
        EndShaderMode();
      }
    } else {
      Vector2 drawPosition = {player.moveable.position.x - player.size.x / 2,
                              player.moveable.position.y - player.size.y / 2};
      Rectangle playerRect = {drawPosition.x, drawPosition.y, player.size.x,
                              player.size.y};
      DrawRectangleRec(playerRect, player.color);
    }
  }

  if (debugMode) {
    // Rectangle hitBoxRect = {player.moveable.position.x + player.hitBox.x,
    //                         player.moveable.position.y + player.hitBox.y,
    //                         player.hitBox.width, player.hitBox.height};
    // DrawRectangleLinesEx(hitBoxRect, 1.0f, RED);

    Rectangle collisionBoxRect = GetPlayerPosition(&player);

    DrawRectangleLinesEx(collisionBoxRect, 1.0f, GREEN);
    DrawCircle(collisionBoxRect.x + collisionBoxRect.width / 2,
               collisionBoxRect.y + collisionBoxRect.height / 2, 2, RED);
  }
}

void UpdatePlayerState(PlayerEntity *player) {
  PlayerState newState = player->state;

  if (player->state == PLAYER_HURT && player->stun.stunTime > 0) {
    // no need to change state
    return;
  }

  bool isPlayerInShootingState =
      player->canShoot && player->actions.shootButtonDown;
  if (player->shootingStateDelay > 0.0f) {
    isPlayerInShootingState = true;
  }

  if (!player->onGround) {
    newState =
        isPlayerInShootingState ? PLAYER_JUMPING_SHOOTING : PLAYER_JUMPING;
  } else if (fabs(player->actions.axisDirection.x) > 0.1f) {
    newState =
        isPlayerInShootingState ? PLAYER_RUNNING_SHOOTING : PLAYER_RUNNING;
  } else {
    newState =
        isPlayerInShootingState ? PLAYER_STANDING_SHOOTING : PLAYER_STANDING;
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
void CopyPlayerAnimationTimeline(PlayerEntity *player, PlayerState newState,
                                 PlayerState oldState) {
  player->timelines[newState].currentFrameIndex =
      player->timelines[oldState].currentFrameIndex;
  player->timelines[newState].currentTime =
      player->timelines[oldState].currentTime;
}

bool ShouldPlayerResetAnimationTimeline(PlayerEntity *player,
                                        PlayerState newState) {
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

void ShootWeapon(PlayerEntity *player) {
  if (!player->canShoot) {
    return;
  }

  int direction = 1; // facing right
  if (!player->facingRight) {
    direction = -1;
  }

  Vector2 projectilePos = (Vector2){
      .x = player->moveable.position.x + (16 * direction),
      .y = player->moveable.position.y,
  };

  SpawnProjectile(&player->projectiles, player->projectileTexture,
                  projectilePos, player->currentWeapon, direction);
  PlaySoundEffect(SOUND_SHOOT);

  player->canShoot = false; // when the button is released set this to true
  player->shootingStateDelay = PLAYER_SHOOTING_STATE_TIME;
}

void HandleShooting(PlayerEntity *player, float deltaTime) {
  // update shooting delay
  if (player->shootingStateDelay > 0.0f) {
    player->shootingStateDelay -= deltaTime;
    if (player->shootingStateDelay < 0.0f) {
      player->shootingStateDelay = 0;
    }
  }

  if (player->actions.shootButtonReleased) {
    player->canShoot = true;
  }

  player->canShoot = CanPlayerShoot(player);

  if (!player->canShoot) {
    return;
  }

  if (!player->actions.shootButtonDown) {
    // no need to do anything else
    return;
  }

  ShootWeapon(player);
}

void HandleJump(PlayerEntity *player, float deltaTime) {
  if (player->actions.jumpKeyPressed && player->onGround) {
    player->moveable.velocity.y = player->jump.jumpSpeed;
    player->isJumping = true;
  }

  // Continue the height of the jump slightly after release
  if (player->isJumping && !player->actions.jumpKeyDown) {
    player->moveable.velocity.y *= player->jump.jumpReleaseFactor;
    player->isJumping = false;
  }
}

bool CanPlayerShoot(PlayerEntity *player) {
  if (!player->canShoot) {
    return false;
  }

  // check if player has max projectiles
  if (player->projectiles.length >= MAX_BUSTER_SHOTS) {
    return false;
  }

  return true;
}

void UpdatePlayer(PlayerEntity *player, float deltaTime) {

  // handle hurt state
  UpdateStunSystem(&player->stun, deltaTime);

  // NOTE: Friction logic
  //   else {
  //   if (player->moveable.velocity.x > 0) {
  //     player->moveable.velocity.x -= PLAYER_FRICTION * deltaTime;
  //     if (player->moveable.velocity.x < 0)
  //       player->moveable.velocity.x = 0;
  //   } else if (player->moveable.velocity.x < 0) {
  //     player->moveable.velocity.x += PLAYER_FRICTION * deltaTime;
  //     if (player->moveable.velocity.x > 0)
  //       player->moveable.velocity.x = 0;
  //   }
  // }

  if (!IsPlayerStunned(player)) {
    if (player->actions.axisDirection.x != 0.0f) {
      player->moveable.velocity.x =
          player->actions.axisDirection.x * player->moveable.speed.x;
      player->facingRight = (player->actions.axisDirection.x > 0);
    } else {
      player->moveable.velocity.x = 0;
    }
    if (player->actions.changeWeaponNextPressed) {
      ChangeNextWeapon(player);
    }
    HandleShooting(player, deltaTime);
    HandleJump(player, deltaTime);
  }

  FallSystem(&player->fall, &player->moveable);

  MovePlayer(player, deltaTime);

  UpdatePlayerState(player);
  PlayAnimationTimeline(&player->timelines[player->state], deltaTime);

  player->onGround = false;
}

Rectangle GetPlayerPosition(PlayerEntity *player) {
  return (Rectangle){player->moveable.position.x + player->collisionBox.x,
                     player->moveable.position.y + player->collisionBox.y,
                     player->collisionBox.width, player->collisionBox.height};
}

void CheckPlayerCollisions(PlayerEntity *player, Platform platforms[],
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
        if (player->moveable.velocity.y > 0) {
          player->moveable.position.y =
              platforms[i].rect.y + player->collisionBox.y;
          player->moveable.velocity.y = 0;
          player->onGround = true;
          player->isJumping = false;
          groundedPlatformIndex = i;
        }
      } else if (overlapBottom < overlapTop && overlapBottom < overlapLeft &&
                 overlapBottom < overlapRight) {
        // Collision from bottom (player hitting platform from below)
        if (player->moveable.velocity.y < 0) {
          player->moveable.position.y = platforms[i].rect.y +
                                        platforms[i].rect.height -
                                        player->collisionBox.y;
          player->moveable.velocity.y = 0;
        }
      } else if (overlapLeft < overlapRight) {
        // Collision from left (player moving right into platform)
        if (player->moveable.velocity.x > 0) {
          player->moveable.position.x =
              platforms[i].rect.x + player->collisionBox.x;
          player->moveable.velocity.x = 0;
        }
      } else {
        // Collision from right (player moving left into platform)
        if (player->moveable.velocity.x < 0) {
          player->moveable.position.x = platforms[i].rect.x +
                                        platforms[i].rect.width -
                                        player->collisionBox.x;
          player->moveable.velocity.x = 0;
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
      player->moveable.position.x +=
          groundedPlatform->velocity.x * GetFrameTime();
      player->moveable.position.y +=
          groundedPlatform->velocity.y * GetFrameTime();
    }
  }

  // Reset grounded platform if player is no longer on ground
  if (!player->onGround) {
    groundedPlatformIndex = -1;
  }
}

PlayerEntity CreatePlayer(Texture2D sprite, Texture2D projectileTexture) {
  PlayerEntity player = {
      .moveable =
          {
              .position = {111, 412},
              .prevPosition = {111, 412},
              .velocity = {0, 0},
              .speed = {PLAYER_SPEED, 0},
          },
      .controller =
          {
              .gamepadId = -1,
          },
      .actions =
          {
              .axisDirection = (Vector2){0, 0},
              .jumpKeyDown = false,
              .jumpKeyPressed = false,
              .shootButtonDown = false,
              .shootButtonReleased = false,
              .changeWeaponNextPressed = false,
              .changeWeaponPrevPressed = false,
              .moveAxis = (Vector2){0, 0},
          },
      .jump =
          {
              .isJumping = false,
              .jumpReleaseFactor = JUMP_RELEASE_FACTOR,
              .jumpSpeed = JUMP_SPEED,
              .onGround = false,
          },
      .fall =
          {
              .maxFallSpeed = MAX_FALL_SPEED,
              .gravity = GRAVITY,
          },
      .stun =
          {
              .invincibilityTime = 0.0f,
              .stunTime = 0.0f,
          },
      .size = PLAYER_SIZE,
      .origin = {0, 0},
      .collisionBox =
          {
              -PLAYER_SIZE.x / 2,
              -PLAYER_SIZE.y / 2,
              PLAYER_SIZE.x,
              PLAYER_SIZE.y,
          },
      .hitBox =
          {
              PLAYER_SIZE.x / 2,
              PLAYER_SIZE.y / 2,
              PLAYER_SIZE.x,
              PLAYER_SIZE.y,
          },
      .onGround = false,
      .isJumping = false,
      .canShoot = true,
      .shootingStateDelay = 0.0f,
      .color = BLUE,
      .sprite = sprite,
      .projectileTexture = projectileTexture,
      .currentWeapon = WEAPON_BUSTER,
      .weapons = CreateWeaponsArray(),
      .projectiles = {.length = 0},
      .healthbar = CreateHealthBar(
          (Vector2){50, 100}, PLAYER_HEALTHBAR_MAX_HEALTH,
          PLAYER_HEALTHBAR_SEGMENT_WIDTH, PLAYER_HEALTHBAR_SEGMENT_HEIGHT),
      .timelines = {
          [PLAYER_STANDING] =
              {
                  .loop = true,
                  .position = {-11, -12},
                  .frame = {0, 0, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE},
                  .frameGap = 0,
              },
          [PLAYER_STANDING_SHOOTING] =
              {
                  .loop = true,
                  .position = {-11, -12},
                  .frame = {80, 0, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE},
                  .frameGap = 0,
              },
          [PLAYER_RUNNING] =
              {
                  .loop = true,
                  .position = {-11, -11},
                  .frame = {0, 40, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE},
                  .frameGap = 0,
              },
          [PLAYER_RUNNING_SHOOTING] =
              {
                  .loop = true,
                  .position = {-11, -11},
                  .frame = {0, 80, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE},
                  .frameGap = 0,
              },
          [PLAYER_JUMPING] =
              {
                  .loop = true,
                  .position = {-13, -15},
                  .frame = {0, 120, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE},
                  .frameGap = 0,
              },
          [PLAYER_JUMPING_SHOOTING] =
              {
                  .loop = true,
                  .position = {-13, -15},
                  .frame = {0, 160, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE},
                  .frameGap = 0,
              },
          [PLAYER_CLIMBING] = {},
          [PLAYER_CLIMBING_SHOOTING] = {},
          [PLAYER_SLIDING] = {},
          [PLAYER_HURT] =
              {
                  .loop = true,
                  .position = {-11, -11},
                  .frame = {0, 320, PLAYER_FRAME_SIZE, PLAYER_FRAME_SIZE},
                  .frameGap = 0,
              },

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

  // Getting hit
  AddAnimationFrame(&player.timelines[PLAYER_HURT], 0,
                    PLAYER_HURT_FRAME_DURATION);
  AddAnimationFrame(&player.timelines[PLAYER_HURT], 1,
                    PLAYER_HURT_FRAME_DURATION);

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
  case PLAYER_HURT:
    return "PLAYER_HURT";
  default:
    return "UNKNOWN";
  }
}

bool IsPlayerStunned(PlayerEntity *player) {
  return player->state == PLAYER_HURT && player->stun.stunTime > 0;
}

void CheckPlayerHurt(PlayerEntity *player, Rectangle hitbox) {

  if (player->stun.invincibilityTime > 0) {
    return;
  }

  Rectangle playerRect = GetPlayerPosition(player);

  if (CheckCollisionRecs(playerRect, hitbox)) {
    // change state to hit
    // TODO: Update damage to enemy damage
    HurtPlayer(player, 5);
  }
}

void HurtPlayer(PlayerEntity *player, int damage) {

  player->state = PLAYER_HURT;
  SetHealthBar(&player->healthbar, player->healthbar.currentHealth - damage);
  if (player->healthbar.currentHealth <= 0) {
    // TODO: Kill MegaMan
    return;
  }

  // set the velocity to move the character back and down
  if (player->facingRight) {
    player->moveable.velocity.x = -PLAYER_HURT_SPEED;
  } else {
    player->moveable.velocity.x = PLAYER_HURT_SPEED;
  }

  player->moveable.velocity.y = 0;
  player->stun.invincibilityTime = PLAYER_HURT_INVINCIBILITY_TIME;
  player->stun.stunTime = PLAYER_STUN_TIME;
}

void ChangeNextWeapon(PlayerEntity *player) {
  for (int i = player->currentWeapon + 1; i <= WEAPON_TOTAL; i++) {
    if (player->weapons[i].active) {
      player->currentWeapon = i;
      return;
    }
  }

  player->currentWeapon = WEAPON_BUSTER;
}

Weapon *GetCurrentWeapon(PlayerEntity *player, Weapon *weapons[WEAPON_TOTAL]) {
  return weapons[player->currentWeapon];
}

void PlayerControllerSystem(PlayerControllerComponent *controller,
                            ActionsComponent *actions) {
  // reset direction state
  actions->axisDirection = (Vector2){0, 0};

  bool gamepadConnected = IsGamepadAvailable(controller->gamepadId);
  if (!gamepadConnected) {
    // try to detect a controller
    for (int i = 0; i < MAX_GAMEPADS; i++) {
      if (IsGamepadAvailable(i) &&
          IsGamepadButtonDown(i, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        // set the gamepadId
        controller->gamepadId = i;
        gamepadConnected = true;
        break;
      }
    }
  }

  if (!gamepadConnected) {
    // we tried to connect, still don't have a connection
    return;
  }

  // detect player x axis movement
  float axisX =
      GetGamepadAxisMovement(controller->gamepadId, GAMEPAD_AXIS_LEFT_X);
  if (fabs(axisX) > 0.1f) {
    actions->axisDirection.x = axisX;
  }

  if (IsGamepadButtonDown(controller->gamepadId,
                          GAMEPAD_BUTTON_LEFT_FACE_LEFT) ||
      GetGamepadAxisMovement(controller->gamepadId, GAMEPAD_AXIS_LEFT_X) <
          -0.1f) {
    actions->axisDirection.x = -1.0f;
  }
  if (IsGamepadButtonDown(controller->gamepadId,
                          GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ||
      GetGamepadAxisMovement(controller->gamepadId, GAMEPAD_AXIS_LEFT_X) >
          0.1f) {
    actions->axisDirection.x = 1.0f;
  }
  // keyboard detection takes priority over controller
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
    actions->axisDirection.x = -1.0f;
  }
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
    actions->axisDirection.x = 1.0f;
  }

  // TODO: add the y axis here. Might be needed for controlling weapons
  actions->shootButtonDown =
      (IsKeyDown(KEY_X) || IsKeyDown(KEY_Z) ||
       IsGamepadButtonDown(controller->gamepadId,
                           GAMEPAD_BUTTON_RIGHT_FACE_LEFT));

  // NOTE: This being before the shooting mechanic might cause some fun things
  // if multiple buttons can shoot
  actions->shootButtonReleased = IsGamepadButtonReleased(
      controller->gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);

  // change weapon/colormode
  // TODO: Change weapon
  actions->changeWeaponNextPressed =
      IsKeyPressed(KEY_C) ||
      IsGamepadButtonPressed(controller->gamepadId,
                             GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);

  actions->jumpKeyPressed =
      IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) ||
      IsGamepadButtonPressed(controller->gamepadId,
                             GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
  actions->jumpKeyDown = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) ||
                         IsKeyDown(KEY_W) ||
                         IsGamepadButtonDown(controller->gamepadId,
                                             GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
}
