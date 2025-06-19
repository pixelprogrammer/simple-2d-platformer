#include <raylib.h>

bool CheckCollisionRecs(Rectangle rec1, Rectangle rec2) {
  return (rec1.x < rec2.x + rec2.width && rec1.x + rec1.width > rec2.x &&
          rec1.y < rec2.y + rec2.height && rec1.y + rec1.height > rec2.y);
}
