#pragma once

#include <stdbool.h>

#include "cmsis_os.h"

#include "main.h"
#include "virt_timer.h"

#define DEBOUNCE_DELAY_MS 50 
#define PHYSICAL_BUTTON_COUNT 16

extern osThreadId button_interrupt_thread_handle;
extern uint16_t interrupted_pin;

typedef struct _Button {
  GPIO_TypeDef* port;
  uint16_t pin;
  void (*on_press)(void*);
  void* on_press_arg;
} Button;

typedef struct _ButtonInterruptHandler {
  volatile uint16_t interrupted_pin;
  volatile bool busy;
} ButtonInterruptHandler;

Button* new_button(GPIO_TypeDef* port,
                   uint16_t pin,
                   void (*on_press)(void*),
                   void* on_press_arg,
                   uint32_t on_press_arg_size);
void delete_button(Button* button);