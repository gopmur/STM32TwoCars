#pragma once

#include <stdbool.h>

#include "main.h"
#include "virt_timer.h"

#define PHYSICAL_BUTTON_COUNT 16
typedef struct _Button {
  GPIO_TypeDef* port;
  uint16_t pin;
  void (*on_press)(void*);
  void* on_press_arg;
} Button;

typedef struct _ButtonInterruptHandler {
  TIM_HandleTypeDef* debounce_timer;
  volatile uint16_t interrupted_pin;
  volatile bool busy;
} ButtonInterruptHandler;

extern ButtonInterruptHandler button_interrupt_handler;

void init_buttons(TIM_HandleTypeDef* debounce_timer);
Button* new_button(GPIO_TypeDef* port,
                   uint16_t pin,
                   void (*on_press)(void*),
                   void* on_press_arg,
                   uint32_t on_press_arg_size);
void delete_button(Button* button);
void handle_button_interrupt(uint16_t pin);
void debounce_timer_callback();