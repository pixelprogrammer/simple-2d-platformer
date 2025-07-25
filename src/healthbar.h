#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include "raylib.h"

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
void      UpdateHealthBar(HealthBar* healthBar, int currentHealth);
void      DrawHealthBar(HealthBar healthBar);
void      DrawHealthBarSegment(int value, int totalWidth, int totalHeight, HealthBar healthBar);

#endif
