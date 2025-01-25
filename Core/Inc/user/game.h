#pragma once

#include "main.h"

#define MIN_STARTING_HEALTH 3
#define MAX_STARTING_HEALTH 5
#define DEFAULT_SETTINGS_ENTRY GAME_SETTINGS_MENU_ENTRY_START_HEALTH
#define DEFAULT_MAIN_MENU_ENTRY GAME_MAIN_MENU_ENTRY_PLAY
#define DEFAULT_GAME_OVER_ENTRY GAME_OVER_ENTRY_RESTART
#define DEFAULT_GAME_STATE GAME_STATE_FIRST_PAGE
#define DEFAULT_STARTING_HEALTH 3
#define DEFAULT_MELODY 0
#define DEFAULT_DIFFICULTY GAME_DIFFICULTY_NORMAL
#define GAME_COUNT_DOWN_START 3
#define MIN_DISTANCE_BETWEEN_GENERATIONS 2
#define MAX_TEMPO 90
#define MAX_TEMPO_SCORES 50
#define GAME_MAX_SPEED_BLOCK_PER_S 10
#define GAME_MIN_SPEED_BLOCK_PER_S 1
#define GAME_SPEED_STEP 1
#define CAR_LENGTH 2

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
  GAME_SETTINGS_MENU_ENTRY_SPEED,
  GAME_SETTINGS_MENU_ENTRY_COUNT,
} GameSettingsMenuEntry;

typedef enum _GameOverEntry {
  GAME_OVER_ENTRY_RESTART,
  GAME_OVER_ENTRY_EXIT,
  GAME_OVER_ENTRY_COUNT,
} GameOverEntry;

extern const char* GAME_OVER_ENTRY_STRINGS[GAME_OVER_ENTRY_COUNT];

typedef enum _GameState {
  GAME_STATE_FIRST_PAGE,
  GAME_STATE_MAIN_MENU,
  GAME_STATE_SETTINGS,
  GAME_STATE_ABOUT,
  GAME_STATE_COUNT_DOWN,
  GAME_STATE_PLAYING,
  GAME_STATE_GAME_OVER,
} GameState;

typedef enum _GameDifficulty {
  GAME_DIFFICULTY_EASY,
  GAME_DIFFICULTY_NORMAL,
  GAME_DIFFICULTY_HARD,
  GAME_DIFFICULTY_COUNT,
} GameDifficulty;

extern const char* GAME_DIFFICULTY_STRING[GAME_DIFFICULTY_COUNT];

typedef enum _Direction {
  DIRECTION_RIGHT,
  DIRECTION_LEFT,
} Direction;

typedef enum _Shape {
  SHAPE_CIRCLE,
  SHAPE_SQUARE,
  SHAPE_EMPTY,
} Shape;

typedef struct _Game {
  GameState state;
  GameMainMenuEntry main_menu_selected_entry;
  GameSettingsMenuEntry settings_menu_selected_entry;
  GameOverEntry game_over_selected_entry;
  uint8_t starting_health;
  GameDifficulty difficulty;
  uint16_t high_score;
  uint8_t count_down;
  char* player_name;
  Direction cars_position[2];
  Shape road[LCD_WIDTH][LCD_HEIGHT];
  int8_t health;
  uint16_t points;
  uint16_t speed;
  int16_t* melody;
  RTC_TimeTypeDef game_start_time;
} Game;

Game new_game();
void game_set_state(Game* game, GameState state);
void game_main_menu_down(Game* game);
void game_main_menu_up(Game* game);
void game_main_menu_select(Game* game);
void game_settings_menu_up(Game* game);
void game_settings_menu_down(Game* game);
void game_settings_menu_increase(Game* game);
void game_settings_menu_decrease(Game* game);
void game_over_up(Game* game);
void game_over_down(Game* game);
void game_over_select(Game* game);
void game_set_player_name(Game* game, char* name);
void game_count_down_tick(Game* game);
void game_cars_forward(Game* game);
void game_left_car_turn(Game* game);
void game_right_car_turn(Game* game);
void game_set_difficulty(Game* game, GameDifficulty difficulty);
void game_set_starting_health(Game* game, uint8_t starting_health);
void game_set_speed(Game* game, uint16_t speed);
void game_set_melody(Game* game, uint8_t melody);
void shift_and_generate_road(Game* game);