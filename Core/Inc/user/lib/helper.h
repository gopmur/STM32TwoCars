#pragma once

#include "main.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define EXEC_EVERY_MS(code_block, delay_ms) \
  {                                         \
    static uint32_t current_time;           \
    static uint32_t prev_time = 0;          \
    static uint32_t passed_time = 0;        \
                                            \
    current_time = HAL_GetTick();           \
    passed_time = current_time - prev_time; \
                                            \
    if (passed_time > delay_ms) {           \
      prev_time = current_time;             \
      code_block                            \
    }                                       \
  }

#define EXEC_ONCE(code_block)     \
  {                               \
    static bool executed = false; \
    if (!executed) {              \
      code_block executed = true; \
    }                             \
  }

#define ON_VAR_CHANGE(var, code_block) \
  {                                    \
    static typeof(var) prev_var;       \
    if (prev_var != var) {             \
      code_block prev_var = var;       \
    }                                  \
  }

void start_timer_by_period_ms_it(TIM_HandleTypeDef* timer,
                                 uint32_t period_ms,
                                 uint32_t clock_frequency);
uint8_t get_nth_digit(uint32_t number, uint32_t digit_index);
uint32_t get_number_len(uint32_t number);
int32_t uipow(uint32_t base, uint32_t exp);
GPIO_TypeDef* get_exti_button_port_by_pin(uint16_t pin);
void fill_buffer(uint8_t* buffer, uint32_t buffer_size, uint8_t value);
uint16_t concat_numbers(uint32_t left_number,
                        uint32_t right_number,
                        uint8_t right_number_length);
uint8_t get_pin_number(uint16_t pin);
uint32_t flatten_2d_index(uint32_t i, uint32_t j, uint32_t width);
uint32_t time_to_seconds(RTC_TimeTypeDef time);
uint32_t get_time_difference_in_seconds(RTC_TimeTypeDef future_time,
                                        RTC_TimeTypeDef past_time);