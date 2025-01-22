#include "game.h"

Game new_game() {
  Game game = {
      .state = GameStateFirstPage,
  };
  return game;
}

void game_set_state(Game* game, GameState state) {
  game->state = state;
}