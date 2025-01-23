#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

void on_receive(Uart* uart, char* message, uint32_t message_length) {
  ParsedMessage parsed_message = parse_message(message);

  switch (parsed_message.type) {
    case MESSAGE_TYPE_SET_SECONDS:
      rtc_set_seconds(&hrtc, parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_MINUTES:
      rtc_set_minutes(&hrtc, parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_HOURS:
      rtc_set_hours(&hrtc, parsed_message.value.i32);
      break;
    case MESSAGE_TYPE_SET_NAME:
      game_set_player_name(&game, parsed_message.value.char_p);
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
  const uint8_t WIDTH = 20;
  const uint8_t HEIGHT = 4;
  const char SELECTOR_CHAR = '>';
  const uint8_t REFRESH_PERIOD_MS = 100;
  const char LEFT_ARROW = '<';
  const char RIGHT_ARROW = '>';

  Lcd lcd =
      new_lcd((GPIO_TypeDef*)LCD_PORT, LCD_RS_Pin, LCD_RW_Pin, LCD_E_Pin,
              LCD_D4_Pin, LCD_D5_Pin, LCD_D6_Pin, LCD_D7_Pin, WIDTH, HEIGHT);
  while (true) {
    ON_VAR_CHANGE(game.state, { lcd_clear(&lcd); });
    switch (game.state) {
      case GAME_STATE_FIRST_PAGE:
        lcd_print(&lcd, 0, 0, "Two Cars");
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
      case GAME_STATE_PLAYING:
        break;
      case GAME_STATE_ABOUT:
        RTC_TimeTypeDef time = rtc_get_time(&hrtc);
        lcd_printf(&lcd, 0, 2, "%02d:%02d:%02d", time.Hours, time.Minutes,
                   time.Seconds);
        if (game.player_name) {
          lcd_printf(&lcd, 0, 0,
                     game.player_name);
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

        lcd_printf(&lcd, 0, 3, "%cBack",
                   CHAR_IF(game.settings_menu_selected_entry ==
                               GAME_SETTINGS_MENU_ENTRY_BACK,
                           SELECTOR_CHAR));

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
    default:
    case KeySelect:
      game_main_menu_select((Game*)&game);
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
    case KeySelect:
      game_settings_menu_select((Game*)&game);
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

void handle_about_keypad(Key key) {
  switch (key) {
    case KeyBack:
      game_set_state((Game*)&game, GAME_STATE_MAIN_MENU);
      break;
    default:
      break;
  }
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
      break;
  }
}

void main_thread(void* arg) {
  osDelay(500);
  uart_sendln(uart, "main_thread");
  // music_player_play(music_player, doom_melody, 100, true);
  while (true) {
  }
}

void peripheral_setup() {
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
}

void os_setup() {
  osThreadDef(mainThread, main_thread, osPriorityNormal, 0, 128 * 1);
  osThreadId main_thread_handle = osThreadCreate(osThread(mainThread), NULL);
  if (main_thread_handle == NULL) {
    // uart_sendln(uart, "log: main thread did not start successfully");
  } else {
    // uart_sendln(uart, "log: main thread started successfully");
  }

  osThreadDef(displayThread, display_thread, osPriorityNormal, 0, 128 * 2);
  display_thread_handle = osThreadCreate(osThread(displayThread), NULL);
  if (display_thread_handle == NULL) {
    // uart_sendln(uart, "log: display thread did not start successfully");
  } else {
    // uart_sendln(uart, "log: display thread started successfully");
  }
}

void setup() {
  peripheral_setup();
  os_setup();

  game = new_game();
}