#include "entry.h"
#include "cmsis_os.h"
#include "main.h"

#include "connectivity.uart.h"
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
  }
}

void setup() {
  uart = new_uart(&huart1, message_receive_callback);

  osThreadDef(display, display_thread, osPriorityNormal, 0, 128 * 4);
  osThreadCreate(osThread(display), NULL);
}

void loop() {}