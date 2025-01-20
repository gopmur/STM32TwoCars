#include "gout.h"

Gout new_gout(GPIO_TypeDef* port, uint16_t pin) {
  Gout gout = {
    .pin = pin,
    .port = port,
  };
  return gout;
}

void gout_reset(Gout *gout) {
  HAL_GPIO_WritePin(gout->port, gout->pin, 0);
}

void gout_set(Gout *gout) {
  HAL_GPIO_WritePin(gout->port, gout->pin, 1);
}
void gout_toggle(Gout *gout) {
  HAL_GPIO_TogglePin(gout->port, gout->pin);
}