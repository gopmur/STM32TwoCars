#include <math.h>

#include "pwm.h"
#include "pwm.helper.h"
#include "helper.h"

Pwm new_pwm(TIM_HandleTypeDef* timer,
            uint32_t channel,
            uint32_t timer_base_frequency) {
  Pwm buzzer = {
      .timer = timer,
      .channel = channel,
      .timer_base_frequency = timer_base_frequency,
      .current_duty_cycle = 0,
      .current_frequency = 0,
  };
  pwm_set_resolution(&buzzer, 1024);
  pwm_set_duty_cycle(&buzzer, 0);
  return buzzer;
}

void pwm_set_resolution(Pwm* pwm, uint32_t resolution) {
  if (resolution == 0)
    return;
  uint32_t timer_base_frequency = pwm->timer_base_frequency;
  uint32_t psc = pwm->timer->Instance->PSC;
  pwm->current_frequency = timer_base_frequency / resolution / psc;
  pwm->timer->Instance->ARR = resolution - 1;
  pwm->timer->Instance->EGR = TIM_EGR_UG;
}

void pwm_set_frequency(Pwm* pwm, uint32_t frequency) {
  TIM_HandleTypeDef* timer = pwm->timer;
  uint32_t timer_base_frequency = pwm->timer_base_frequency;
  uint32_t timer_pre_scaler = timer->Instance->PSC;

  timer->Instance->ARR =
      timer_base_frequency / ((timer_pre_scaler + 1) * frequency);

  pwm->current_frequency = frequency;
  pwm_set_duty_cycle(pwm, pwm->current_duty_cycle);

  // set timer update generation signal
  timer->Instance->EGR = TIM_EGR_UG;
}

void pwm_led_set_intensity(PwmLed* pwm_led, float intensity) {
  pwm_set_duty_cycle(pwm_led, pow(intensity / 100, 2) * 100);
}

void pwm_set_duty_cycle(Pwm* pwm, float duty_cycle) {
  if (duty_cycle > 100 || duty_cycle < 0)
    return;

  TIM_HandleTypeDef* timer = pwm->timer;
  volatile uint32_t* ccr = get_timer_ccr(timer, pwm->channel);
  if (ccr == NULL)
    return;

  pwm->current_duty_cycle = duty_cycle;
  float arr_modifier = duty_cycle / 100;
  *ccr = timer->Instance->ARR * arr_modifier;

  // set timer update generation signal
  timer->Instance->EGR = TIM_EGR_UG;
}

void buzzer_set_volume(Buzzer* buzzer, float volume) {
  pwm_set_duty_cycle(buzzer, (powf(volume, 3)) / powf(100, 3));
}

void pwm_start(Pwm* pwm) {
  if (pwm->is_active)
    return;
  HAL_TIM_PWM_Start(pwm->timer, pwm->channel);
  pwm->is_active = true;
}

void pwm_stop(Pwm* pwm) {
  if (!pwm->is_active)
    return;
  HAL_TIM_PWM_Stop(pwm->timer, pwm->channel);
  pwm->is_active = false;
}
