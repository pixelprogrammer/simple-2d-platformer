#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"

typedef enum {
    SOUND_JUMP,
    SOUND_LAND,
    SOUND_SHOOT,
    SOUND_HIT,
    SOUND_PICKUP,
    SOUND_ENEMY_DEATH,
    SOUND_PLAYER_DEATH,
    SOUND_MENU_SELECT,
    SOUND_COUNT
} SoundID;

void  InitSoundSystem(void);
void  DestroySoundSystem(void);
void  LoadSoundEffect(SoundID id, const char* filename);
void  UnloadSoundEffect(SoundID id);
void  PlaySoundEffect(SoundID id);
void  SetSoundEffectVolume(SoundID id, float volume);
float GetSoundEffectVolume(SoundID id);
bool  IsSoundEffectPlaying(SoundID id);
void  StopSoundEffect(SoundID id);

#endif
