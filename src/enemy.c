#include "enemy.h"
#include <math.h>
#include <raylib.h>
#include <stdlib.h>

void InitEnemy(Enemy *enemy, Vector2 position, Texture2D sprite) {
  enemy->position = position;
  enemy->velocity = (Vector2){0.0f, 0.0f};
  enemy->size = (Vector2){32.0f, 32.0f};
  enemy->rotation = 0.0f;
  enemy->rotationSpeed = 0.0f;
  enemy->sprite = sprite;
  enemy->spriteOrigin = (Vector2){enemy->size.x / 2, enemy->size.y / 2};
  enemy->hitbox =
      (Rectangle){position.x - enemy->size.x / 2,
                  position.y - enemy->size.y / 2, enemy->size.x, enemy->size.y};
  enemy->active = true;
  enemy->speed = 42.0f;
  enemy->health = 1;
}

void UpdateEnemy(Enemy *enemy, Vector2 playerPosition, float deltaTime) {
  if (!enemy->active) {
    // respawn for now
    enemy->active = true;
    enemy->position = (Vector2){75, 75};
    return;
  }

  enemy->rotation += enemy->rotationSpeed * deltaTime;

  // Calculate direction to player
  Vector2 direction = {playerPosition.x - enemy->position.x,
                       playerPosition.y - enemy->position.y};

  // Normalize direction vector
  float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
  if (distance > 0) {
    direction.x /= distance;
    direction.y /= distance;
  }

  // Set velocity towards player
  enemy->velocity.x = direction.x * enemy->speed;
  enemy->velocity.y = direction.y * enemy->speed;

  enemy->position.x += enemy->velocity.x * deltaTime;
  enemy->position.y += enemy->velocity.y * deltaTime;

  enemy->hitbox.x = enemy->position.x - enemy->size.x / 2;
  enemy->hitbox.y = enemy->position.y - enemy->size.y / 2;
}

void DrawEnemy(Enemy enemy) {
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

  Vector2 drawPosition = {enemy.position.x - enemy.size.x / 2,
                          enemy.position.y - enemy.size.y / 2};
  DrawRectangleV(drawPosition, enemy.size, RED);
  DrawRectangleV((Vector2){.x = enemy.hitbox.x, .y = enemy.hitbox.y},
                 enemy.size, BLUE);
  // }
}

bool CheckEnemyCollision(Enemy enemy, Rectangle other) {
  if (!enemy.active)
    return false;

  return CheckCollisionRecs(enemy.hitbox, other);
}

void DestroyEnemy(Enemy *enemy) {
  enemy->active = false;
  enemy->health = 0;
}
