#include "LiquidCrystal.h"

#include "custom_chars.h"

char car_back_char[] = {0x00, 0x06, 0x1F, 0x1F, 0x1F, 0x1F, 0x06, 0x00};
char car_front_char[] = {0x00, 0x06, 0x1F, 0x17, 0x17, 0x1F, 0x06, 0x00};

void init_custom_chars() {
  createChar(CHAR_CAR_BACK, (uint8_t*)car_back_char);
  createChar(CHAR_CAR_FRONT, (uint8_t*)car_front_char);
}