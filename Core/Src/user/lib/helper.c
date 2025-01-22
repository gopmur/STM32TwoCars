#include "helper.h"
#include "main.h"

void start_timer_by_period_ms_it(TIM_HandleTypeDef* timer,
                                 uint32_t period_ms,
                                 uint32_t clock_frequency) {
  float new_arr = ((float)clock_frequency * (float)period_ms) /
                  ((float)timer->Instance->PSC * 1000.0);
  timer->Instance->CNT = 0;
  timer->Instance->ARR = (uint32_t)new_arr;
  HAL_TIM_Base_Start_IT(timer);
}

uint8_t get_nth_digit(uint32_t number, uint32_t digit_index) {
  for (uint32_t i = 0; i < digit_index; i++) {
    number /= 10;
    if (number == 0)
      break;
  }
  return number % 10;
}

int32_t uipow(uint32_t base, uint32_t exp) {
  if (base == 0 && exp == 0)
    return -1;
  if (base == 1 || base == 0)
    return base;
  uint32_t result = 1;
  for (int i = 0; i < exp - 1; i++) {
    result *= base;
  }
  return base;
}

uint32_t get_number_len(uint32_t number) {
  if (number == 0)
    return 1;
  uint32_t len = 0;
  while (number) {
    len++;
    number /= 10;
  }
  return len;
}

void fill_buffer(uint8_t* buffer, uint32_t buffer_size, uint8_t value) {
  for (int i = 0; i < buffer_size; i++) {
    buffer[i] = value;
  }
}

uint16_t concat_numbers(uint32_t left_number,
                        uint32_t right_number,
                        uint8_t right_number_length) {
  return left_number * uipow(10, right_number_length) + right_number;
}

uint8_t get_pin_number(uint16_t pin) {
  for (uint8_t i = 0; i < 15; i++) {
    if ((pin >> i) & 1)
      return i;
  }
}

uint32_t flatten_2d_index(uint32_t i, uint32_t j, uint32_t width) {
  return i * width + j;
}

uint32_t time_to_seconds(RTC_TimeTypeDef time) {
  return time.Seconds + time.Minutes * 60 + time.Hours * 3600;
}

uint32_t get_time_difference_in_seconds(RTC_TimeTypeDef future_time,
                                        RTC_TimeTypeDef past_time) {
  uint32_t future_time_in_seconds = time_to_seconds(future_time);
  uint32_t past_time_in_seconds = time_to_seconds(past_time);
  return future_time_in_seconds >= past_time_in_seconds
             ? future_time_in_seconds - past_time_in_seconds
             : 24 * 3600 + future_time_in_seconds - past_time_in_seconds;
}