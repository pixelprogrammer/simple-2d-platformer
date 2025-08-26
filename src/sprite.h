#ifndef SPRITE_H
#define SPRITE_H

#include <raylib.h>

typedef struct {
    Texture2D texture;
    Rectangle source;
    Rectangle dest;
    Vector2   origin;
} Sprite;

void DrawSprite(Sprite sprite);

#endif
