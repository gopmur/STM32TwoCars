#include <malloc.h>
#include <memory.h>
#include <stdbool.h>

#include "helper.h"
#include "key_pad.h"

#include "main.h"

void mask_button_interrupt(KeyPad* key_pad, uint8_t row) {
  EXTI->IMR &= ~key_pad->buttons[row]->pin;
}

void unmask_button_interrupt(KeyPad* key_pad, uint8_t row) {
  EXTI->IMR |= key_pad->buttons[row]->pin;
}

void mask_all_button_interrupts(KeyPad* key_pad) {
  for (uint8_t row = 0; row < key_pad->height; row++) {
    mask_button_interrupt(key_pad, row);
  }
}

void unmask_all_button_interrupts(KeyPad* key_pad) {
  for (uint8_t row = 0; row < key_pad->height; row++) {
    unmask_button_interrupt(key_pad, row);
  }
}

void on_row_press(OnRowPressArg* arg) {
  ;
  KeyPad* key_pad = arg->key_pad;
  uint32_t row = arg->row;
  mask_all_button_interrupts(key_pad);
  for (uint8_t col = 0; col < key_pad->width; col++) {
    HAL_GPIO_WritePin(key_pad->col_enable_ports[col],
                      key_pad->col_enable_pins[col], GPIO_PIN_RESET);
  }
  uint8_t col;
  for (col = 0; col < key_pad->width; col++) {
    HAL_GPIO_WritePin(key_pad->col_enable_ports[col],
                      key_pad->col_enable_pins[col], GPIO_PIN_SET);
    bool is_high = HAL_GPIO_ReadPin(key_pad->buttons[row]->port,
                                    key_pad->buttons[row]->pin);
    if (is_high) {
      key_pad->on_press(row, col);
      break;
    }
  }
  for (; col < key_pad->width; col++) {
    HAL_GPIO_WritePin(key_pad->col_enable_ports[col],
                      key_pad->col_enable_pins[col], GPIO_PIN_SET);
  }
  unmask_all_button_interrupts(key_pad);
}

KeyPad* new_key_pad(uint8_t width,
                    uint8_t height,
                    GPIO_TypeDef** col_enable_ports,
                    GPIO_TypeDef** row_exti_ports,
                    uint16_t* col_enable_pins,
                    uint16_t* row_exti_pins,
                    void (*on_press)(uint8_t row, uint8_t col)) {
  for (uint8_t col = 0; col < width; col++) {
    HAL_GPIO_WritePin(col_enable_ports[col], col_enable_pins[col],
                      GPIO_PIN_SET);
  }
  KeyPad* key_pad = malloc(sizeof(KeyPad));
  key_pad->width = width;
  key_pad->height = height;
  key_pad->buttons = malloc(height * sizeof(Button*));
  key_pad->col_enable_pins = malloc(width * sizeof(uint16_t));
  key_pad->col_enable_ports = malloc(width * sizeof(GPIO_TypeDef*));
  for (uint32_t row = 0; row < height; row++) {
    OnRowPressArg on_row_press_arg = {
        .key_pad = key_pad,
        .row = row,
    };
    key_pad->buttons[row] = new_button(
        row_exti_ports[row], row_exti_pins[row], (void (*)(void*))on_row_press,
        &on_row_press_arg, sizeof(OnRowPressArg));
    ;
  }
  memcpy(key_pad->col_enable_pins, col_enable_pins, width * sizeof(uint16_t));
  memcpy(key_pad->col_enable_ports, col_enable_ports,
         width * sizeof(GPIO_TypeDef*));
  key_pad->on_press = on_press;
  return key_pad;
}

void delete_key_pad(KeyPad* key_pad) {
  if (key_pad->buttons != NULL) {
    for (uint32_t row = 0; row < key_pad->height; row++) {
      delete_button(key_pad->buttons[row]);
    }
    free(key_pad->buttons);
  }
  if (key_pad->col_enable_pins != NULL)
    free(key_pad->col_enable_pins);
  if (key_pad->col_enable_ports != NULL)
    free(key_pad->col_enable_ports);
  free(key_pad);
}