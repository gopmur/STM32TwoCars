#pragma once

#define LCD_WIDTH 20
#define LCD_HEIGHT 4

typedef struct _Lcd {
  char* current_display;
  uint8_t cursor_x;
  uint8_t cursor_y;
  uint8_t width;
  uint8_t height;
} Lcd;

Lcd new_lcd(GPIO_TypeDef* port,
            uint16_t rs_pin,
            uint16_t rw_pin,
            uint16_t e_pin,
            uint16_t d4_pin,
            uint16_t d5_pin,
            uint16_t d6_pin,
            uint16_t d7_pin,
            uint8_t width,
            uint8_t height);
void lcd_clear(Lcd* lcd);
void lcd_write(Lcd* lcd, char c, uint8_t x, uint8_t y);
void lcd_print(Lcd* lcd, const char* str, uint8_t x, uint8_t y);
