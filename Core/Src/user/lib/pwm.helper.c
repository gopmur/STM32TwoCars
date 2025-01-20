#include "pwm.helper.h"

volatile uint32_t* get_timer_ccr(TIM_HandleTypeDef* timer, uint32_t channel) {
  switch (channel) {
    case TIM_CHANNEL_1:
      return &timer->Instance->CCR1;
    case TIM_CHANNEL_2:
      return &timer->Instance->CCR2;
    case TIM_CHANNEL_3:
      return &timer->Instance->CCR3;
    case TIM_CHANNEL_4:
      return &timer->Instance->CCR4;
    case TIM_CHANNEL_5:
      return &timer->Instance->CCR5;
    case TIM_CHANNEL_6:
      return &timer->Instance->CCR6;
    default:
      return NULL;
  }
}