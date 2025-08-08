#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "./platform.h"
#include "./animation.h"
#include "healthbar.h"
#include "weapons.h"

#define PLAYER_SPEED 200.0f
#define PLAYER_RUN_SPEED 350.0f
#define PLAYER_ACCELERATION 800.0f
#define PLAYER_FRICTION 600.0f
#define JUMP_SPEED -400.0f
#define MIN_JUMP_SPEED -200.0f
#define JUMP_RELEASE_FACTOR 0.3f
#define GRAVITY 400.0f
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
    Vector2           position;      // Center position of player
    Vector2           prevPosition;  // Previous center position
    Vector2           size;
    Vector2           origin;
    Vector2           velocity;
    Rectangle         collisionBox;  // Offset from center position
    Rectangle         hitBox;        // Offset from center position
    bool              onGround;
    bool              isJumping;
    Color             color;
    Texture2D         sprite;
    Vector2           spriteOrigin;
    PlayerState       state;
    bool              facingRight;
    AnimationTimeline timelines[PLAYER_TOTAL_STATES];
    HealthBar         healthbar;
    Shader            colorShader;
    WeaponType        currentWeapon;
    Weapon           *weapons;
    int               primaryTintColorLoc;
    int               secondaryTintColorLoc;
    int               primaryTargetColorLoc;
    int               secondaryTargetColorLoc;
    int               toleranceLoc;
} Player;

void      UpdatePlayer(Player *player, float deltaTime);
void      UpdatePlayerWeaponHealthbarColor(Player *player);
void      UpdatePlayerState(Player *player, float inputDirection, bool isRunning, bool isShooting);
void      DrawPlayer(Player player, bool debugMode);
void      MovePlayer(Player *player, Vector2 newPosition);
Rectangle GetPlayerPosition(Player *player);
void      CheckPlayerCollisions(Player *player, Platform platforms[], int platformCount);
Player    CreatePlayer(Texture2D sprite);
AnimationTimeline GetCurrentAnimationTimeline(Player *player);
bool              ShouldPlayerResetAnimationTimeline(Player *player, PlayerState newState);
void        CopyPlayerAnimationTimeline(Player *player, PlayerState newState, PlayerState oldState);
const char *PlayerStateToString(PlayerState state);
HealthBar   CreatePlayerWeaponHealthBar();
Weapon     *GetCurrentWeapon(Player *player, Weapon *weapons[WEAPON_TOTAL]);
void        ChangeNextWeapon(Player *player);

#endif
