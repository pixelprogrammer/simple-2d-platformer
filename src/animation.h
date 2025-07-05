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
    Vector2        position;
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
void DrawAnimatedSprite(Texture2D         *sprite,
                        AnimationTimeline *timeline,
                        Vector2            position,
                        Vector2            size,
                        Vector2            origin,
                        bool               facingRight);

#endif
