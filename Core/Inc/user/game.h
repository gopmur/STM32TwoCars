#pragma once

#include "main.h"

#define MIN_STARTING_HEALTH 3
#define MAX_STARTING_HEALTH 5
#define DEFAULT_SETTINGS_ENTRY GAME_SETTINGS_MENU_ENTRY_START_HEALTH
#define DEFAULT_MAIN_MENU_ENTRY GAME_MAIN_MENU_ENTRY_PLAY
#define DEFAULT_GAME_STATE GAME_STATE_FIRST_PAGE
#define DEFAULT_STARTING_HEALTH 3
#define DEFAULT_DIFFICULTY GAME_DIFFICULTY_NORMAL
#define GAME_COUNT_DOWN_START 3

typedef enum _GameMainMenuEntry {
  GAME_MAIN_MENU_ENTRY_PLAY,
  GAME_MAIN_MENU_ENTRY_SETTINGS,
  GAME_MAIN_MENU_ENTRY_ABOUT,
  GAME_MAIN_MENU_ENTRY_COUNT,
  GAME_MAIN_MENU_ENTRY_START = GAME_MAIN_MENU_ENTRY_PLAY,
} GameMainMenuEntry;

extern const char* MAIN_MENU_ENTRY_STRINGS[GAME_MAIN_MENU_ENTRY_COUNT];

typedef enum _GameSettingsMenuEntry {
  GAME_SETTINGS_MENU_ENTRY_START_HEALTH,
  GAME_SETTINGS_MENU_ENTRY_DIFFICULTY,
  GAME_SETTINGS_MENU_ENTRY_BACK,
  GAME_SETTINGS_MENU_ENTRY_COUNT,
} GameSettingsMenuEntry;

typedef enum _GameState {
  GAME_STATE_FIRST_PAGE,
  GAME_STATE_MAIN_MENU,
  GAME_STATE_SETTINGS,
  GAME_STATE_ABOUT,
  GAME_STATE_COUNT_DOWN,
  GAME_STATE_PLAYING,
} GameState;

typedef enum _GameDifficulty {
  GAME_DIFFICULTY_EASY,
  GAME_DIFFICULTY_NORMAL,
  GAME_DIFFICULTY_HARD,
  GAME_DIFFICULTY_COUNT,
} GameDifficulty;

extern const char* GAME_DIFFICULTY_STRING[GAME_DIFFICULTY_COUNT];

typedef struct _Game {
  GameState state;
  GameMainMenuEntry main_menu_selected_entry;
  GameSettingsMenuEntry settings_menu_selected_entry;
  uint8_t starting_health;
  GameDifficulty difficulty;
  uint32_t high_score;
  uint8_t count_down;
  char* player_name;
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
void game_set_player_name(Game* game, char* name);
void game_count_down_tick(Game *game);