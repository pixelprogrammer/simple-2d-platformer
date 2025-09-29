#ifndef FALLABLE_H
#define FALLABLE_H

#include "moveable.h"
typedef struct {
    float gravity;
    float maxFallSpeed;
} FallComponent;

void FallSystem(FallComponent *fallComponent, MoveComponent *moveComponent);

#endif  // !FALLABLE_H
