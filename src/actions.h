#ifndef ACTIONS_H
#define ACTIONS_H

#include <raylib.h>

typedef struct {
    bool    jumpKeyDown;
    bool    jumpKeyPressed;
    bool    shootButtonDown;
    bool    shootButtonReleased;
    bool    changeWeaponNextPressed;
    bool    changeWeaponPrevPressed;
    Vector2 moveAxis;
    Vector2 axisDirection;
} ActionsComponent;
#endif  // ACTIONS_H
