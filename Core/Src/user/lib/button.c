#include <malloc.h>
#include <memory.h>

#include "cmsis_os.h"

#include "button.h"
#include "connectivity.uart.h"
#include "helper.h"
#include "virt_timer.h"

extern Uart* uart;

Button* button_pool[PHYSICAL_BUTTON_COUNT] = {NULL};
ButtonInterruptHandler button_interrupt_handler;

osThreadId button_interrupt_thread_handle;
uint16_t interrupted_pin;

void button_interrupt_thread(void* args) {
  while (true) {
    vTaskSuspend(NULL);
    uint8_t pin_number = get_pin_number(interrupted_pin);
    if (button_pool[pin_number] == NULL)
      continue;
    Button* button = button_pool[pin_number];
    osDelay(DEBOUNCE_DELAY_MS);
    bool is_high = HAL_GPIO_ReadPin(button->port, button->pin);
    if (is_high) {
      button->on_press(button->on_press_arg);
    }
  }
}

Button* new_button(GPIO_TypeDef* port,
                   uint16_t pin,
                   void (*on_press)(void*),
                   void* on_press_arg,
                   uint32_t on_press_arg_size) {
  EXEC_ONCE({
    osThreadDef(button_interrupt, button_interrupt_thread, osPriorityNormal, 0,
                128);
    button_interrupt_thread_handle =
        osThreadCreate(osThread(button_interrupt), NULL);
    if (button_interrupt_thread_handle == NULL) {
      uart_sendln(uart,
                  "log: button_interrupt thread did not start successfully");
    } else {
      uart_sendln(uart, "log: button_interrupt thread started successfully");
    }
  })

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