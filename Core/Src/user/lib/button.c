#include <malloc.h>
#include <memory.h>

#include "button.h"
#include "helper.h"
#include "virt_timer.h"

Button* button_pool[PHYSICAL_BUTTON_COUNT] = {NULL};
ButtonInterruptHandler button_interrupt_handler;

void init_buttons(TIM_HandleTypeDef* debounce_timer) {
  button_interrupt_handler.busy = false;
  button_interrupt_handler.interrupted_pin = 0;
  button_interrupt_handler.debounce_timer = debounce_timer;
}

void handle_button_interrupt(uint16_t pin) {
  if (button_interrupt_handler.busy)
    return;
  uint8_t pin_number = get_pin_number(pin);
  if (button_pool[pin_number] == NULL)
    return;
  button_interrupt_handler.busy = true;
  button_interrupt_handler.interrupted_pin = pin;
  button_interrupt_handler.debounce_timer->Instance->CNT = 0;
  HAL_TIM_Base_Start_IT(button_interrupt_handler.debounce_timer);
}

void debounce_timer_callback() {
  HAL_TIM_Base_Stop_IT(button_interrupt_handler.debounce_timer);
  button_interrupt_handler.busy = false;
  uint8_t pin_number = get_pin_number(button_interrupt_handler.interrupted_pin);
  Button* button = button_pool[pin_number];
  if (button == NULL)
    return;
  bool is_high = HAL_GPIO_ReadPin(button->port, button->pin);
  if (!is_high)
    return;
  if (button->on_press)
    button->on_press(button->on_press_arg);
}

Button* new_button(GPIO_TypeDef* port,
                   uint16_t pin,
                   void (*on_press)(void*),
                   void* on_press_arg,
                   uint32_t on_press_arg_size) {
  uint8_t pin_number = get_pin_number(pin);

  if (pin_number > PHYSICAL_BUTTON_COUNT || button_pool[pin_number] != NULL)
    return NULL;
  Button* button = (Button*)malloc(sizeof(Button));
  button->port = port;
  button->pin = pin;
  button->on_press = on_press;
  button->on_press_arg = malloc(on_press_arg_size);
  memcpy(button->on_press_arg, on_press_arg, on_press_arg_size);
  button_pool[pin_number] = button;
  return button;
}

void delete_button(Button* button) {
  if (button_pool[button->pin] == NULL)
    return;
  if (button->on_press_arg != NULL) {
    free(button->on_press_arg);
  }
  button_pool[button->pin] = NULL;
  free(button);
}