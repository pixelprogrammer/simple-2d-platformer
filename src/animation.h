#ifndef ANIMATION_H
#define ANIMATION_H

#include <raylib.h>

#define MAX_ANIMATION_FRAMES 32

typedef struct {
    int   id;
    float duration;
} AnimationFrame;

typedef struct {
    AnimationFrame frames[MAX_ANIMATION_FRAMES];
    int            frameCount;
    Rectangle      frame;
    int            frameGap;
    bool           loop;
    bool           finished;
    float          currentTime;
    int            currentFrameIndex;
} AnimationTimeline;

void InitAnimationTimeline(AnimationTimeline *timeline, bool loop);
void AddAnimationFrame(AnimationTimeline *timeline, int frame, float duration);
void PlayAnimationTimeline(AnimationTimeline *timeline, float deltaTime);
void ResetAnimationTimeline(AnimationTimeline *timeline);
int  GetCurrentAnimationFrame(AnimationTimeline *timeline);
bool IsAnimationFinished(AnimationTimeline *timeline);

// void InitAnimatedSprite(AnimatedSprite *sprite,
//                         Texture2D       texture,
//                         int             frameWidth,
//                         int             frameHeight,
//                         int             frameX,
//                         int             frameY);
// void UpdateAnimatedSprite(AnimatedSprite *sprite, float deltaTime);
void DrawAnimatedSprite(Texture2D         *sprite,
                        AnimationTimeline *timeline,
                        Vector2            position,
                        Vector2            size,
                        bool               facingRight);

#endif
