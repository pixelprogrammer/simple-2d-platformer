#ifndef MOVEABLE_H
#define MOVEABLE_H

#include <raylib.h>

typedef struct {
    Vector2 position;
    Vector2 prevPosition;
    Vector2 velocity;
    Vector2 speed;
} MoveComponent;

void SystemMoveEntity(MoveComponent *moveable, float deltaTime);

#endif  // !MOVEABLE_H
