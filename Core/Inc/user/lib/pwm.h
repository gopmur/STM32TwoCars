#pragma once

#include <stdbool.h>

#include "main.h"

typedef struct _Pwm {
  TIM_HandleTypeDef* timer;
  uint32_t channel;
  volatile uint32_t timer_base_frequency;
  volatile uint32_t current_frequency;
  volatile float current_duty_cycle;
  volatile bool is_active;
} Pwm;


Pwm new_pwm(TIM_HandleTypeDef* timer,
                  uint32_t channel,
                  uint32_t timer_base_frequency);
void pwm_set_frequency(Pwm* pwm, uint32_t frequency);
void pwm_set_resolution(Pwm* pwm, uint32_t resolution);
void pwm_set_duty_cycle(Pwm* pwm, float duty_cycle);
void pwm_start(Pwm* pwm);
void pwm_stop(Pwm* pwm);

typedef Pwm Buzzer;
void buzzer_set_volume(Buzzer* buzzer, float volume);

typedef Pwm PwmLed;
void pwm_led_set_intensity(PwmLed *pwm_led, float intensity);