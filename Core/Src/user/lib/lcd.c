#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "LiquidCrystal.h"

#include "helper.h"
#include "lcd.h"

void init_lcd(GPIO_TypeDef* port,
              uint16_t rs_pin,
              uint16_t rw_pin,
              uint16_t e_pin,
              uint16_t d4_pin,
              uint16_t d5_pin,
              uint16_t d6_pin,
              uint16_t d7_pin,
              uint8_t width,
              uint8_t height){EXEC_ONCE({
  LiquidCrystal(port, rs_pin, rw_pin, e_pin, d4_pin, d5_pin, d6_pin, d7_pin);
  begin(width, height);
})}

Lcd new_lcd(GPIO_TypeDef* port,
            uint16_t rs_pin,
            uint16_t rw_pin,
            uint16_t e_pin,
            uint16_t d4_pin,
            uint16_t d5_pin,
            uint16_t d6_pin,
            uint16_t d7_pin,
            uint8_t width,
            uint8_t height) {
  Lcd lcd;

  lcd.cursor_x = 0;
  lcd.cursor_y = 0;
  lcd.current_display = malloc(width * height);
  lcd.width = width;
  lcd.height = height;

  char(*current_display)[width] = (char(*)[width])lcd.current_display;
  for (uint8_t i = 0; i < height; i++) {
    for (uint8_t j = 0; j < width; j++) {
      current_display[i][j] = ' ';
    }
  }

  init_lcd(port, rs_pin, rw_pin, e_pin, d4_pin, d5_pin, d6_pin, d7_pin, width,
           height);
  return lcd;
}

void lcd_clear(Lcd* lcd) {
  char(*current_display)[lcd->width] =
      (char(*)[lcd->width])lcd->current_display;
  for (uint8_t i = 0; i < lcd->height; i++) {
    for (uint8_t j = 0; j < lcd->width; j++) {
      current_display[i][j] = ' ';
    }
  }
  clear();
}

void inc_cursor(uint8_t* x, uint8_t* y) {
  uint8_t x_val = *x;
  uint8_t y_val = *y;
  x_val++;
  if (x_val >= LCD_WIDTH) {
    x_val = 0;
    y_val += 2;
    y_val %= LCD_HEIGHT;
  }
  *x = x_val;
  *y = y_val;
}

void lcd_inc_cursor(Lcd* lcd) {
  inc_cursor(&lcd->cursor_x, &lcd->cursor_y);
}

void lcd_set_cursor(Lcd* lcd, uint8_t x, uint8_t y) {
  lcd->cursor_x = x;
  lcd->cursor_y = y;
  setCursor(x, y);
}

void lcd_write(Lcd* lcd, uint8_t x, uint8_t y, char c) {
  char(*current_display)[lcd->width] =
      (char(*)[lcd->width])lcd->current_display;
  if (current_display[y][x] == c) {
    return;
  }
  current_display[y][x] = c;
  if (lcd->cursor_x != x || lcd->cursor_y != y) {
    setCursor(x, y);
    lcd->cursor_x = x;
    lcd->cursor_y = y;
  }
  write(c);
  lcd_inc_cursor(lcd);
}

void lcd_print(Lcd* lcd, uint8_t x, uint8_t y, const char* str) {
  uint32_t str_len = strlen(str);
  if (str_len == 0)
    return;
  for (uint8_t str_index = 0; str_index < str_len; str_index++) {
    lcd_write(lcd, x, y, str[str_index]);
    inc_cursor(&x, &y);
  }
}

void lcd_printf(Lcd* lcd, uint8_t x, uint8_t y, const char* str, ...) {
  va_list args;
  va_start(args, str);
  char buffer[lcd->width];
  vsprintf(buffer, str, args);
  lcd_print(lcd, x, y, buffer);
  va_end(args);
}