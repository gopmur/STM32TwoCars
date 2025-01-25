#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LiquidCrystal.h"

#include "cmsis_os.h"
#include "entry.h"
#include "main.h"

#include "connectivity.uart.h"
#include "helper.h"
#include "key_pad.h"
#include "lcd.h"
#include "music_player.h"
#include "notes.h"
#include "pwm.h"
#include "rtc.h"
#include "seven_segment.h"

#include "custom_chars.h"
#include "game.h"
#include "keymap.h"
#include "message_parser.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;
extern RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef* buzzer_timer = &htim2;

Pwm buzzer;
MusicPlayer* music_player;
Uart* uart;
SevenSegment* seven_segment;

volatile Game game;

osThreadId display_thread_handle;

void seven_segment_set_health(SevenSegment* seven_segment, uint8_t health) {
  seven_segment->digits[0] = health;
}

void seven_segment_set_points(SevenSegment* seven_segment, uint8_t points) {
  for (uint8_t i = 1; i < 4; i++) {
    seven_segment->digits[i] = points % 10;
    points /= 10;
  }
}

void on_receive(Uart* uart, char* message, uint32_t message_length) {
  ParsedMessage parsed_message = parse_message(message);

  switch (parsed_message.type) {
    case MESSAGE_TYPE_SET_SECONDS:
      rtc_set_seconds(&hrtc, parsed_message.value.i32);
      srand(parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_MINUTES:
      rtc_set_minutes(&hrtc, parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_HOURS:
      rtc_set_hours(&hrtc, parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_NAME:
      game_set_player_name((Game*)&game, parsed_message.value.char_p);
      free(parsed_message.value.char_p);
      break;
    case MESSAGE_TYPE_TIME_SYN:
      rtc_syn(&hrtc, parsed_message.value.time_syn_data);
      srand(parsed_message.value.time_syn_data.second);
      break;
    case MESSAGE_TYPE_SET_DIFFICULTY:
      game_set_difficulty((Game*)&game, parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_STARTING_HEALTH:
      game_set_starting_health((Game*)&game, parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_SPEED:
      game_set_speed((Game*)&game, parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_MUSIC:
      game_set_melody((Game*)&game, parsed_message.value.i32);
      break;
  }
}

void update_display() {
  xTaskNotifyGive(display_thread_handle);
}

void display_thread(void* args) {
  extern const char* GAME_DIFFICULTY_STRING[GAME_DIFFICULTY_COUNT];
  extern const char* MAIN_MENU_ENTRY_STRINGS[GAME_MAIN_MENU_ENTRY_COUNT];

  const GPIO_TypeDef* LCD_PORT = LCD_RS_GPIO_Port;
  const uint8_t WIDTH = LCD_WIDTH;
  const uint8_t HEIGHT = LCD_HEIGHT;
  const char SELECTOR_CHAR = '>';
  const uint8_t REFRESH_PERIOD_MS = 100;
  const char LEFT_ARROW = '<';
  const char RIGHT_ARROW = '>';

  Lcd lcd =
      new_lcd((GPIO_TypeDef*)LCD_PORT, LCD_RS_Pin, LCD_RW_Pin, LCD_E_Pin,
              LCD_D4_Pin, LCD_D5_Pin, LCD_D6_Pin, LCD_D7_Pin, WIDTH, HEIGHT);
  init_custom_chars();
  lcd_clear(&lcd);
  while (true) {
    ON_VAR_CHANGE(game.state, {
      lcd_clear(&lcd);
      seven_segment_disable_all_digits(seven_segment);
    });
    switch (game.state) {
      case GAME_STATE_FIRST_PAGE:
        lcd_print(&lcd, 6, 1, "Two");
        lcd_print(&lcd, 10, 1, "Cars");
        for (uint8_t i = 0; i < LCD_WIDTH; i++) {
          for (uint8_t j = 0; j < LCD_HEIGHT; j++) {
            if (j == 1 && ((i >= 6 && i < 9) || (i >= 10 && i < 14))) {
              continue;
            }
            lcd_write(&lcd, i, j,
                      game.road[i][j] == SHAPE_CIRCLE   ? CHAR_CIRCLE
                      : game.road[i][j] == SHAPE_SQUARE ? CHAR_SQUARE
                                                        : ' ');
          }
        }
        break;
      case GAME_STATE_MAIN_MENU:
        lcd_print(&lcd, 0, 0, "Main Menu");
        for (uint8_t entry = GAME_MAIN_MENU_ENTRY_START;
             entry < GAME_MAIN_MENU_ENTRY_COUNT; entry++) {
          lcd_printf(
              &lcd, 0, entry + 1, "%c%s",
              CHAR_IF(game.main_menu_selected_entry == entry, SELECTOR_CHAR),
              MAIN_MENU_ENTRY_STRINGS[entry]);
        }
        break;
      case GAME_STATE_ABOUT:
        RTC_TimeTypeDef time = rtc_get_time(&hrtc);
        RTC_DateTypeDef date = rtc_get_date(&hrtc);
        lcd_printf(&lcd, 0, 3, "20%02d/%02d/%02d", date.Year, date.Month,
                   date.Date);
        lcd_printf(&lcd, 0, 2, "%02d:%02d:%02d", time.Hours, time.Minutes,
                   time.Seconds);
        if (game.player_name) {
          lcd_printf(&lcd, 0, 0, "%-20s", game.player_name);
        }
        lcd_printf(&lcd, 0, 1, "High Score: %d", game.high_score);
        break;
      case GAME_STATE_SETTINGS:
        lcd_print(&lcd, 0, 0, "Settings");
        lcd_printf(
            &lcd, 0, 1, "%cStarting Health %c%d%c",
            CHAR_IF(game.settings_menu_selected_entry ==
                        GAME_SETTINGS_MENU_ENTRY_START_HEALTH,
                    SELECTOR_CHAR),
            CHAR_IF(game.starting_health != MIN_STARTING_HEALTH, LEFT_ARROW),
            game.starting_health,
            CHAR_IF(game.starting_health != MAX_STARTING_HEALTH, RIGHT_ARROW));
        lcd_printf(
            &lcd, 0, 2, "%cDifficulty %c%-6s%c",
            CHAR_IF(game.settings_menu_selected_entry ==
                        GAME_SETTINGS_MENU_ENTRY_DIFFICULTY,
                    SELECTOR_CHAR),
            CHAR_IF(game.difficulty != GAME_DIFFICULTY_EASY, LEFT_ARROW),
            GAME_DIFFICULTY_STRING[game.difficulty],
            CHAR_IF(game.difficulty != GAME_DIFFICULTY_HARD, RIGHT_ARROW));

        lcd_printf(
            &lcd, 0, 3, "%cSpeed %c%-2d%c",
            CHAR_IF(game.settings_menu_selected_entry ==
                        GAME_SETTINGS_MENU_ENTRY_SPEED,
                    SELECTOR_CHAR),
            CHAR_IF(game.speed > GAME_MIN_SPEED_BLOCK_PER_S, LEFT_ARROW),
            game.speed,
            CHAR_IF(game.speed < GAME_MAX_SPEED_BLOCK_PER_S, RIGHT_ARROW));
        break;
      case GAME_STATE_COUNT_DOWN:
        lcd_printf(&lcd, 4, 1, "Get Ready! %d", game.count_down);
        break;
      case GAME_STATE_PLAYING:
        seven_segment_set_health(seven_segment, game.health);
        seven_segment_set_points(seven_segment, game.points);
        lcd_write(&lcd, WIDTH - 1, game.cars_position[DIRECTION_RIGHT],
                  CHAR_CAR_BACK);
        lcd_write(&lcd, WIDTH - 2, game.cars_position[DIRECTION_RIGHT],
                  CHAR_CAR_FRONT);

        lcd_write(
            &lcd, WIDTH - 1, !game.cars_position[DIRECTION_RIGHT],
            game.road[LCD_WIDTH - 1][!game.cars_position[DIRECTION_RIGHT]] ==
                    SHAPE_CIRCLE
                ? CHAR_CIRCLE
            : game.road[LCD_WIDTH - 1][!game.cars_position[DIRECTION_RIGHT]] ==
                    SHAPE_SQUARE
                ? CHAR_SQUARE
                : ' ');

        lcd_write(
            &lcd, WIDTH - 2, !game.cars_position[DIRECTION_RIGHT],
            game.road[LCD_WIDTH - 2][!game.cars_position[DIRECTION_RIGHT]] ==
                    SHAPE_CIRCLE
                ? CHAR_CIRCLE
            : game.road[LCD_WIDTH - 2][!game.cars_position[DIRECTION_RIGHT]] ==
                    SHAPE_SQUARE
                ? CHAR_SQUARE
                : ' ');
        lcd_write(&lcd, WIDTH - 1, 2 + game.cars_position[DIRECTION_LEFT],
                  CHAR_CAR_BACK);
        lcd_write(&lcd, WIDTH - 2, 2 + game.cars_position[DIRECTION_LEFT],
                  CHAR_CAR_FRONT);

        lcd_write(
            &lcd, WIDTH - 1, 2 + !game.cars_position[DIRECTION_LEFT],
            game.road[LCD_WIDTH - 1][2 + !game.cars_position[DIRECTION_LEFT]] ==
                    SHAPE_CIRCLE
                ? CHAR_CIRCLE
            : game.road[LCD_WIDTH - 1]
                       [2 + !game.cars_position[DIRECTION_LEFT]] == SHAPE_SQUARE
                ? CHAR_SQUARE
                : ' ');
        lcd_write(
            &lcd, WIDTH - 2, 2 + !game.cars_position[DIRECTION_LEFT],
            game.road[LCD_WIDTH - 2][2 + !game.cars_position[DIRECTION_LEFT]] ==
                    SHAPE_CIRCLE
                ? CHAR_CIRCLE
            : game.road[LCD_WIDTH - 2]
                       [2 + !game.cars_position[DIRECTION_LEFT]] == SHAPE_SQUARE
                ? CHAR_SQUARE
                : ' ');
        for (uint8_t i = 0; i < LCD_WIDTH - 2; i++) {
          for (uint8_t j = 0; j < LCD_HEIGHT; j++) {
            lcd_write(&lcd, i, j,
                      game.road[i][j] == SHAPE_CIRCLE   ? CHAR_CIRCLE
                      : game.road[i][j] == SHAPE_SQUARE ? CHAR_SQUARE
                                                        : ' ');
          }
        }
        break;
      case GAME_STATE_GAME_OVER:
        lcd_print(&lcd, 0, 0, "Game Over!");
        lcd_printf(&lcd, 0, 1, "Score: %d", game.points);
        for (uint8_t entry = 0, i = 2; entry < GAME_OVER_ENTRY_COUNT;
             entry++, i++) {
          lcd_printf(
              &lcd, 0, i, "%c%s",
              CHAR_IF(game.game_over_selected_entry == entry, SELECTOR_CHAR),
              GAME_OVER_ENTRY_STRINGS[entry]);
        }
        break;
    }
    ulTaskNotifyTake(true, REFRESH_PERIOD_MS);
  }
}

void handle_first_page_keypad(Key key) {
  game_set_state((Game*)&game, GAME_STATE_MAIN_MENU);
  update_display();
}

void handle_main_menu_keypad(Key key) {
  switch (key) {
    case KeyUp:
      game_main_menu_up((Game*)&game);
      break;
    case KeyDown:
      game_main_menu_down((Game*)&game);
      break;
    case KeySelect:
      game_main_menu_select((Game*)&game);
      break;
    default:
      break;
  }
  update_display();
}

void handle_settings_menu_keypad(Key key) {
  switch (key) {
    case KeyUp:
      game_settings_menu_up((Game*)&game);
      break;
    case KeyDown:
      game_settings_menu_down((Game*)&game);
      break;
    case KeyRight:
      game_settings_menu_increase((Game*)&game);
      break;
    case KeyLeft:
      game_settings_menu_decrease((Game*)&game);
      break;
    case KeyBack:
      game_set_state((Game*)&game, GAME_STATE_MAIN_MENU);
    default:
      break;
  }
  update_display();
}

void key_seq_get_char(Key key, char* entered_char, bool* next) {
  static uint32_t last_tick = 0;
  static Key last_key;
  static uint8_t seq = 0;

  if (strlen(KEY_SEQ[key]) == 0) {
    return;
  }

  uint32_t current_tick = HAL_GetTick();
  if (current_tick - last_tick > 1000 || last_key != key) {
    seq = 0;
    *next = true;
  } else {
    if (seq < strlen(KEY_SEQ[key]) - 1) {
      seq++;
    }
    *next = false;
  }
  *entered_char = KEY_SEQ[key][seq];
  last_key = key;
  last_tick = current_tick;
}

void handle_about_keypad(Key key) {
  switch (key) {
    case KeyBack:
      game_set_state((Game*)&game, GAME_STATE_MAIN_MENU);
      break;
    case KeyDel:
      game_delete_from_name((Game*)&game);
      break;
    default:
      char entered_char;
      bool next;
      key_seq_get_char(key, &entered_char, &next);
      game_add_to_name((Game*)&game, entered_char, next);
      break;
  }
}

void handle_playing_keypad(Key key) {
  switch (key) {
    case KeyLeft:
      game_left_car_turn((Game*)&game);
      break;
    case KeyRight:
      game_right_car_turn((Game*)&game);
      break;
    case KeyBack:
      game_set_state((Game*)&game, GAME_STATE_MAIN_MENU);
  }
  update_display();
}

void handle_game_over_keypad(Key key) {
  switch (key) {
    case KeyBack:
      game_set_state((Game*)&game, GAME_STATE_MAIN_MENU);
      break;
    case KeyUp:
      game_over_up((Game*)&game);
      break;
    case KeyDown:
      game_over_down((Game*)&game);
      break;
    case KeySelect:
      game_over_select((Game*)&game);
      break;
    default:
      break;
  }
  update_display();
}

void keypad_callback(uint8_t i, uint8_t j) {
  Key key = KEYMAP[i][j];
  switch (game.state) {
    case GAME_STATE_FIRST_PAGE:
      handle_first_page_keypad(key);
      break;
    case GAME_STATE_MAIN_MENU:
      handle_main_menu_keypad(key);
      break;
    case GAME_STATE_SETTINGS:
      handle_settings_menu_keypad(key);
      break;
    case GAME_STATE_ABOUT:
      handle_about_keypad(key);
      break;
    case GAME_STATE_PLAYING:
      handle_playing_keypad(key);
      break;
    case GAME_STATE_GAME_OVER:
      handle_game_over_keypad(key);
      break;
  }
}

void main_thread(void* arg) {
  while (true) {
    switch (game.state) {
      case GAME_STATE_COUNT_DOWN:
        osDelay(1000);
        game_count_down_tick((Game*)&game);
        break;
      case GAME_STATE_PLAYING:
        osDelay(1000 / game.speed);
        game_cars_forward((Game*)&game);
        break;
      case GAME_STATE_GAME_OVER:
      case GAME_STATE_FIRST_PAGE:
        game_shift_and_generate_road((Game*)&game);
        osDelay(1000 / GAME_MIN_SPEED_BLOCK_PER_S);
        break;
    }
  }
}

void setup() {
  uart = new_uart(&huart1, on_receive);
  uart_sendln(uart, "\n\n\n\nsystem start");
  new_key_pad(
      4, 4,
      (GPIO_TypeDef*[]){KEY_PAD_COL_1_EN_GPIO_Port, KEY_PAD_COL_2_EN_GPIO_Port,
                        KEY_PAD_COL_3_EN_GPIO_Port, KEY_PAD_COL_4_EN_GPIO_Port},
      (GPIO_TypeDef*[]){
          KEY_PAD_ROW_1_EXTI_GPIO_Port, KEY_PAD_ROW_2_EXTI_GPIO_Port,
          KEY_PAD_ROW_3_EXTI_GPIO_Port, KEY_PAD_ROW_4_EXTI_GPIO_Port},
      (uint16_t[]){KEY_PAD_COL_1_EN_Pin, KEY_PAD_COL_2_EN_Pin,
                   KEY_PAD_COL_3_EN_Pin, KEY_PAD_COL_4_EN_Pin},
      (uint16_t[]){KEY_PAD_ROW_1_EXTI_Pin, KEY_PAD_ROW_2_EXTI_Pin,
                   KEY_PAD_ROW_3_EXTI_Pin, KEY_PAD_ROW_4_EXTI_Pin},
      keypad_callback);
  buzzer = new_pwm(buzzer_timer, TIM_CHANNEL_1, APB2_CLOCK_FREQUENCY);
  music_player = new_music_player(&buzzer);
  seven_segment = new_seven_segment(
      (uint16_t[]){
          SVN_SEG_D1_Pin,
          SVN_SEG_D2_Pin,
          SVN_SEG_D3_Pin,
          SVN_SEG_D4_Pin,
      },
      SVN_SEG_DP_Pin,
      (uint16_t[]){
          SVN_SEG_BCD1_Pin,
          SVN_SEG_BCD2_Pin,
          SVN_SEG_BCD3_Pin,
          SVN_SEG_BCD4_Pin,
      },
      (GPIO_TypeDef*[]){
          SVN_SEG_D1_GPIO_Port,
          SVN_SEG_D2_GPIO_Port,
          SVN_SEG_D3_GPIO_Port,
          SVN_SEG_D4_GPIO_Port,
      },
      SVN_SEG_DP_GPIO_Port,
      (GPIO_TypeDef*[]){
          SVN_SEG_BCD1_GPIO_Port,
          SVN_SEG_BCD2_GPIO_Port,
          SVN_SEG_BCD3_GPIO_Port,
          SVN_SEG_BCD4_GPIO_Port,
      });
  osThreadDef(mainThread, main_thread, osPriorityNormal, 0, 128 * 2);
  osThreadCreate(osThread(mainThread), NULL);

  osThreadDef(displayThread, display_thread, osPriorityNormal, 0, 128 * 2);
  display_thread_handle = osThreadCreate(osThread(displayThread), NULL);

  srand(rtc_get_time(&hrtc).Seconds);

  init_sine_wave();
  game = new_game();
}