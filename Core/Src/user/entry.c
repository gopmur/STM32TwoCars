#include "entry.h"
#include "cmsis_os.h"
#include "main.h"

#include "button.h"
#include "connectivity.uart.h"
#include "helper.h"
#include "lcd.h"

#include <stdbool.h>

extern UART_HandleTypeDef huart1;

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

void button_callback(void* args) {
  HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
}

void main_thread(void* arg) {
  while (true) {
  }
}

void peripheral_setup() {
  uart = new_uart(&huart1, NULL);
  uart_sendln(uart, "\n\n\n\nsystem start");
  HAL_GPIO_WritePin(KEY_PAD_COL_1_EN_GPIO_Port, KEY_PAD_COL_1_EN_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(KEY_PAD_COL_2_EN_GPIO_Port, KEY_PAD_COL_2_EN_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(KEY_PAD_COL_3_EN_GPIO_Port, KEY_PAD_COL_3_EN_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(KEY_PAD_COL_4_EN_GPIO_Port, KEY_PAD_COL_4_EN_Pin,
                    GPIO_PIN_SET);

  new_button(KEY_PAD_ROW_1_EXTI_GPIO_Port, KEY_PAD_ROW_1_EXTI_Pin,
             button_callback, NULL, 0);
  new_button(KEY_PAD_ROW_2_EXTI_GPIO_Port, KEY_PAD_ROW_2_EXTI_Pin,
             button_callback, NULL, 0);
  new_button(KEY_PAD_ROW_3_EXTI_GPIO_Port, KEY_PAD_ROW_3_EXTI_Pin,
             button_callback, NULL, 0);
  new_button(KEY_PAD_ROW_4_EXTI_GPIO_Port, KEY_PAD_ROW_4_EXTI_Pin,
             button_callback, NULL, 0);
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
}

void setup() {
  peripheral_setup();
  os_setup();
}