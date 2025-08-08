#ifndef WEAPONS_H
#define WEAPONS_H

#include "healthbar.h"

#define WEAPON_HEALTHBAR_POSITION (Vector2){64, 100}

typedef struct {
    bool      active;
    HealthBar healthBar;
} Weapon;

typedef enum {
    WEAPON_DEFAULT,
    WEAPON_ICE,
    WEAPON_FIRE,
    WEAPON_WIND,
    WEAPON_WATER,
    WEAPON_SPARK,
    WEAPON_MISSILE,
    WEAPON_LAZER,
    WEAPON_BOMB,
    WEAPON_RUSH_JUMP,
    WEAPON_RUSH_JET,
    WEAPON_TOTAL,
} WeaponType;

Weapon *CreateWeaponsArray(void);
void    RefillWeapons(Weapon *weapons[WEAPON_TOTAL]);
void    DrawWeaponsBar(Weapon weapon);

#endif
