#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "connectivity.uart.h"
#include "helper.h"
#include "music_player.h"
#include "notes.h"
#include "rtc.h"

#include "entry.h"
#include "main.h"

#include "game.h"

extern RTC_HandleTypeDef hrtc;

const int16_t* melodies[] = {doom_melody, mario_melody, tetris_melody};
const uint8_t MELODY_COUNT = sizeof(melodies) / sizeof(melodies[0]);

const char* MAIN_MENU_ENTRY_STRINGS[GAME_MAIN_MENU_ENTRY_COUNT] = {
    "Play",
    "Settings",
    "About",
};

const char* GAME_DIFFICULTY_STRING[GAME_DIFFICULTY_COUNT] = {
    "Easy",
    "Normal",
    "Hard",
};

const char* GAME_OVER_ENTRY_STRINGS[GAME_OVER_ENTRY_COUNT] = {
    "Rstart",
    "Exit",
};

void game_clear_road(Game* game) {
  for (uint8_t i = 0; i < LCD_WIDTH; i++) {
    for (uint8_t j = 0; j < LCD_HEIGHT; j++) {
      game->road[i][j] = SHAPE_EMPTY;
    }
  }
}

Game new_game() {
  Game game = {
      .state = DEFAULT_GAME_STATE,
      .main_menu_selected_entry = DEFAULT_MAIN_MENU_ENTRY,
      .settings_menu_selected_entry = DEFAULT_SETTINGS_ENTRY,
      .game_over_selected_entry = DEFAULT_GAME_OVER_ENTRY,
      .starting_health = DEFAULT_STARTING_HEALTH,
      .difficulty = DEFAULT_DIFFICULTY,
      .count_down = GAME_COUNT_DOWN_START,
      .cars_position[DIRECTION_LEFT] = DIRECTION_RIGHT,
      .cars_position[DIRECTION_RIGHT] = DIRECTION_LEFT,
      .health = DEFAULT_STARTING_HEALTH,
      .points = 0,
      .speed = GAME_MIN_SPEED_BLOCK_PER_S,
      .melody = melodies[DEFAULT_MELODY],
  };
  strncpy(game.player_name, DEFAULT_PLAYER_NAME, PLAYER_MAX_NAME_LENGTH + 1);
  game_clear_road(&game);
  return game;
}

void game_set_state(Game* game, GameState state) {
  if (game->state == state) {
    return;
  }

  if (state == GAME_STATE_PLAYING) {
    music_player_play(music_player, game->melody, 100, true);
  }

  else {
    music_player_stop(music_player);
  }

  game->state = state;
  switch (state) {
    case GAME_STATE_COUNT_DOWN:
      game->count_down = GAME_COUNT_DOWN_START;
      break;
    case GAME_STATE_MAIN_MENU:
      game->main_menu_selected_entry = GAME_MAIN_MENU_ENTRY_PLAY;
      break;
    case GAME_STATE_SETTINGS:
      game->settings_menu_selected_entry =
          GAME_SETTINGS_MENU_ENTRY_START_HEALTH;
      break;
    case GAME_STATE_PLAYING:
      game->game_start_time = rtc_get_time(&hrtc);
      game->cars_position[DIRECTION_LEFT] = DIRECTION_RIGHT;
      game->cars_position[DIRECTION_RIGHT] = DIRECTION_LEFT;
      game->health = game->starting_health;
      game->points = 0;
      game_clear_road(game);
      break;
    case GAME_STATE_GAME_OVER:
      RTC_TimeTypeDef game_end_time = rtc_get_time(&hrtc);
      uart_send(uart, "     \r");
      uart_sendf(uart, "log %s,%d,%d:%d:%d,%d:%d:%d,%d,%d,%d\r",
                 game->player_name, game->points, game->game_start_time.Hours,
                 game->game_start_time.Minutes, game->game_start_time.Seconds,
                 game_end_time.Hours, game_end_time.Minutes,
                 game_end_time.Seconds, game->starting_health, game->difficulty,
                 game->speed);
      game->game_over_selected_entry = GAME_OVER_ENTRY_RESTART;
      break;
  }
}

