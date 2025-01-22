#include "main.h"
#include "cmsis_os.h"
#include "entry.h"

extern osThreadId button_interrupt_thread_handle;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (button_interrupt_thread_handle == NULL) {
    return;
  }
  osThreadResume(button_interrupt_thread_handle);
}