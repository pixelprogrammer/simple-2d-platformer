#ifndef SCREEN_H
#define SCREEN_H

#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

Rectangle GetBoundsRect(Camera2D camera);
Rectangle GetCameraWorldRectangle(Camera2D camera);

#endif  // !SCREEN_H
