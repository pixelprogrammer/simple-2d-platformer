#include "sound.h"
#include <stdio.h>

typedef struct {
  Sound sound;
  bool loaded;
  float volume;
} SoundEffect;

static SoundEffect soundMap[SOUND_COUNT] = {0};
static bool soundSystemInitialized = false;

void InitSoundSystem(void) {
  if (!soundSystemInitialized) {
    InitAudioDevice();

    for (int i = 0; i < SOUND_COUNT; i++) {
      soundMap[i].loaded = false;
      soundMap[i].volume = 1.0f;
    }

    soundSystemInitialized = true;
  }
}

void DestroySoundSystem(void) {
  if (soundSystemInitialized) {
    for (int i = 0; i < SOUND_COUNT; i++) {
      if (soundMap[i].loaded) {
        UnloadSound(soundMap[i].sound);
        soundMap[i].loaded = false;
      }
    }

    CloseAudioDevice();
    soundSystemInitialized = false;
  }
}

void LoadSoundEffect(SoundID id, const char *filename) {
  if (!soundSystemInitialized) {
    printf("Warning: Sound system not initialized\n");
    return;
  }

  if (id >= SOUND_COUNT) {
    printf("Error: Invalid sound ID\n");
    return;
  }

  if (soundMap[id].loaded) {
    UnloadSound(soundMap[id].sound);
  }

  soundMap[id].sound = LoadSound(filename);
  soundMap[id].loaded = true;
}

void UnloadSoundEffect(SoundID id) {
  if (id >= SOUND_COUNT || !soundMap[id].loaded) {
    return;
  }

  UnloadSound(soundMap[id].sound);
  soundMap[id].loaded = false;
}

void PlaySoundEffect(SoundID id) {
  if (id >= SOUND_COUNT || !soundMap[id].loaded) {
    return;
  }

  SetSoundVolume(soundMap[id].sound, soundMap[id].volume);
  PlaySound(soundMap[id].sound);
}

void SetSoundEffectVolume(SoundID id, float volume) {
  if (id >= SOUND_COUNT) {
    return;
  }

  soundMap[id].volume = volume;
  if (soundMap[id].loaded) {
    SetSoundVolume(soundMap[id].sound, volume);
  }
}

float GetSoundEffectVolume(SoundID id) {
  if (id >= SOUND_COUNT) {
    return 0.0f;
  }

  return soundMap[id].volume;
}

bool IsSoundEffectPlaying(SoundID id) {
  if (id >= SOUND_COUNT || !soundMap[id].loaded) {
    return false;
  }

  return IsSoundPlaying(soundMap[id].sound);
}

void StopSoundEffect(SoundID id) {
  if (id >= SOUND_COUNT || !soundMap[id].loaded) {
    return;
  }

  StopSound(soundMap[id].sound);
}
