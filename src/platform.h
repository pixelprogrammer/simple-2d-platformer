#ifndef PLATFORM_H
#define PLATFORM_H

#include <raylib.h>

typedef enum {
    PLATFORM_STATIC,
    PLATFORM_MOVING_HORIZONTAL,
    PLATFORM_MOVING_VERTICAL,
    PLATFORM_MOVING_CIRCULAR
} PlatformType;

typedef struct {
    Rectangle    rect;
    Color        color;
    PlatformType type;
    Vector2      velocity;
    Vector2      startPosition;
    Vector2      endPosition;
    float        speed;
    bool         movingToEnd;
} Platform;

void UpdatePlatform(Platform *platform, float deltaTime);
void UpdatePlatforms(Platform platforms[], int platformCount, float deltaTime);

#endif
