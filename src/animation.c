#include "animation.h"
#include <raylib.h>
#include <stdio.h>

void InitAnimationTimeline(AnimationTimeline *timeline, bool loop) {
  timeline->frameCount = 0;
  timeline->loop = loop;
  timeline->finished = false;
  timeline->currentTime = 0.0f;
  timeline->currentFrameIndex = 0;
}

void AddAnimationFrame(AnimationTimeline *timeline, int frame, float duration) {
  if (timeline->frameCount < MAX_ANIMATION_FRAMES) {
    timeline->frames[timeline->frameCount].id = frame;
    timeline->frames[timeline->frameCount].duration = duration;
    timeline->frameCount++;
  }
}

void PlayAnimationTimeline(AnimationTimeline *timeline, float deltaTime) {
  if (timeline->finished && !timeline->loop) {
    return;
  }

  if (timeline->frameCount == 0) {
    return;
  }

  timeline->currentTime += deltaTime;

  AnimationFrame currentFrame = timeline->frames[timeline->currentFrameIndex];
  printf("Playing animation, current frame: [%d], currentFrameTime: [%f]\n",
         currentFrame.id, timeline->currentTime);

  if (timeline->currentTime >= currentFrame.duration) {
    timeline->currentTime -= currentFrame.duration;
    timeline->currentFrameIndex++;

    if (timeline->currentFrameIndex >= timeline->frameCount) {
      if (timeline->loop) {
        timeline->currentFrameIndex = 0;
      } else {
        timeline->currentFrameIndex = timeline->frameCount - 1;
        timeline->finished = true;
      }
    }
  }
}

void ResetAnimationTimeline(AnimationTimeline *timeline) {
  timeline->currentTime = 0.0f;
  timeline->currentFrameIndex = 0;
  timeline->finished = false;
}

int GetCurrentAnimationFrame(AnimationTimeline *timeline) {
  if (timeline->frameCount == 0) {
    return 0;
  }
  return timeline->frames[timeline->currentFrameIndex].id;
}

bool IsAnimationFinished(AnimationTimeline *timeline) {
  return timeline->finished;
}

// void InitAnimatedSprite(AnimatedSprite *sprite, Texture2D texture,
//                         int frameWidth, int frameHeight, int frameX,
//                         int frameY) {
//   sprite->sprite = texture;
//   sprite->frameWidth = frameWidth;
//   sprite->frameHeight = frameHeight;
//   sprite->framePos.x = frameX;
//   sprite->framePos.y = frameY;
//   sprite->facingRight = true;
//   InitAnimationTimeline(&sprite->timeline, true);
// }

void DrawAnimatedSprite(Texture2D *sprite, AnimationTimeline *timeline,
                        Vector2 position, Vector2 size, bool facingRight) {
  if (sprite->id == 0) {
    return;
  }

  int currentFrame = GetCurrentAnimationFrame(timeline);

  Rectangle sourceRec = {
      (currentFrame * timeline->frame.width) +
          (currentFrame * timeline->frameGap) + timeline->frame.x,
      timeline->frame.y,
      facingRight ? timeline->frame.width : -timeline->frame.width,
      timeline->frame.height};

  Rectangle destRec = {position.x, position.y, size.x, size.y};

  Vector2 origin = {0, 0};
  DrawTexturePro(*sprite, sourceRec, destRec, origin, 0.0f, WHITE);
}
