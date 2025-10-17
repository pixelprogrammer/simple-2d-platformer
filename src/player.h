#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "./platform.h"
#include "./animation.h"
#include "actions.h"
#include "fallable.h"
#include "healthbar.h"
#include "jumpable.h"
#include "moveable.h"
#include "stunnable.h"
#include "weapons.h"

#define PLAYER_SPEED 82.5f
#define PLAYER_RUN_SPEED 350.0f
#define PLAYER_ACCELERATION 800.0f
#define PLAYER_FRICTION 600.0f
// -285 is the default but with the gravity pull down we add the gravity 15
#define JUMP_SPEED -307.5f
// with normal gravity
#define JUMP_PEAK_HEIGHT 50.0f
#define MIN_JUMP_SPEED -200.0f
#define JUMP_RELEASE_FACTOR 0.3f
#define GRAVITY 15.0f
#define MAX_FALL_SPEED 420.0f

#define PLAYER_HURT_SPEED 20.0f

#define PLAYER_FRAME_SIZE 40
#define PLAYER_RUNNING_FRAME_DURATION 0.25
#define PLAYER_HURT_FRAME_DURATION 0.1
#define PLAYER_STUN_TIME 0.4
#define PLAYER_HURT_INVINCIBILITY_TIME 1.5

// in seconds
#define PLAYER_SHOOTING_STATE_TIME 0.3f
#define PLAYER_HURT_STATE_TIME 0.3f

#define PLAYER_HEALTHBAR_MAX_HEALTH 28
#define PLAYER_HEALTHBAR_SEGMENT_WIDTH 6
#define PLAYER_HEALTHBAR_SEGMENT_HEIGHT 1

typedef enum {
    PLAYER_STANDING,
    PLAYER_STANDING_SHOOTING,
    PLAYER_JUMPING,
    PLAYER_JUMPING_SHOOTING,
    PLAYER_RUNNING,
    PLAYER_RUNNING_SHOOTING,
    PLAYER_SLIDING,
    PLAYER_CLIMBING,
    PLAYER_CLIMBING_SHOOTING,
    PLAYER_HURT,
    PLAYER_TOTAL_STATES,
} PlayerState;

typedef enum {
    COLOR_NORMAL,
    COLOR_PINK,
    COLOR_PURPLE,
    COLOR_RED,
    COLOR_FIRE,
    COLOR_TOTAL
} PlayerColorMode;

typedef struct {
    int gamepadId;
} PlayerControllerComponent;

typedef struct {
    MoveComponent             moveable;
    PlayerControllerComponent controller;
    ActionsComponent          actions;
    StunComponent             stun;
    JumpComponent             jump;
    FallComponent             fall;
    Vector2                   size;
    Vector2                   origin;
    Rectangle                 collisionBox;  // Offset from center position
    Rectangle                 hitBox;        // Offset from center position
    bool                      onGround;
    bool                      isJumping;
    bool                      canShoot;
    float                     shootingStateDelay;
    Color                     color;
    Texture2D                 sprite;
    Vector2                   spriteOrigin;
    Texture2D                 projectileTexture;
    PlayerState               state;
    bool                      facingRight;
    AnimationTimeline         timelines[PLAYER_TOTAL_STATES];
    HealthBar                 healthbar;
    Shader                    colorShader;
    WeaponType                currentWeapon;
    Weapon                   *weapons;
    ProjectileArray           projectiles;
    int                       primaryTintColorLoc;
    int                       secondaryTintColorLoc;
    int                       primaryTargetColorLoc;
    int                       secondaryTargetColorLoc;
    int                       toleranceLoc;
} PlayerEntity;

void         UpdatePlayer(PlayerEntity *player, float deltaTime);
void         UpdatePlayerWeaponHealthbarColor(PlayerEntity *player);
void         UpdatePlayerState(PlayerEntity *player);
void         DrawPlayer(PlayerEntity player, bool debugMode);
void         MovePlayer(PlayerEntity *player, float deltaTime);
void         HandleJump(PlayerEntity *player, float deltaTime);
void         HandleShooting(PlayerEntity *player, float deltaTime);
void         CheckPlayerHurt(PlayerEntity *player, Rectangle hitbox);
void         HurtPlayer(PlayerEntity *player, int damage);
bool         IsPlayerStunned(PlayerEntity *player);
Rectangle    GetPlayerPosition(PlayerEntity *player);
void         CheckPlayerCollisions(PlayerEntity *player, Platform platforms[], int platformCount);
PlayerEntity CreatePlayer(Texture2D sprite, Texture2D projectileTexture);
AnimationTimeline GetCurrentAnimationTimeline(PlayerEntity *player);
bool              ShouldPlayerResetAnimationTimeline(PlayerEntity *player, PlayerState newState);
void CopyPlayerAnimationTimeline(PlayerEntity *player, PlayerState newState, PlayerState oldState);
const char *PlayerStateToString(PlayerState state);
HealthBar   CreatePlayerWeaponHealthBar();
Weapon     *GetCurrentWeapon(PlayerEntity *player, Weapon *weapons[WEAPON_TOTAL]);
void        ChangeNextWeapon(PlayerEntity *player);
bool        CanPlayerShoot(PlayerEntity *player);
void PlayerControllerSystem(PlayerControllerComponent *controller, ActionsComponent *actions);

#endif
