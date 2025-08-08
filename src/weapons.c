#include "weapons.h"
#include "healthbar.h"
#include "player.h"
#include <raylib.h>

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
    case WEAPON_DEFAULT:
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
