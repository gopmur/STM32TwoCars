#pragma once

#include "main.h"

#define MIN_STARTING_HEALTH 3
#define MAX_STARTING_HEALTH 5
#define DEFAULT_SETTINGS_ENTRY GameSettingsMenuEntryStartHealth
#define DEFAULT_MAIN_MENU_ENTRY GameMainMenuEntryPlay
#define DEFAULT_GAME_STATE GameStateFirstPage
#define DEFAULT_STARTING_HEALTH 3
#define DEFAULT_DIFFICULTY GameDifficultyNormal

typedef enum _GameMainMenuEntry {
  GameMainMenuEntryPlay,
  GameMainMenuEntrySettings,
  GameMainMenuEntryAbout,
  GameMainMenuEntryCount,
  GameMainMenuEntryStart = GameMainMenuEntryPlay,
} GameMainMenuEntry;

extern const char* MAIN_MENU_ENTRY_STRINGS[GameMainMenuEntryCount];

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

typedef enum _GameDifficulty {
  GameDifficultyEasy,
  GameDifficultyNormal,
  GameDifficultyHard,
  GameDifficultyCount,
} GameDifficulty;

extern const char* GAME_DIFFICULTY_STRING[GameDifficultyCount];

typedef struct _Game {
  GameState state;
  GameMainMenuEntry main_menu_selected_entry;
  GameSettingsMenuEntry settings_menu_selected_entry;
  uint8_t starting_health;
  GameDifficulty difficulty;
} Game;

Game new_game();
void game_set_state(Game* game, GameState state);
void game_main_menu_down(Game* game);
void game_main_menu_up(Game* game);
void game_main_menu_select(Game* game);

void game_settings_menu_up(Game* game);
void game_settings_menu_down(Game* game);
void game_settings_menu_select(Game* game);
void game_settings_menu_increase(Game* game);
void game_settings_menu_decrease(Game* game);