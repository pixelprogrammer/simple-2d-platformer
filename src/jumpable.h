#ifndef JUMPABLE_H
#define JUMPABLE_H
#include <stdbool.h>
#include "actions.h"
#include "moveable.h"

typedef struct {
    bool  onGround;
    bool  isJumping;
    float jumpReleaseFactor;
    float jumpSpeed;
} JumpComponent;

#endif  // !JUMPABLE_H

void JumpSystem(ActionsComponent *actions,
                JumpComponent    *jumpComponent,
                MoveComponent    *moveComponent);
