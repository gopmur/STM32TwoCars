#include <stdbool.h>

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
#include "seven_segment.h"

#include "game.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;

TIM_HandleTypeDef* buzzer_timer = &htim2;

Pwm buzzer;
MusicPlayer* music_player;
Uart* uart;
SevenSegment* seven_segment;

volatile Game game;

osThreadId display_thread_handle;

void message_receive_callback(Uart* uart,
                              char* message,
                              uint32_t message_length) {
  uart_sendln(uart, message);
}

void update_display() {
  osThreadResume(display_thread_handle);
}

void display_thread(void* args) {
  const GPIO_TypeDef* LCD_PORT = LCD_RS_GPIO_Port;
  const uint8_t WIDTH = 20;
  const uint8_t HEIGHT = 4;

  Lcd lcd =
      new_lcd((GPIO_TypeDef*)LCD_PORT, LCD_RS_Pin, LCD_RW_Pin, LCD_E_Pin,
              LCD_D4_Pin, LCD_D5_Pin, LCD_D6_Pin, LCD_D7_Pin, WIDTH, HEIGHT);
  while (true) {
    ON_VAR_CHANGE(game.state, { lcd_clear(&lcd); });
    switch (game.state) {
      case GameStateFirstPage:
        lcd_print(&lcd, 0, 0, "Two Cars");
        break;
      case GameStateMainMenu:
        lcd_print(&lcd, 0, 0, "Main Menu");
        lcd_printf(&lcd, 0, 1, "%c Start", '>');
        lcd_printf(&lcd, 0, 2, "%c Settings", ' ');
        lcd_printf(&lcd, 0, 3, "%c Exit", ' ');
        break;
      case GameStatePlaying:
        break;
    }
    osThreadSuspend(NULL);
  }
}

void keypad_callback(uint8_t i, uint8_t j) {
  switch (game.state) {
    case GameStateFirstPage:
      game_set_state(&game, GameStateMainMenu);
      update_display();
      break;
    case GameStateMainMenu:
      break;
    case GameStatePlaying:
      break;
  }
}

void main_thread(void* arg) {
  osDelay(500);
  // uart_sendln(uart, "main_thread");
  // music_player_play(music_player, doom_melody, 100, true);
  while (true) {
  }
}

void peripheral_setup() {
  uart = new_uart(&huart1, NULL);
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
  osThreadDef(mainThread, main_thread, osPriorityNormal, 0, 128 * 4);
  osThreadId main_thread_handle = osThreadCreate(osThread(mainThread), NULL);
  if (main_thread_handle == NULL) {
    uart_sendln(uart, "log: main thread did not start successfully");
  } else {
    uart_sendln(uart, "log: main thread started successfully");
  }

  osThreadDef(displayThread, display_thread, osPriorityNormal, 0, 128 * 4);
  display_thread_handle = osThreadCreate(osThread(displayThread), NULL);
  if (display_thread_handle == NULL) {
    uart_sendln(uart, "log: display thread did not start successfully");
  } else {
    uart_sendln(uart, "log: display thread started successfully");
  }
}

void setup() {
  peripheral_setup();
  os_setup();

  game = new_game();
}