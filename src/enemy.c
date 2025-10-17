#include "enemy.h"
#include "moveable.h"
#include "player.h"
#include "weapons.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

void InitEnemy(EnemyEntity *enemy, Vector2 position, Texture2D sprite) {
  enemy->moveable = (MoveComponent){
      .position = position,
      .prevPosition = position,
      .velocity = (Vector2){0.0f, 0.0f},
      .speed = (Vector2){42.0f, 42.0f},
  };
  enemy->size = (Vector2){32.0f, 32.0f};
  enemy->rotation = 0.0f;
  enemy->rotationSpeed = 0.0f;
  enemy->sprite = sprite;
  enemy->spriteOrigin = (Vector2){enemy->size.x / 2, enemy->size.y / 2};
  enemy->hitbox = (Rectangle){
      position.x - enemy->size.x / 2,
      position.y - enemy->size.y / 2,
      enemy->size.x,
      enemy->size.y,
  };
  enemy->active = true;
  enemy->health = 1;
  enemy->enemyType = ENEMY_SEEKER;
}

void UpdateEnemy(EnemyEntity *enemy, PlayerEntity *player, float deltaTime) {
  EnemyAISystem(enemy, player, deltaTime);
  SystemMoveEntity(&enemy->moveable, deltaTime);
}

void EnemySeekerAI(EnemyEntity *enemy, PlayerEntity *player, float deltaTime) {

  enemy->rotation += enemy->rotationSpeed * deltaTime;

  // Calculate direction to player
  Vector2 direction = {
      player->moveable.position.x - enemy->moveable.position.x,
      player->moveable.position.y - enemy->moveable.position.y,
  };

  // Normalize direction vector
  float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
  if (distance > 0) {
    direction.x /= distance;
    direction.y /= distance;
  }

  // Set velocity towards player
  enemy->moveable.velocity.x = direction.x * enemy->moveable.speed.x;
  enemy->moveable.velocity.y = direction.y * enemy->moveable.speed.y;

  enemy->hitbox.x = enemy->moveable.position.x - enemy->size.x / 2;
  enemy->hitbox.y = enemy->moveable.position.y - enemy->size.y / 2;
}

void EnemyAISystem(EnemyEntity *enemy, PlayerEntity *player, float deltaTime) {
  switch (enemy->enemyType) {
  case ENEMY_SEEKER:
    EnemySeekerAI(enemy, player, deltaTime);
    break;
  default:
    printf("No AI system for enemy type: %s\n",
           EnemyTypeToString(enemy->enemyType));
    return;
  }
}

void DrawEnemy(EnemyEntity enemy) {
  if (!enemy.active)
    return;

  // if (enemy.sprite.id > 0) {
  //   DrawTexturePro(enemy.sprite,
  //                  (Rectangle){0, 0, enemy.sprite.width,
  //                  enemy.sprite.height}, (Rectangle){enemy.position.x,
  //                  enemy.position.y, enemy.size.x,
  //                              enemy.size.y},
  //                  enemy.spriteOrigin, enemy.rotation, WHITE);
  // } else {

  Vector2 drawPosition = {
      enemy.moveable.position.x - enemy.size.x / 2,
      enemy.moveable.position.y - enemy.size.y / 2,
  };
  DrawRectangleV(drawPosition, enemy.size, RED);
  DrawRectangleV((Vector2){.x = enemy.hitbox.x, .y = enemy.hitbox.y},
                 enemy.size, BLUE);
  // }
}

bool CheckEnemyCollision(EnemyEntity enemy, Rectangle other) {
  if (!enemy.active)
    return false;

  return CheckCollisionRecs(enemy.hitbox, other);
}

void CheckProjectileToEnemyCollisions(EnemyEntity *enemy, ProjectileArray *a) {
  for (int i = a->length; i >= 0; i--) {
    Projectile projectile = a->projectiles[i];

    if (CheckEnemyCollision(*enemy, projectile.hitBox)) {
      // the projectile hit the enemy
      enemy->health -= projectile.damage;
      DestroyProjectile(a, i);

      if (enemy->health <= 0) {
        // kill the enemy
        // TODO: remove enemy from the array, maybe later
        DestroyEnemy(enemy);
      }
    }
  }
}

void DestroyEnemy(EnemyEntity *enemy) {
  enemy->active = false;
  enemy->health = 0;
}

char *EnemyTypeToString(EnemyType type) {
  switch (type) {
  case ENEMY_SEEKER:
    return "Enemy Seeker";
  default:
    return "Unknown enemy type";
  }
}
