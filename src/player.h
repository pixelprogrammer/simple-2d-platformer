#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "./platform.h"
#include "./animation.h"

#define PLAYER_SPEED 200.0f
#define PLAYER_RUN_SPEED 350.0f
#define PLAYER_ACCELERATION 800.0f
#define PLAYER_FRICTION 600.0f
#define JUMP_SPEED -400.0f
#define MIN_JUMP_SPEED -200.0f
#define JUMP_RELEASE_FACTOR 0.3f
#define GRAVITY 800.0f

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

typedef struct {
    Vector2           position;
    Vector2           velocity;
    Rectangle         rect;
    bool              onGround;
    bool              isJumping;
    Color             color;
    Texture2D         sprite;
    PlayerState       state;
    bool              facingRight;
    AnimationTimeline timelines[PLAYER_TOTAL_STATES];
} Player;

void              UpdatePlayer(Player *player, float deltaTime);
void              DrawPlayer(Player player);
void              MovePlayer(Player *player, Vector2 newPosition);
void              CheckPlayerCollisions(Player *player, Platform platforms[], int platformCount);
Player            CreatePlayer(Texture2D sprite);
AnimationTimeline GetCurrentAnimationTimeline(Player *player);
#endif
