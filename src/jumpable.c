#include "jumpable.h"
#include "actions.h"
#include "moveable.h"
#include "player.h"

void JumpSystem(ActionsComponent *actions, JumpComponent *jumpComponent,
                MoveComponent *moveComponent) {
  if (actions->jumpKeyPressed && jumpComponent->onGround) {
    moveComponent->velocity.y = jumpComponent->jumpSpeed;
    jumpComponent->isJumping = true;
  }

  // Continue the height of the jump slightly after release
  if (jumpComponent->isJumping && !actions->jumpKeyDown) {
    moveComponent->velocity.y *= jumpComponent->jumpReleaseFactor;
    jumpComponent->isJumping = false;
  }
}
