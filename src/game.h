#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
typedef enum {
    GAME_MENU,
    GAME_LEVEL_SELECT,
    GAME_LEVEL_PLAYING,
    GAME_LEVEL_PAUSED,
    GAME_GAME_OVER,
    GAME_VICTORY
} GameState;

GameState GetCurrentGameState(void);
GameState GetPreviousGameState(void);

void GameInit();
void GameExit();
bool GameShouldExit();
void GameCleanup();

void SetGameState(GameState newState);
void UpdateGameState(void);
void DrawGameState(void);

void UpdateMenuState(void);
void UpdateLevelSelectState(void);
void UpdatePlayingState(void);
void UpdatePausedState(void);
void UpdateGameOverState(void);
void UpdateVictoryState(void);

void DrawMenuState(void);
void DrawLevelSelectState(void);
void DrawPlayingState(void);
void DrawPausedState(void);
void DrawGameOverState(void);
void DrawVictoryState(void);

#endif
