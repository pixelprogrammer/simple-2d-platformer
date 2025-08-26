#include "screen.h"
#include <raylib.h>
extern int screen_width;
extern int screen_height;
extern char *screen_title;

Rectangle GetBoundsRect(Camera2D camera) {
  Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
  return (Rectangle){
      .x = topLeft.x,
      .y = topLeft.y,
      .width = SCREEN_WIDTH,
      .height = SCREEN_HEIGHT,
  };
}
