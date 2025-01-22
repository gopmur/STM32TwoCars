#include "entry.h"
#include "cmsis_os.h"
#include "main.h"

#include "connectivity.uart.h"
#include "helper.h"
#include "lcd.h"

#include <stdbool.h>

extern UART_HandleTypeDef huart1;

Uart* uart;

// read only
osThreadId button_interrupt_thread_handle;

void message_receive_callback(Uart* uart,
                              char* message,
                              uint32_t message_length) {
  uart_sendln(uart, message);
}

void button_interrupt_thread(void* args) {
  while (true) {
    osThreadSuspend(NULL);
    HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
  }
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

void setup() {
  uart = new_uart(&huart1, message_receive_callback);

  osThreadDef(display, display_thread, osPriorityNormal, 0, 128);
  osThreadId display_thread_handle = osThreadCreate(osThread(display), NULL);
  if (display_thread_handle == NULL) {
    uart_sendln(uart, "log: display thread did not start successfully");
  }

  osThreadDef(button_interrupt, button_interrupt_thread, osPriorityNormal, 0,
              128);

  button_interrupt_thread_handle =
      osThreadCreate(osThread(button_interrupt), NULL);
  if (button_interrupt_thread_handle == NULL) {
    uart_sendln(uart,
                "log: button_interrupt thread did not start successfully");
  }

  HAL_GPIO_WritePin(KEY_PAD_COL_1_EN_GPIO_Port, KEY_PAD_COL_1_EN_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(KEY_PAD_COL_2_EN_GPIO_Port, KEY_PAD_COL_2_EN_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(KEY_PAD_COL_3_EN_GPIO_Port, KEY_PAD_COL_3_EN_Pin,
                    GPIO_PIN_SET);
  HAL_GPIO_WritePin(KEY_PAD_COL_4_EN_GPIO_Port, KEY_PAD_COL_4_EN_Pin,
                    GPIO_PIN_SET);
}