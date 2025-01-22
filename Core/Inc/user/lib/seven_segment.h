#pragma once

#include "main.h"

#include <stdbool.h>

#define SEVEN_SEGMENT_DIGIT_COUNT 4
#define SEVEN_SEGMENT_DISABLED_DIGIT 15
#define SEVEN_SEGMENT_UPDATE_PERIOD_MS 5

typedef struct _SevenSegment {
  uint16_t digit_pins[SEVEN_SEGMENT_DIGIT_COUNT];
  uint16_t dp_pin;
  uint16_t bcd_pins[SEVEN_SEGMENT_DIGIT_COUNT];
  GPIO_TypeDef* digit_ports[SEVEN_SEGMENT_DIGIT_COUNT];
  GPIO_TypeDef* dp_port;
  GPIO_TypeDef* bcd_ports[SEVEN_SEGMENT_DIGIT_COUNT];
  volatile uint8_t digits[SEVEN_SEGMENT_DIGIT_COUNT];
  volatile bool decimal_points[SEVEN_SEGMENT_DIGIT_COUNT];
} SevenSegment;

SevenSegment* new_seven_segment(uint16_t digit_pins[4],
                                uint16_t dp_pin,
                                uint16_t bcd_pins[4],
                                GPIO_TypeDef* digit_ports[4],
                                GPIO_TypeDef* dp_port,
                                GPIO_TypeDef* bcd_ports[4]);

void seven_segment_loop(SevenSegment* seven_segment);
void seven_segment_set_digit(SevenSegment* seven_segment,
                             uint8_t digit_value,
                             uint8_t digit_index);
void seven_segment_disable_digit(SevenSegment* seven_segment,
                                 uint8_t digit_index);