void game_main_menu_down(Game* game) {
  if (game->main_menu_selected_entry + 1 >= GAME_MAIN_MENU_ENTRY_COUNT ||
      game->state != GAME_STATE_MAIN_MENU) {
    return;
  }
  game->main_menu_selected_entry++;
}

void game_main_menu_up(Game* game) {
  if (game->main_menu_selected_entry <= 0 ||
      game->state != GAME_STATE_MAIN_MENU) {
    return;
  }
  game->main_menu_selected_entry--;
}

void game_main_menu_select(Game* game) {
  if (game->state != GAME_STATE_MAIN_MENU) {
    return;
  }
  switch (game->main_menu_selected_entry) {
    case GAME_MAIN_MENU_ENTRY_PLAY:
      game_set_state(game, GAME_STATE_COUNT_DOWN);
      break;
    case GAME_MAIN_MENU_ENTRY_SETTINGS:
      game_set_state(game, GAME_STATE_SETTINGS);
      break;
    case GAME_MAIN_MENU_ENTRY_ABOUT:
      game_set_state(game, GAME_STATE_ABOUT);
      break;
    default:
      break;
  }
}

void game_settings_menu_up(Game* game) {
  if (game->state != GAME_STATE_SETTINGS ||
      game->settings_menu_selected_entry <= 0) {
    return;
  }
  game->settings_menu_selected_entry--;
}

void game_settings_menu_down(Game* game) {
  if (game->state != GAME_STATE_SETTINGS ||
      game->settings_menu_selected_entry + 1 >=
          GAME_SETTINGS_MENU_ENTRY_COUNT) {
    return;
  }
  game->settings_menu_selected_entry++;
}

void game_settings_menu_increase(Game* game) {
  if (game->state != GAME_STATE_SETTINGS) {
    return;
  }
  switch (game->settings_menu_selected_entry) {
    case GAME_SETTINGS_MENU_ENTRY_START_HEALTH:
      if (game->starting_health < MAX_STARTING_HEALTH) {
        game->starting_health++;
      }
      break;
    case GAME_SETTINGS_MENU_ENTRY_DIFFICULTY:
      if (game->difficulty < GAME_DIFFICULTY_HARD) {
        game->difficulty++;
      }
      break;
    case GAME_SETTINGS_MENU_ENTRY_SPEED:
      if (game->speed + GAME_SPEED_STEP <= GAME_MAX_SPEED_BLOCK_PER_S) {
        game->speed += GAME_SPEED_STEP;
      }
      break;
    default:
      break;
  }
}

void game_settings_menu_decrease(Game* game) {
  if (game->state != GAME_STATE_SETTINGS) {
    return;
  }
  switch (game->settings_menu_selected_entry) {
    case GAME_SETTINGS_MENU_ENTRY_START_HEALTH:
      if (game->starting_health > MIN_STARTING_HEALTH) {
        game->starting_health--;
      }
      break;
    case GAME_SETTINGS_MENU_ENTRY_DIFFICULTY:
      if (game->difficulty > 0) {
        game->difficulty--;
      }
      break;
    case GAME_SETTINGS_MENU_ENTRY_SPEED:
      if ((int16_t)game->speed - GAME_SPEED_STEP >=
          GAME_MIN_SPEED_BLOCK_PER_S) {
        game->speed -= GAME_SPEED_STEP;
      }
      break;
    default:
      break;
  }
}

void game_set_player_name(Game* game, char* name) {
  if (name == NULL || strlen(name) > PLAYER_MAX_NAME_LENGTH) {
    return;
  }
  strncpy(game->player_name, name, PLAYER_MAX_NAME_LENGTH + 1);
}

void game_count_down_tick(Game* game) {
  if (game->count_down == 0) {
    game_set_state(game, GAME_STATE_PLAYING);
  }
  game->count_down--;
}

