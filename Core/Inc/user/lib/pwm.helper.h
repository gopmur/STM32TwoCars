#pragma once

#include "main.h"

volatile uint32_t* get_timer_ccr(TIM_HandleTypeDef* timer, uint32_t channel);