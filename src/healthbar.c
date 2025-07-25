#include "healthbar.h"
#include <raylib.h>

HealthBar CreateHealthBar(Vector2 position, int maxHealth, int segmentWidth,
                          int segmentHeight) {
  HealthBar healthBar = {0};
  healthBar.position = position;
  healthBar.maxHealth = maxHealth;
  healthBar.currentHealth = maxHealth;
  healthBar.segmentWidth = segmentWidth;
  healthBar.segmentHeight = segmentHeight;
  healthBar.segmentSpacing = 1.0f;

  healthBar.borderWidth = 1;
  healthBar.healthColorLight = (Color){252, 252, 252, 255};
  healthBar.healthColor = YELLOW;
  healthBar.emptyColor = DARKGRAY;
  healthBar.borderColor = BLACK;
  return healthBar;
}

void UpdateHealthBar(HealthBar *healthBar, int currentHealth) {
  if (currentHealth < 0)
    currentHealth = 0;
  if (currentHealth > healthBar->maxHealth)
    currentHealth = healthBar->maxHealth;
  healthBar->currentHealth = currentHealth;
}

void DrawHealthBar(HealthBar healthBar) {
  float totalWidth = healthBar.segmentWidth + (healthBar.borderWidth * 2);
  float totalHeight = healthBar.segmentHeight * healthBar.maxHealth +
                      healthBar.segmentSpacing * healthBar.maxHealth;

  DrawRectangle(healthBar.position.x, healthBar.position.y, totalWidth,
                totalHeight, healthBar.borderColor);

  for (int i = 0; i < healthBar.currentHealth; i++) {
    DrawHealthBarSegment(i, totalWidth, totalHeight, healthBar);
  }
}

void DrawHealthBarSegment(int value, int totalWidth, int totalHeight,
                          HealthBar healthBar) {
  int x = healthBar.position.x + 1;
  int y = healthBar.position.y + totalHeight -
          (healthBar.segmentSpacing * (value + 1)) -
          (healthBar.segmentHeight * (value + 1));
  int colorSegmentWidth = healthBar.segmentWidth / 3;
  int lightSegmentWidth = healthBar.segmentWidth % 3;
  if (lightSegmentWidth == 0) {
    lightSegmentWidth = colorSegmentWidth;
  }
  DrawRectangle(x, y, colorSegmentWidth, healthBar.segmentHeight,
                healthBar.healthColor);
  DrawRectangle(x + colorSegmentWidth, y, colorSegmentWidth,
                healthBar.segmentHeight, healthBar.healthColorLight);
  DrawRectangle(x + colorSegmentWidth + lightSegmentWidth, y, colorSegmentWidth,
                healthBar.segmentHeight, healthBar.healthColor);
}