void shift_road(Game* game) {
  for (uint8_t i = LCD_WIDTH - 1; i > 0; i--) {
    for (uint8_t j = 0; j < LCD_HEIGHT; j++) {
      game->road[i][j] = game->road[i - 1][j];
    }
  }
  for (uint8_t j = 0; j < LCD_HEIGHT; j++) {
    game->road[0][j] = SHAPE_EMPTY;
  }
}

void game_generate_shape_left(Game* game) {
  static uint8_t distance_to_last_generation = 0;
  bool generate_circle = rand() > 3 * (float)RAND_MAX / (game->difficulty + 4);
  bool generate_square = rand() > 3 * (float)RAND_MAX / (game->difficulty + 4);
  if (distance_to_last_generation <
      MIN_DISTANCE_BETWEEN_GENERATIONS +
          (GAME_DIFFICULTY_COUNT - game->difficulty)) {
    distance_to_last_generation++;
    return;
  }
  distance_to_last_generation = 0;
  Direction side = rand() % 2;
  if (generate_circle) {
    game->road[0][side + 2] = SHAPE_CIRCLE;
  }
  if (generate_square) {
    game->road[0][!side + 2] = SHAPE_SQUARE;
  }
}

void game_generate_shape_right(Game* game) {
  static uint8_t distance_to_last_generation = 0;
  bool generate_circle = rand() > 3 * (float)RAND_MAX / (game->difficulty + 4);
  bool generate_square = rand() > 3 * (float)RAND_MAX / (game->difficulty + 4);
  if (distance_to_last_generation <
      MIN_DISTANCE_BETWEEN_GENERATIONS +
          (GAME_DIFFICULTY_COUNT - game->difficulty)) {
    distance_to_last_generation++;
    return;
  }
  distance_to_last_generation = 0;
  Direction side = rand() % 2;
  if (generate_circle) {
    game->road[0][side] = SHAPE_CIRCLE;
  }
  if (generate_square) {
    game->road[0][!side] = SHAPE_SQUARE;
  }
}

void game_update_health_circle(Game* game) {
  if (game->road[LCD_WIDTH - 1][!game->cars_position[DIRECTION_LEFT] + 2] ==
      SHAPE_CIRCLE) {
    game->health--;
  }
  if (game->road[LCD_WIDTH - 1][!game->cars_position[DIRECTION_RIGHT]] ==
      SHAPE_CIRCLE) {
    game->health--;
  }
}

void game_update_health_square(Game* game) {
  for (uint8_t i = 1; i <= CAR_LENGTH; i++) {
    if (game->road[LCD_WIDTH - i][game->cars_position[DIRECTION_LEFT] + 2] ==
        SHAPE_SQUARE) {
      game->health--;
      game->road[LCD_WIDTH - i][game->cars_position[DIRECTION_LEFT] + 2] =
          SHAPE_EMPTY;
      music_player_push(music_player, sine_wave);
    }
    if (game->road[LCD_WIDTH - i][game->cars_position[DIRECTION_RIGHT]] ==
        SHAPE_SQUARE) {
      game->health--;
      game->road[LCD_WIDTH - i][game->cars_position[DIRECTION_RIGHT]] =
          SHAPE_EMPTY;
      music_player_push(music_player, sine_wave);
    }
  }
}

