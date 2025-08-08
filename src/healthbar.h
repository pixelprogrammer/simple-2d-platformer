#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include "raylib.h"

#define HEALTHBAR_DEFAULT YELLOW
#define HEALTHBAR_DEFAULT_LIGHT CLITERAL(Color){253, 249, 0, 255}

#define HEALTHBAR_PINK CLITERAL(Color){0, 255, 255, 255}
#define HEALTHBAR_PINK_LIGHT CLITERAL(Color){225, 255, 255, 255}

#define HEALTHBAR_PURPLE CLITERAL(Color){155, 53, 215, 255}
#define HEALTHBAR_PURPLE_LIGHT CLITERAL(Color){200, 110, 255, 255}

#define HEALTHBAR_FIRE CLITERAL(Color){251, 113, 30, 255}
#define HEALTHBAR_FIRE_LIGHT CLITERAL(Color){255, 155, 55, 255}

#define HEALTHBAR_RUSH CLITERAL(Color){235, 20, 20, 255}
#define HEALTHBAR_RUSH_LIGHT CLITERAL(Color){255, 80, 80, 255}

typedef struct {
    Vector2 position;
    int     maxHealth;
    int     currentHealth;
    int     segmentWidth;
    int     segmentHeight;
    int     segmentSpacing;
    int     borderWidth;
    Color   healthColor;
    Color   healthColorLight;
    Color   emptyColor;
    Color   borderColor;
} HealthBar;

HealthBar CreateHealthBar(Vector2 position, int maxHealth, int segmentWidth, int segmentHeight);
void      SetHealthBar(HealthBar* healthBar, int currentHealth);
void      UpdateHealthbar(HealthBar* healthBar, int health);
void      DrawHealthBar(HealthBar healthBar);
void      DrawHealthBarSegment(int value, int totalHeight, HealthBar healthBar);

#endif
