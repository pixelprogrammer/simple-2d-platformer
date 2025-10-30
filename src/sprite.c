#include "sprite.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 10

TextureLoader *CreateTextureLoader(void) {
  TextureLoader *loader = malloc(sizeof(TextureLoader));
  if (!loader)
    return NULL;

  loader->textures = malloc(sizeof(Texture2D) * INITIAL_CAPACITY);
  loader->paths = malloc(sizeof(char *) * INITIAL_CAPACITY);
  loader->count = 0;
  loader->capacity = INITIAL_CAPACITY;

  if (!loader->textures || !loader->paths) {
    free(loader->textures);
    free(loader->paths);
    free(loader);
    return NULL;
  }

  return loader;
}

void DestroyTextureLoader(TextureLoader *loader) {
  if (!loader)
    return;

  UnloadAllTextures(loader);

  for (int i = 0; i < loader->count; i++) {
    free(loader->paths[i]);
  }

  free(loader->textures);
  free(loader->paths);
  free(loader);
}

static bool ResizeLoader(TextureLoader *loader) {
  int newCapacity = loader->capacity * 2;

  Texture2D *newTextures =
      realloc(loader->textures, sizeof(Texture2D) * newCapacity);
  char **newPaths = realloc(loader->paths, sizeof(char *) * newCapacity);

  if (!newTextures || !newPaths) {
    return false;
  }

  loader->textures = newTextures;
  loader->paths = newPaths;
  loader->capacity = newCapacity;

  return true;
}

int LoadTextureToLoader(TextureLoader *loader, const char *path) {
  if (!loader || !path)
    return -1;

  for (int i = 0; i < loader->count; i++) {
    if (strcmp(loader->paths[i], path) == 0) {
      return i;
    }
  }

  if (loader->count >= loader->capacity) {
    if (!ResizeLoader(loader)) {
      return -1;
    }
  }

  Texture2D texture = LoadTexture(path);
  if (texture.id == 0) {
    return -1;
  }

  loader->paths[loader->count] = malloc(strlen(path) + 1);
  if (!loader->paths[loader->count]) {
    UnloadTexture(texture);
    return -1;
  }

  strcpy(loader->paths[loader->count], path);
  loader->textures[loader->count] = texture;

  return loader->count++;
}

Texture2D GetTextureFromLoader(TextureLoader *loader, int index) {
  if (!loader || index < 0 || index >= loader->count) {
    Texture2D empty = {0};
    return empty;
  }

  return loader->textures[index];
}

void UnloadAllTextures(TextureLoader *loader) {
  if (!loader)
    return;

  for (int i = 0; i < loader->count; i++) {
    UnloadTexture(loader->textures[i]);
  }

  loader->count = 0;
}

void DrawSprite(Sprite sprite, Texture2D *texture) {
  if (!texture->id) {
    return;
  }

  DrawTexturePro(*texture, sprite.source, sprite.dest, sprite.origin, 0.0f,
                 WHITE);
}
