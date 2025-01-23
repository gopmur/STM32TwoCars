#include "game.h"

const char* MAIN_MENU_ENTRY_STRINGS[GameMainMenuEntryCount] = {
    "Play",
    "Settings",
    "About",
};

const char* GAME_DIFFICULTY_STRING[GameDifficultyCount] = {
    "Easy",
    "Normal",
    "Hard",
};

Game new_game() {
  Game game = {
      .state = DEFAULT_GAME_STATE,
      .main_menu_selected_entry = DEFAULT_MAIN_MENU_ENTRY,
      .settings_menu_selected_entry = DEFAULT_SETTINGS_ENTRY,
      .starting_health = DEFAULT_STARTING_HEALTH,
      .difficulty = DEFAULT_DIFFICULTY,
  };
  return game;
}

void game_set_state(Game* game, GameState state) {
  if (game->state == state) {
    return;
  }
  game->state = state;
  switch (state) {
    case GameStateMainMenu:
      game->main_menu_selected_entry = GameMainMenuEntryPlay;
      break;
    case GameStateSettings:
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
    game_set_state(game, GameStateMainMenu);
  }
}

void game_settings_menu_increase(Game* game) {
  if (game->state != GameStateSettings) {
    return;
  }
  switch (game->settings_menu_selected_entry) {
    case GameSettingsMenuEntryStartHealth:
      if (game->starting_health < MAX_STARTING_HEALTH) {
        game->starting_health++;
      }
      break;
    case GameSettingsMenuEntryDifficulty:
      if (game->difficulty < GameDifficultyHard) {
        game->difficulty++;
      }
      break;
    default:
      break;
  }
}

void game_settings_menu_decrease(Game* game) {
  if (game->state != GameStateSettings) {
    return;
  }
  switch (game->settings_menu_selected_entry) {
    case GameSettingsMenuEntryStartHealth:
      if (game->starting_health > 0) {
        game->starting_health--;
      }
      break;
    case GameSettingsMenuEntryDifficulty:
      if (game->difficulty > 0) {
        game->difficulty--;
      }
      break;
    default:
      break;
  }
}