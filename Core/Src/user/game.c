#include "game.h"

Game new_game() {
  Game game = {
      .state = GameStateFirstPage,
      .main_menu_selected_entry = GameMainMenuEntryPlay,
  };
  return game;
}

void game_set_state(Game* game, GameState state) {
  if (game->state == state) {
    return;
  }
  game->state = state;
  switch (state) {
  GameStateMainMenu:
    game->main_menu_selected_entry = GameMainMenuEntryPlay;
    break;
  GameStateSettings:
    game->settings_menu_selected_entry = GameSettingsMenuEntryStartHealth;
    break;
  }
}

void game_main_menu_down(Game* game) {
  if (game->main_menu_selected_entry + 1 >= GameMainMenuEntryCount ||
      game->state != GameStateMainMenu) {
    return;
  }
  game->main_menu_selected_entry++;
}

void game_main_menu_up(Game* game) {
  if (game->main_menu_selected_entry <= 0 || game->state != GameStateMainMenu) {
    return;
  }
  game->main_menu_selected_entry--;
}

void game_main_menu_select(Game* game) {
  if (game->state != GameStateMainMenu) {
    return;
  }
  switch (game->main_menu_selected_entry) {
    case GameMainMenuEntryPlay:
      game_set_state(game, GameStatePlaying);
      break;
    case GameMainMenuEntrySettings:
      game_set_state(game, GameStateSettings);
      break;
    case GameMainMenuEntryAbout:
      game_set_state(game, GameStateAbout);
      break;
    default:
      break;
  }
}

void game_settings_menu_up(Game* game) {
  if (game->state != GameStateSettings ||
      game->settings_menu_selected_entry <= 0) {
    return;
  }
  game->settings_menu_selected_entry--;
}

void game_settings_menu_down(Game* game) {
  if (game->state != GameStateSettings ||
      game->settings_menu_selected_entry + 1 >= GameSettingsMenuEntryCount) {
    return;
  }
  game->settings_menu_selected_entry++;
}

void game_settings_menu_select(Game* game) {
  if (game->state != GameStateSettings) {
    return;
  }
  if (game->settings_menu_selected_entry == GameSettingsMenuEntryBack) {
    game_set_state(game, GameStateSettings);
  }
}

void game_settings_menu_increase(Game* game) {}

void game_settings_menu_decrease(Game* game) {}