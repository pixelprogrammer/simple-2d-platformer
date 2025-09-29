#include "fallable.h"
#include "moveable.h"

void FallSystem(FallComponent *fallComponent, MoveComponent *moveComponent) {
  moveComponent->velocity.y += fallComponent->gravity;
  if (moveComponent->velocity.y > fallComponent->maxFallSpeed) {
    moveComponent->velocity.y = fallComponent->maxFallSpeed;
  }
}
