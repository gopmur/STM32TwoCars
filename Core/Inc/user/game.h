#pragma once

typedef enum _GameMainMenuEntry {
  GameMainMenuEntryPlay,
  GameMainMenuEntrySettings,
  GameMainMenuEntryAbout,
  GameMainMenuEntryCount,
} GameMainMenuEntry;

typedef enum _GameSettingsMenuEntry {
  GameSettingsMenuEntryStartHealth,
  GameSettingsMenuEntryDifficulty,
  GameSettingsMenuEntryBack,
  GameSettingsMenuEntryCount,
} GameSettingsMenuEntry;

typedef enum _GameState {
  GameStateFirstPage,
  GameStateMainMenu,
  GameStateSettings,
  GameStateAbout,
  GameStatePlaying,
} GameState;

typedef struct _Game {
  GameState state;
  GameMainMenuEntry main_menu_selected_entry;
  GameSettingsMenuEntry settings_menu_selected_entry;
} Game;

Game new_game();
void game_set_state(Game* game, GameState state);
void game_main_menu_down(Game* game);
void game_main_menu_up(Game* game);
void game_main_menu_select(Game* game);

void game_settings_menu_up(Game *game);
void game_settings_menu_down(Game *game);
void game_settings_menu_select(Game *game);
void game_settings_menu_increase(Game *game);
void game_settings_menu_decrease(Game *game);