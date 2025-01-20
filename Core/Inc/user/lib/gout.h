#pragma once

#include "main.h"

typedef struct _Gout {
  GPIO_TypeDef *port;
  uint16_t pin;
} Gout;

Gout new_gout(GPIO_TypeDef* port, uint16_t pin);
void gout_reset(Gout *gout);
void gout_set(Gout *gout);
void gout_toggle(Gout *gout);