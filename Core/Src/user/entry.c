#include "entry.h"
#include "cmsis_os.h"
#include "main.h"

#include "lcd.h"

#include <stdbool.h>

void display_thread(void* args) {
  const GPIO_TypeDef* LCD_PORT = LCD_RS_GPIO_Port;
  const uint8_t WIDTH = 20;
  const uint8_t HEIGHT = 4;

  Lcd lcd = new_lcd(LCD_PORT, LCD_RS_Pin, LCD_RW_Pin, LCD_E_Pin, LCD_D4_Pin,
                    LCD_D5_Pin, LCD_D6_Pin, LCD_D7_Pin, WIDTH, HEIGHT);
  while (true) {
    HAL_GPIO_TogglePin(LD10_GPIO_Port, LD10_Pin);
    lcd_print(&lcd, "Hello World", 0, 0);
    osDelay(100);
  }
}

void setup() {

  osThreadDef(display, display_thread, osPriorityNormal, 0, 128 * 4);
  osThreadCreate(osThread(display), NULL);
}

void loop() {}