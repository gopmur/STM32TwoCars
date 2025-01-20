#include "button.h"

#include "main.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  handle_button_interrupt(GPIO_Pin);
}