#ifndef ENEMY_H
#define ENEMY_H

#include "actions.h"
#include "moveable.h"
#include "player.h"
#include "raylib.h"

typedef enum { ENEMY_SEEKER, ENEMY_TYPE_TOTAL } EnemyType;

typedef struct {
    MoveComponent    moveable;
    ActionsComponent actions;
    Vector2          size;
    float            rotation;
    float            rotationSpeed;
    Texture2D        sprite;
    Vector2          spriteOrigin;
    Rectangle        hitbox;
    bool             active;
    int              health;
    EnemyType        enemyType;
} EnemyEntity;

void  InitEnemy(EnemyEntity* enemy, Vector2 position, Texture2D sprite);
void  UpdateEnemy(EnemyEntity* enemy, PlayerEntity* player, float deltaTime);
void  DrawEnemy(EnemyEntity enemy);
bool  CheckEnemyCollision(EnemyEntity enemy, Rectangle other);
void  DestroyEnemy(EnemyEntity* enemy);
char* EnemyTypeToString(EnemyType type);
void  CheckProjectileToEnemyCollisions(EnemyEntity* enemy, ProjectileArray* a);

void EnemyAISystem(EnemyEntity* enemy, PlayerEntity* player, float deltaTime);
void EnemySeekerAI(EnemyEntity* enemy, PlayerEntity* player, float deltaTime);
#endif
