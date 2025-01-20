#include <stdlib.h>

#include "LiquidCrystal.h"

#include "game.h"

#include "main.h"

extern RTC_HandleTypeDef hrtc;

Flower new_flower() {
  Flower flower = {
      .state = FLOWER_STATE_SATISFIED,
      .warning = false,
  };
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  flower.state_change_timestamp = time;
  return flower;
}

void flower_set_state(Flower* flower, FlowerState state) {
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  flower_reset_warning(flower);
  flower->state_change_timestamp = time;
  flower->state = state;
}

void flower_set_warning(Flower* flower) {
  flower->warning = true;
}

void flower_reset_warning(Flower* flower) {
  flower->warning = false;
}

void flower_new_need(Flower* flower) {
  uint8_t random_number = rand() % 2;
  switch (random_number) {
    case 0:
      flower_set_state(flower, FLOWER_STATE_NEEDS_LIGHT);
      break;
    default:
      flower_set_state(flower, FLOWER_STATE_NEEDS_WATER);
      break;
  }
}

Game new_game() {
  Game game = {
      .current_flower = 0,
      .score = 0,
      .state = GAME_STATE_WAITING_FOR_START,
  };
  for (uint8_t flower_index = 0; flower_index < FLOWER_COUNT; flower_index++) {
    game.flowers[flower_index] = new_flower();
  }
  return game;
}

void game_set_state(Game* game, GameState state) {
  if (game->state == state)
    return;
  game->state = state;
  if (game->state == GAME_STATE_RUNNING) {
    for (uint8_t flower_index = 0; flower_index < FLOWER_COUNT;
         flower_index++) {
      game->flowers[flower_index] = new_flower();
    }
  }
}

void game_select_flower(Game* game, uint8_t flower_index) {
  if (game->state != GAME_STATE_RUNNING || flower_index >= 3)
    return;
  game->current_flower = flower_index;
}