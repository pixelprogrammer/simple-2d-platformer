#include "platform.h"
#include <raymath.h>

void UpdatePlatform(Platform *platform, float deltaTime) {
  if (platform->type == PLATFORM_STATIC) {
    return;
  }

  switch (platform->type) {
  case PLATFORM_MOVING_HORIZONTAL:
  case PLATFORM_MOVING_VERTICAL: {
    Vector2 targetPosition =
        platform->movingToEnd ? platform->endPosition : platform->startPosition;
    Vector2 currentPosition = {platform->rect.x, platform->rect.y};

    float distanceToTarget = Vector2Distance(currentPosition, targetPosition);

    if (distanceToTarget < platform->speed * deltaTime) {
      platform->rect.x = targetPosition.x;
      platform->rect.y = targetPosition.y;
      platform->movingToEnd = !platform->movingToEnd;
      platform->velocity = (Vector2){0, 0};
    } else {
      Vector2 direction =
          Vector2Normalize(Vector2Subtract(targetPosition, currentPosition));
      platform->velocity.x = direction.x * platform->speed;
      platform->velocity.y = direction.y * platform->speed;

      platform->rect.x += platform->velocity.x * deltaTime;
      platform->rect.y += platform->velocity.y * deltaTime;
    }
    break;
  }
  case PLATFORM_MOVING_CIRCULAR: {
    static float angle = 0.0f;
    angle += platform->speed * deltaTime;

    Vector2 center = {(platform->startPosition.x + platform->endPosition.x) / 2,
                      (platform->startPosition.y + platform->endPosition.y) /
                          2};

    float radiusX =
        fabs(platform->endPosition.x - platform->startPosition.x) / 2;
    float radiusY =
        fabs(platform->endPosition.y - platform->startPosition.y) / 2;

    Vector2 newPosition = {center.x + radiusX * cos(angle),
                           center.y + radiusY * sin(angle)};

    platform->velocity.x = (newPosition.x - platform->rect.x) / deltaTime;
    platform->velocity.y = (newPosition.y - platform->rect.y) / deltaTime;

    platform->rect.x = newPosition.x;
    platform->rect.y = newPosition.y;
    break;
  }
  default:
    break;
  }
}

void UpdatePlatforms(Platform platforms[], int platformCount, float deltaTime) {
  for (int i = 0; i < platformCount; i++) {
    UpdatePlatform(&platforms[i], deltaTime);
  }
}
