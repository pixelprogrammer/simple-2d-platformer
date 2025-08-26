#ifndef WEAPONS_H
#define WEAPONS_H

#include <raylib.h>
#include "healthbar.h"
#include "sprite.h"

#define WEAPON_HEALTHBAR_POSITION (Vector2){64, 100}
#define MAX_PROJECTILES 5

#define BUSTER_SHOT_WIDTH 8
#define BUSTER_SHOT_HEIGHT 6
#define BUSTER_SHOT_SPEED 400.0f

typedef enum {
    WEAPON_BUSTER,
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

typedef struct {
    bool      active;
    HealthBar healthBar;
    Sound    *sound;
} Weapon;

typedef struct {
    int        damage;
    Vector2    position;
    int        direction;
    WeaponType type;
    Rectangle  hitBox;
    Sprite     sprite;
} Projectile;

typedef struct {
    int        length;
    Projectile projectiles[MAX_PROJECTILES];
} ProjectileArray;

Weapon *CreateWeaponsArray(void);
void    RefillWeapons(Weapon *weapons[WEAPON_TOTAL]);
void    DrawWeaponsBar(Weapon weapon);
void    UpdateProjectile(Projectile *projectile, float deltaTime);
void    UpdateProjectiles(ProjectileArray *projectileArray, float deltaTime);
void    DrawProjectile(Projectile *projectile);
void    SpawnProjectile(
       ProjectileArray *array, Texture2D texture, Vector2 position, WeaponType type, int direction);
void DestroyProjectile(ProjectileArray *a, int projectileIndex);
void CheckProjectileCollisions(ProjectileArray *a, Rectangle bounds);

// create the different types of projectiles
Projectile CreateBusterProjectile(Texture2D texture, Vector2 position, int direction);

#endif
