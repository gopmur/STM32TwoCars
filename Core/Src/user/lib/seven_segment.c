#include <malloc.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "cmsis_os.h"

#include "helper.h"
#include "main.h"
#include "seven_segment.h"

void seven_segment_thread(SevenSegment* seven_segment) {
  while (true) {
    seven_segment_loop(seven_segment);
    osDelay(SEVEN_SEGMENT_UPDATE_PERIOD_MS);
  }
}

SevenSegment* new_seven_segment(uint16_t digit_pins[4],
                                uint16_t dp_pin,
                                uint16_t bcd_pins[4],
                                GPIO_TypeDef* digit_ports[4],
                                GPIO_TypeDef* dp_port,
                                GPIO_TypeDef* bcd_ports[4]) {
  SevenSegment* seven_segment = malloc(sizeof(SevenSegment));
  seven_segment->dp_pin = dp_pin;
  seven_segment->dp_port = dp_port;
  memcpy(seven_segment->digit_pins, digit_pins, sizeof(uint16_t) * 4);
  memcpy(seven_segment->bcd_pins, bcd_pins, sizeof(uint16_t) * 4);
  memcpy(seven_segment->digit_ports, digit_ports, sizeof(GPIO_TypeDef*) * 4);
  memcpy(seven_segment->bcd_ports, bcd_ports, sizeof(GPIO_TypeDef*) * 4);
  for (int i = 0; i < SEVEN_SEGMENT_DIGIT_COUNT; i++) {
    seven_segment->digits[i] = SEVEN_SEGMENT_DISABLED_DIGIT;
  }
  for (int i = 0; i < SEVEN_SEGMENT_DIGIT_COUNT; i++) {
    seven_segment->decimal_points[i] = false;
  }
  osThreadDef(sevenSegmentThread, seven_segment_thread, osPriorityNormal, 0,
              128 * 2);
  osThreadCreate(osThread(sevenSegmentThread), seven_segment);
  return seven_segment;
}

void seven_segment_activate_decimal_point(SevenSegment* seven_segment) {
  HAL_GPIO_WritePin(seven_segment->dp_port, seven_segment->dp_pin, 0);
}

void seven_segment_deactivate_decimal_point(SevenSegment* seven_segment) {
  HAL_GPIO_WritePin(seven_segment->dp_port, seven_segment->dp_pin, 1);
}

void seven_segment_activate_digit(SevenSegment* seven_segment,
                                  uint8_t digit_id) {
  HAL_GPIO_WritePin(seven_segment->digit_ports[digit_id],
                    seven_segment->digit_pins[digit_id], 1);
}

void seven_segment_deactivate_digit(SevenSegment* seven_segment,
                                    uint8_t digit_id) {
  HAL_GPIO_WritePin(seven_segment->digit_ports[digit_id],
                    seven_segment->digit_pins[digit_id], 0);
}

void seven_segment_deactivate_all_digits(SevenSegment* seven_segment) {
  for (int i = 0; i < SEVEN_SEGMENT_DIGIT_COUNT; i++) {
    seven_segment_deactivate_digit(seven_segment, i);
  }
}

void seven_segment_set_active_digit(SevenSegment* seven_segment,
                                    uint8_t digit_id) {
  seven_segment_deactivate_all_digits(seven_segment);
  seven_segment_activate_digit(seven_segment, digit_id);
}

void seven_segment_show_digit(SevenSegment* seven_segment,
                              uint8_t digit,
                              bool decimal_point) {
  for (int i = 0; i < SEVEN_SEGMENT_DIGIT_COUNT; i++) {
    HAL_GPIO_WritePin(seven_segment->bcd_ports[i], seven_segment->bcd_pins[i],
                      (digit >> i) & 1);
  }
  if (decimal_point)
    seven_segment_activate_decimal_point(seven_segment);
  else
    seven_segment_deactivate_decimal_point(seven_segment);
}

void split_number(uint16_t number,
                  uint8_t* left_number,
                  uint8_t* right_number) {
  *left_number = number / 100;
  *right_number = number % 100;
}

void seven_segment_loop(SevenSegment* seven_segment) {
  static uint8_t digit_index = 0;
  seven_segment_set_active_digit(seven_segment, digit_index);
  seven_segment_show_digit(seven_segment, seven_segment->digits[digit_index],
                           seven_segment->decimal_points[digit_index]);
  digit_index++;
  digit_index %= SEVEN_SEGMENT_DIGIT_COUNT;
}

void seven_segment_set_digit(SevenSegment* seven_segment,
                             uint8_t digit_value,
                             uint8_t digit_index) {
  if (digit_index >= SEVEN_SEGMENT_DIGIT_COUNT || digit_value > 9)
    return;
  seven_segment->digits[digit_index] = digit_value;
}

void seven_segment_disable_digit(SevenSegment* seven_segment,
                                 uint8_t digit_index) {
  if (digit_index >= SEVEN_SEGMENT_DIGIT_COUNT)
    return;
  seven_segment->digits[digit_index] = SEVEN_SEGMENT_DISABLED_DIGIT;
}

void seven_segment_disable_all_digits(SevenSegment* seven_segment) {
  for (uint8_t i = 0; i < 4; i++) {
    seven_segment_disable_digit(seven_segment, i);
  }
}