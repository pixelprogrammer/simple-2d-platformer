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

// Returns a Rectangle representing the visible world area of a Camera2D
Rectangle GetCameraWorldRectangle(Camera2D camera) {
  float screenWidth = (float)GetScreenWidth();
  float screenHeight = (float)GetScreenHeight();

  // Get the top-left and bottom-right corners of the screen in world
  // coordinates
  Vector2 screenTopLeft = GetScreenToWorld2D((Vector2){0.0f, 0.0f}, camera);
  Vector2 screenBottomRight =
      GetScreenToWorld2D((Vector2){screenWidth, screenHeight}, camera);

  // Calculate the width and height of the visible area
  float width = screenBottomRight.x - screenTopLeft.x;
  float height = screenBottomRight.y - screenTopLeft.y;

  // Return the visible world area as a Rectangle
  return (Rectangle){screenTopLeft.x, screenTopLeft.y, width, height};
}
