#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>

#include "music_player.h"
#include "notes.h"

#include "entry.h"

#include "game.h"

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
      .starting_health = DEFAULT_STARTING_HEALTH,
      .difficulty = DEFAULT_DIFFICULTY,
      .player_name = NULL,
      .count_down = GAME_COUNT_DOWN_START,
      .cars_position[DIRECTION_LEFT] = DIRECTION_RIGHT,
      .cars_position[DIRECTION_RIGHT] = DIRECTION_LEFT,
      .health = DEFAULT_STARTING_HEALTH,
      .points = 0,
  };
  game_clear_road(&game);
  return game;
}

void game_set_state(Game* game, GameState state) {
  if (game->state == state) {
    return;
  }

  if (state == GAME_STATE_PLAYING) {
    music_player_play(music_player, doom_melody, 100, true);
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
      game->cars_position[DIRECTION_LEFT] = DIRECTION_RIGHT;
      game->cars_position[DIRECTION_RIGHT] = DIRECTION_LEFT;
      game->health = game->starting_health;
      game->points = 0;
      game_clear_road(game);
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

void game_settings_menu_select(Game* game) {
  if (game->state != GAME_STATE_SETTINGS) {
    return;
  }
  if (game->settings_menu_selected_entry == GAME_SETTINGS_MENU_ENTRY_BACK) {
    game_set_state(game, GAME_STATE_MAIN_MENU);
  }
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
    default:
      break;
  }
}

void game_set_player_name(Game* game, char* name) {
  if (game->player_name != NULL) {
    free(game->player_name);
  }
  game->player_name = name;
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
  if (game->road[LCD_WIDTH - 1][game->cars_position[DIRECTION_LEFT] + 2] ==
      SHAPE_SQUARE) {
    game->health--;
    game->road[LCD_WIDTH - 1][game->cars_position[DIRECTION_LEFT] + 2] =
        SHAPE_EMPTY;
  }
  if (game->road[LCD_WIDTH - 1][game->cars_position[DIRECTION_RIGHT]] ==
      SHAPE_SQUARE) {
    game->health--;
    game->road[LCD_WIDTH - 1][game->cars_position[DIRECTION_RIGHT]] =
        SHAPE_EMPTY;
  }
}

void game_update_points(Game* game) {
  if (game->road[LCD_WIDTH - 1][game->cars_position[DIRECTION_LEFT] + 2] ==
      SHAPE_CIRCLE) {
    game->points++;
    game->road[LCD_WIDTH - 1][game->cars_position[DIRECTION_LEFT] + 2] =
        SHAPE_EMPTY;
  }
  if (game->road[LCD_WIDTH - 1][game->cars_position[DIRECTION_RIGHT]] ==
      SHAPE_CIRCLE) {
    game->points++;
    game->road[LCD_WIDTH - 1][game->cars_position[DIRECTION_RIGHT]] =
        SHAPE_EMPTY;
  }
}

void game_cars_forward(Game* game) {
  game_update_health_circle(game);
  shift_road(game);
  game_update_health_square(game);
  game_update_points(game);
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