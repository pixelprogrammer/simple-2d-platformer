#include "moveable.h"

void SystemMoveEntity(MoveComponent *moveable, float deltaTime) {
  moveable->position.x += moveable->velocity.x * deltaTime;
  moveable->position.y += moveable->velocity.y * deltaTime;
}
