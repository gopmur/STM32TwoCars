#include "entry.h"
#include "cmsis_os.h"
#include "main.h"

#include "connectivity.uart.h"
#include "helper.h"
#include "key_pad.h"
#include "lcd.h"
#include "music_player.h"
#include "pwm.h"
#include "notes.h"

#include <stdbool.h>

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;

TIM_HandleTypeDef* buzzer_timer = &htim2;

Pwm buzzer;
MusicPlayer music_player;
Uart* uart;

void message_receive_callback(Uart* uart,
                              char* message,
                              uint32_t message_length) {
  uart_sendln(uart, message);
}

void display_thread(void* args) {
  const GPIO_TypeDef* LCD_PORT = LCD_RS_GPIO_Port;
  const uint8_t WIDTH = 20;
  const uint8_t HEIGHT = 4;

  Lcd lcd =
      new_lcd((GPIO_TypeDef*)LCD_PORT, LCD_RS_Pin, LCD_RW_Pin, LCD_E_Pin,
              LCD_D4_Pin, LCD_D5_Pin, LCD_D6_Pin, LCD_D7_Pin, WIDTH, HEIGHT);
  while (true) {
    osThreadSuspend(NULL);
  }
}

void keypad_callback(uint8_t i, uint8_t j) {
  uart_sendf(uart, "keypad_callback: %d %d\n", i, j);
}

void main_thread(void* arg) {
  osDelay(500);
  uart_sendln(uart, "main_thread");
  music_player_play(&music_player, doom_melody, 100);
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
}

void os_setup() {
  osThreadDef(main, main_thread, osPriorityNormal, 0, 128);
  osThreadId main_thread_handle = osThreadCreate(osThread(main), NULL);
  if (main_thread_handle == NULL) {
    uart_sendln(uart, "log: main thread did not start successfully");
  } else {
    uart_sendln(uart, "log: main thread started successfully");
  }

  osThreadDef(display, display_thread, osPriorityNormal, 0, 128);
  osThreadId display_thread_handle = osThreadCreate(osThread(display), NULL);
  if (display_thread_handle == NULL) {
    uart_sendln(uart, "log: display thread did not start successfully");
  } else {
    uart_sendln(uart, "log: display thread started successfully");
  }

  buzzer = new_pwm(buzzer_timer, TIM_CHANNEL_1, APB2_CLOCK_FREQUENCY);
  music_player = new_music_player(&buzzer);
}

void setup() {
  peripheral_setup();
  os_setup();
}