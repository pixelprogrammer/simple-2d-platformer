#ifndef SPRITE_H
#define SPRITE_H

#include <raylib.h>

typedef struct {
    // for TextureLoader
    int       textureId;
    Rectangle source;
    Rectangle dest;
    Vector2   origin;
} Sprite;

typedef struct {
    Texture2D* textures;
    char**     paths;
    int        count;
    int        capacity;
} TextureLoader;

TextureLoader* CreateTextureLoader(void);
void           DestroyTextureLoader(TextureLoader* loader);
int            LoadTextureToLoader(TextureLoader* loader, const char* path);
Texture2D      GetTextureFromLoader(TextureLoader* loader, int index);
void           UnloadAllTextures(TextureLoader* loader);
void           DrawSprite(Sprite sprite, Texture2D* texture);

#endif
