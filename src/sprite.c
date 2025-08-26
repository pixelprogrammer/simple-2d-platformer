#include "sprite.h"
#include <raylib.h>

void DrawSprite(Sprite sprite) {
  DrawTexturePro(sprite.texture, sprite.source, sprite.dest, sprite.origin,
                 0.0f, WHITE);
}
