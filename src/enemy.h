#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

typedef struct {
    Vector2   position;
    Vector2   velocity;
    Vector2   size;
    float     rotation;
    float     rotationSpeed;
    Texture2D sprite;
    Vector2   spriteOrigin;
    Rectangle hitbox;
    bool      active;
    float     speed;
    int       health;
} Enemy;

void InitEnemy(Enemy* enemy, Vector2 position, Texture2D sprite);
void UpdateEnemy(Enemy* enemy, Vector2 playerPosition, float deltaTime);
void DrawEnemy(Enemy enemy);
bool CheckEnemyCollision(Enemy enemy, Rectangle other);
void DestroyEnemy(Enemy* enemy);

#endif
