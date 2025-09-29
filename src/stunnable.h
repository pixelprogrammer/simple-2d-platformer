#ifndef STUNNABLE_H
#define STUNNABLE_H

typedef struct {
    float invincibilityTime;
    float stunTime;
} StunComponent;

void UpdateStunSystem(StunComponent *stun, float deltaTime);

#endif  // !STUNNABLE_H
