#pragma once

#include <stdbool.h>

#define FLOWER_COUNT 3

typedef enum _GameState {
  GAME_STATE_WAITING_FOR_START,
  GAME_STATE_RUNNING,
  GAME_STATE_OVER,
} GameState;

typedef enum _FlowerState {
  FLOWER_STATE_SATISFIED,
  FLOWER_STATE_NEEDS_WATER,
  FLOWER_STATE_NEEDS_LIGHT,
  FLOWER_STATE_DEAD,
} FlowerState;

typedef struct _Flower {
  volatile FlowerState state;
  volatile RTC_TimeTypeDef state_change_timestamp;
  volatile bool warning;
} Flower;

void flower_set_state(Flower* flower, FlowerState state);
void flower_new_need(Flower* flower);
void flower_set_warning(Flower* flower);
void flower_reset_warning(Flower* flower);
typedef struct _Game {
  volatile GameState state;
  volatile uint8_t current_flower;
  volatile uint16_t score;
  Flower flowers[FLOWER_COUNT];
} Game;

Game new_game();
void game_set_state(Game* game, GameState state);
void game_select_flower(Game* game, uint8_t flower_index);