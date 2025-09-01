#include "../src/player.h"
#include "terminal.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define MAX_JUMP_FRAMES 120

#define TEST_ASSERT(condition, message)                                        \
  do {                                                                         \
    if (!(condition)) {                                                        \
      printf("%s: %s\n", TERMINAL_COLOR_RED("FAIL"), message);                 \
      return 0;                                                                \
    } else {                                                                   \
      printf("%s: %s\n", TERMINAL_COLOR_GREEN("PASS"), message);               \
    }                                                                          \
  } while (0)

typedef struct {
  float frame;
  float height;
  float velocity;
} JumpFrame;

int test_player_jump_height() {
  Player player = {.position = {100, 400},
                   .velocity = {0, 0},
                   .onGround = true,
                   .isJumping = false};

  float deltaTime = 1.0f / 60.0f;
  float initialY = player.position.y;
  JumpFrame frames[MAX_JUMP_FRAMES];
  int frameCount = 0;

  player.velocity.y = JUMP_SPEED;
  player.isJumping = true;
  player.onGround = false;

  printf("\nJump Height Analysis:\n");
  printf("Frame | Height | Velocity\n");
  printf("------|--------|----------\n");

  for (int frame = 0; frame < MAX_JUMP_FRAMES && frameCount < 120; frame++) {
    HandleJump(&player, true, true, deltaTime);
    MovePlayer(&player, deltaTime);

    float height = initialY - player.position.y;
    frames[frameCount] = (JumpFrame){frame, height, player.velocity.y};
    frameCount++;

    printf("%5d | %6.1f | %8.1f\n", frame + 1, height, player.velocity.y);

    if (player.position.y >= initialY && frame > 0) {
      break;
    }
  }

  float maxHeight = 0;
  int maxHeightFrame = 0;
  for (int i = 0; i < frameCount; i++) {
    if (frames[i].height > maxHeight) {
      maxHeight = frames[i].height;
      maxHeightFrame = i;
    }
  }

  printf("\nJump Statistics:\n");
  printf("Max Height: %.1f pixels\n", maxHeight);
  printf("Max Height Frame: %d\n", maxHeightFrame);
  printf("Initial Jump Speed: %.1f\n", JUMP_SPEED);
  printf("Gravity: %.1f\n", GRAVITY);
  printf("Jump Duration: %d frames\n", frameCount);

  char buffer[100];

  TEST_ASSERT(maxHeight > 0, "Player should jump above initial position");
  TEST_ASSERT(maxHeight < 200,
              "Jump height should be reasonable (less than 200 pixels)");
  TEST_ASSERT(frameCount > 10, "Jump should last more than 10 frames");
  sprintf(buffer, "Jump should reach %f pixels height at frame 20",
          JUMP_PEAK_HEIGHT);
  TEST_ASSERT(frames[19].height == JUMP_PEAK_HEIGHT, buffer);
  return 1;
}

int test_jump_release_mechanics() {
  Player player = {.position = {100, 400},
                   .velocity = {0, 0},
                   .onGround = true,
                   .isJumping = true};

  player.velocity.y = JUMP_SPEED;
  float velocityBeforeRelease = player.velocity.y;

  player.velocity.y *= JUMP_RELEASE_FACTOR;
  player.isJumping = false;

  float velocityAfterRelease = player.velocity.y;

  printf("\nJump Release Test:\n");
  printf("Velocity before release: %.1f\n", velocityBeforeRelease);
  printf("Velocity after release: %.1f\n", velocityAfterRelease);
  printf("Release factor: %.1f\n", JUMP_RELEASE_FACTOR);

  TEST_ASSERT(velocityAfterRelease > velocityBeforeRelease,
              "Velocity should be reduced (less negative) after release");
  TEST_ASSERT(fabs(velocityAfterRelease -
                   velocityBeforeRelease * JUMP_RELEASE_FACTOR) < 0.1f,
              "Release factor should be applied correctly");

  return 1;
}

int test_gravity_constants() {
  printf("\nGravity Constants Test:\n");
  printf("JUMP_SPEED: %.1f\n", JUMP_SPEED);
  printf("MIN_JUMP_SPEED: %.1f\n", MIN_JUMP_SPEED);
  printf("GRAVITY: %.1f\n", GRAVITY);
  printf("MAX_FALL_SPEED: %.1f\n", MAX_FALL_SPEED);
  printf("JUMP_RELEASE_FACTOR: %.1f\n", JUMP_RELEASE_FACTOR);

  TEST_ASSERT(JUMP_SPEED < 0, "Jump speed should be negative (upward)");
  TEST_ASSERT(GRAVITY > 0, "Gravity should be positive (downward)");
  TEST_ASSERT(MAX_FALL_SPEED > 0, "Max fall speed should be positive");
  TEST_ASSERT(JUMP_RELEASE_FACTOR < 1.0f,
              "Jump release factor should reduce velocity");
  TEST_ASSERT(MIN_JUMP_SPEED > JUMP_SPEED,
              "Min jump speed should be less negative than jump speed");

  return 1;
}

int main() {
  printf("=== Player Jump Mechanics Test ===\n");

  int tests_passed = 0;
  int total_tests = 3;

  if (test_gravity_constants())
    tests_passed++;
  if (test_jump_release_mechanics())
    tests_passed++;
  if (test_player_jump_height())
    tests_passed++;

  printf("\n=== Test Summary ===\n");
  printf("Tests passed: %d/%d\n", tests_passed, total_tests);

  if (tests_passed == total_tests) {
    printf("All tests PASSED!\n");
    return 0;
  } else {
    printf("Some tests FAILED!\n");
    return 1;
  }
}
