#pragma once

typedef enum _GameState {
  GameStateFirstPage,
  GameStateMainMenu,
  GameStatePlaying,
} GameState;

typedef struct _Game {
  GameState state;
} Game;

Game new_game();
void game_set_state(Game* game, GameState state);