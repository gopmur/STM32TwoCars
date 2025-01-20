#include "button.h"

typedef struct _KeyPad {
  Button** buttons;
  GPIO_TypeDef** col_enable_ports;
  uint16_t* col_enable_pins;
  uint8_t width;
  uint8_t height;
  void (*on_press)(uint8_t row, uint8_t col);
} KeyPad;

typedef struct _OnRowPressArg {
  KeyPad* key_pad;
  uint32_t row;
} OnRowPressArg;

KeyPad* new_key_pad(uint8_t width,
                    uint8_t height,
                    GPIO_TypeDef** col_enable_ports,
                    GPIO_TypeDef** row_exti_ports,
                    uint16_t* col_enable_pins,
                    uint16_t* row_exti_pins,
                    void (*on_press)(uint8_t row, uint8_t col));
void delete_key_pad(KeyPad* key_pad);