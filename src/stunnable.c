#include "stunnable.h"

void UpdateStunSystem(StunComponent *stun, float deltaTime) {
  stun->invincibilityTime -= deltaTime;
  stun->stunTime -= deltaTime;

  if (stun->invincibilityTime < 0) {
    stun->invincibilityTime = 0;
  }

  if (stun->stunTime < 0) {
    stun->stunTime = 0;
  }
}
