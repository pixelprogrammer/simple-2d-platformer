#include "weapons.h"
#include "healthbar.h"
#include "player.h"
#include <raylib.h>
#include <raymath.h>

Weapon *CreateWeaponsArray(void) {
  static Weapon weapons[WEAPON_TOTAL];

  Color primaryColor = HEALTHBAR_DEFAULT;
  Color secondaryColor = HEALTHBAR_DEFAULT_LIGHT;

  for (int i = 0; i < WEAPON_TOTAL; i++) {
    weapons[i].active = false;
    weapons[i].healthBar = CreateHealthBar(
        WEAPON_HEALTHBAR_POSITION, PLAYER_HEALTHBAR_MAX_HEALTH,
        PLAYER_HEALTHBAR_SEGMENT_WIDTH, PLAYER_HEALTHBAR_SEGMENT_HEIGHT);

    switch (i) {
    case WEAPON_LAZER:
      primaryColor = HEALTHBAR_PINK;
      secondaryColor = HEALTHBAR_PINK_LIGHT;
      break;
    case WEAPON_SPARK:
      primaryColor = HEALTHBAR_PURPLE;
      secondaryColor = HEALTHBAR_PURPLE_LIGHT;
      break;
    case WEAPON_FIRE:
      primaryColor = HEALTHBAR_FIRE;
      secondaryColor = HEALTHBAR_FIRE_LIGHT;
      break;
    case WEAPON_RUSH_JET:
    case WEAPON_RUSH_JUMP:
      primaryColor = HEALTHBAR_RUSH;
      secondaryColor = HEALTHBAR_RUSH_LIGHT;
      break;
    case WEAPON_BUSTER:
    default:
      primaryColor = HEALTHBAR_DEFAULT;
      secondaryColor = HEALTHBAR_DEFAULT_LIGHT;
      break;
    }

    weapons[i].healthBar.healthColor = primaryColor;
    weapons[i].healthBar.healthColorLight = secondaryColor;
  }

  return weapons;
}

void RefillWeapons(Weapon *weapons[WEAPON_TOTAL]) {
  for (int i = 1; i < WEAPON_TOTAL; i++) {
    SetHealthBar(&weapons[i]->healthBar, weapons[i]->healthBar.maxHealth);
  }
}

void UpdateProjectiles(ProjectileArray *a, float deltaTime) {
  if (a->length == 0) {
    return;
  }

  for (int i = a->length - 1; i >= 0; i--) {
    UpdateProjectile(&a->projectiles[i], deltaTime);
  }
}

void UpdateProjectile(Projectile *projectile, float deltaTime) {
  switch (projectile->type) {
  case WEAPON_BUSTER:
  default:
    projectile->position.x +=
        BUSTER_SHOT_SPEED * deltaTime * projectile->direction;
    projectile->sprite.dest.x = projectile->position.x;
    break;
  }
}

void CheckProjectileCollisions(ProjectileArray *a, Rectangle bounds) {
  if (a->length == 0) {
    return;
  }

  int projectileWidth = BUSTER_SHOT_WIDTH;
  int projectileHeight = BUSTER_SHOT_HEIGHT;

  // for now lets just delete the projectile when they are off screen
  for (int i = a->length; i >= 0; i--) {

    if (a->projectiles[i].position.x + projectileWidth < bounds.x ||
        a->projectiles[i].position.x > bounds.x + bounds.width) {
      DestroyProjectile(a, i);
    }
  }
}

void DrawProjectile(Projectile *projectile) { DrawSprite(projectile->sprite); }

Projectile CreateBusterProjectile(Texture2D texture, Vector2 position,
                                  int direction) {

  return (Projectile){
      .sprite =
          {
              .texture = texture,
              .source =
                  (Rectangle){
                      .x = 0.0f,
                      .y = 0.0f,
                      .width = BUSTER_SHOT_WIDTH,
                      .height = BUSTER_SHOT_HEIGHT,
                  },
              .dest =
                  (Rectangle){
                      .x = position.x,
                      .y = position.y,
                      .width = BUSTER_SHOT_WIDTH,
                      .height = BUSTER_SHOT_HEIGHT,
                  },
              .origin = (Vector2){.x = BUSTER_SHOT_WIDTH / 2.0f,
                                  .y = BUSTER_SHOT_HEIGHT / 2.0f},
          },
      .position = position,
      .type = WEAPON_BUSTER,
      .direction = direction,
      .damage = 2,
  };
}

void SpawnProjectile(ProjectileArray *a, Texture2D texture, Vector2 position,
                     WeaponType type, int direction) {

  if (a->length >= MAX_PROJECTILES) {
    return;
  }

  Projectile newProjectile = (Projectile){};
  switch (type) {
  case WEAPON_BUSTER:
  default:
    newProjectile = CreateBusterProjectile(texture, position, direction);
  }

  a->projectiles[a->length] = newProjectile;

  a->length++;
}

void DestroyProjectile(ProjectileArray *a, int projectileIndex) {
  if (projectileIndex < 0 || projectileIndex >= a->length) {
    return;
  }
  // shift projectiles
  for (int i = projectileIndex; i < a->length - 1; i++) {
    a->projectiles[i] = a->projectiles[i + 1];
  }

  a->length--;
}
