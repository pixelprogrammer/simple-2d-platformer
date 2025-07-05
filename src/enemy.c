#include "enemy.h"
#include <stdlib.h>

void InitEnemy(Enemy* enemy, Vector2 position, Texture2D sprite) {
    enemy->position = position;
    enemy->velocity = (Vector2){0.0f, 0.0f};
    enemy->size = (Vector2){32.0f, 32.0f};
    enemy->rotation = 0.0f;
    enemy->rotationSpeed = 0.0f;
    enemy->sprite = sprite;
    enemy->spriteOrigin = (Vector2){enemy->size.x / 2, enemy->size.y / 2};
    enemy->hitbox = (Rectangle){position.x - enemy->size.x / 2, position.y - enemy->size.y / 2, enemy->size.x, enemy->size.y};
    enemy->active = true;
    enemy->speed = 50.0f;
    enemy->health = 1;
}

void UpdateEnemy(Enemy* enemy) {
    if (!enemy->active) return;
    
    enemy->rotation += enemy->rotationSpeed * GetFrameTime();
    
    enemy->position.x += enemy->velocity.x * GetFrameTime();
    enemy->position.y += enemy->velocity.y * GetFrameTime();
    
    enemy->hitbox.x = enemy->position.x - enemy->size.x / 2;
    enemy->hitbox.y = enemy->position.y - enemy->size.y / 2;
}

void DrawEnemy(Enemy enemy) {
    if (!enemy.active) return;
    
    if (enemy.sprite.id > 0) {
        DrawTexturePro(enemy.sprite, 
                      (Rectangle){0, 0, enemy.sprite.width, enemy.sprite.height},
                      (Rectangle){enemy.position.x, enemy.position.y, enemy.size.x, enemy.size.y},
                      enemy.spriteOrigin,
                      enemy.rotation,
                      WHITE);
    } else {
        Vector2 drawPosition = {enemy.position.x - enemy.size.x / 2, enemy.position.y - enemy.size.y / 2};
        DrawRectangleV(drawPosition, enemy.size, RED);
    }
}

bool CheckEnemyCollision(Enemy enemy, Rectangle other) {
    if (!enemy.active) return false;
    
    return CheckCollisionRecs(enemy.hitbox, other);
}

void DestroyEnemy(Enemy* enemy) {
    enemy->active = false;
    enemy->health = 0;
}