void game_update_points(Game* game) {
  bool points_changed = false;
  for (uint8_t i = 1; i <= CAR_LENGTH; i++) {
    if (game->road[LCD_WIDTH - i][game->cars_position[DIRECTION_LEFT] + 2] ==
        SHAPE_CIRCLE) {
      game->points++;
      points_changed = true;
      game->road[LCD_WIDTH - i][game->cars_position[DIRECTION_LEFT] + 2] =
          SHAPE_EMPTY;
      music_player_push(music_player, sine_wave);
    }
    if (game->road[LCD_WIDTH - i][game->cars_position[DIRECTION_RIGHT]] ==
        SHAPE_CIRCLE) {
      game->points++;
      points_changed = true;
      game->road[LCD_WIDTH - i][game->cars_position[DIRECTION_RIGHT]] =
          SHAPE_EMPTY;
      music_player_push(music_player, sine_wave);
    }
  }
  if (points_changed) {
    const int16_t BASE_TEMPO = game->melody[0];
    int16_t tempo = BASE_TEMPO + MIN(game->points, MAX_TEMPO_SCORES) *
                                     (MAX_TEMPO - BASE_TEMPO) /
                                     MAX_TEMPO_SCORES;
    music_player_set_tempo(music_player, tempo);
  }
}

void game_shift_and_generate_road(Game* game) {
  shift_road(game);
  game_generate_shape_left(game);
  game_generate_shape_right(game);
}

void game_cars_forward(Game* game) {
  game_update_health_circle(game);
  shift_road(game);
  game_update_health_square(game);
  game_update_points(game);

  if (game->health < 0) {
    game_set_state(game, GAME_STATE_GAME_OVER);
    game->high_score = MAX(game->high_score, game->points);
    return;
  }

  game_generate_shape_left(game);
  game_generate_shape_right(game);
}

void game_left_car_turn(Game* game) {
  if (game->state != GAME_STATE_PLAYING) {
    return;
  }
  game->cars_position[DIRECTION_LEFT] = !game->cars_position[DIRECTION_LEFT];
}
void game_right_car_turn(Game* game) {
  if (game->state != GAME_STATE_PLAYING) {
    return;
  }
  game->cars_position[DIRECTION_RIGHT] = !game->cars_position[DIRECTION_RIGHT];
}

void game_over_up(Game* game) {
  if (game->game_over_selected_entry <= 0 ||
      game->state != GAME_STATE_GAME_OVER) {
    return;
  }
  game->game_over_selected_entry--;
}
void game_over_down(Game* game) {
  if (game->game_over_selected_entry + 1 >= GAME_OVER_ENTRY_COUNT ||
      game->state != GAME_STATE_GAME_OVER) {
    return;
  }
  game->game_over_selected_entry++;
}

void game_over_select(Game* game) {
  switch (game->game_over_selected_entry) {
    case GAME_OVER_ENTRY_EXIT:
      game_set_state(game, GAME_STATE_MAIN_MENU);
      break;
    case GAME_OVER_ENTRY_RESTART:
      game_set_state(game, GAME_STATE_COUNT_DOWN);
      break;
  }
}

void game_set_difficulty(Game* game, GameDifficulty difficulty) {
  if (difficulty >= GAME_DIFFICULTY_COUNT) {
    return;
  }
  game->difficulty = difficulty;
}

void game_set_starting_health(Game* game, uint8_t starting_health) {
  if (starting_health > MAX_STARTING_HEALTH ||
      starting_health < MIN_STARTING_HEALTH) {
    return;
  }
  game->starting_health = starting_health;
}

void game_set_speed(Game* game, uint16_t speed) {
  if (speed > GAME_MAX_SPEED_BLOCK_PER_S ||
      speed < GAME_MIN_SPEED_BLOCK_PER_S ||
      (speed - GAME_MIN_SPEED_BLOCK_PER_S) % GAME_SPEED_STEP != 0) {
    return;
  }
  game->speed = speed;
}

void game_set_melody(Game* game, uint8_t melody) {
  if (melody >= MELODY_COUNT) {
    return;
  }
  if (music_player->is_active) {
    music_player_stop(music_player);
    music_player_play(music_player, melodies[melody], 100, true);
  }
  game->melody = melodies[melody];
}

void game_delete_from_name(Game* game) {
  if (game->state != GAME_STATE_ABOUT) {
    return;
  }
  uint8_t name_length = strlen(game->player_name);
  if (name_length == 0) {
    return;
  }
  game->player_name[name_length - 1] = '\0';
}