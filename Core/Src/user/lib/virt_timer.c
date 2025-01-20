#include "virt_timer.h"

VirtTimer new_virt_timer(uint32_t frequency_divider,
                         void (*period_elapsed_call_back)(void)) {
  VirtTimer virt_timer = {
      .counter = 0,
      .counter_cap = frequency_divider == 0 ? 1 : frequency_divider - 1,
      .period_elapsed_call_back = period_elapsed_call_back,
      .enabled = false,
  };
  return virt_timer;
}

void virt_timer_set_frequency_divider(VirtTimer* virt_timer,
                                 uint32_t frequency_divider) {
  if (frequency_divider != 0) {
    virt_timer->counter_cap = frequency_divider - 1;
  } else {
    virt_timer->counter_cap = 0;
  }
}

void virt_timer_tick_it(VirtTimer* virt_timer) {
  if (!virt_timer->enabled)
    return;
  if (virt_timer->counter == virt_timer->counter_cap) {
    if (virt_timer->period_elapsed_call_back)
      virt_timer->period_elapsed_call_back();
    virt_timer->counter = 0;
  } else {
    virt_timer->counter++;
  }
}

void virt_timer_reset_it(VirtTimer* virt_timer) {
  virt_timer->counter = 0;
}
void virt_timer_stop_it(VirtTimer* virt_timer) {
  virt_timer->enabled = false;
}
void virt_timer_start_it(VirtTimer* virt_timer) {
  virt_timer->enabled = true;
}

void virt_timer_set_duration_ms(VirtTimer* virt_timer,
                                uint32_t duration_ms,
                                uint32_t base_frequency) {
  uint32_t frequency_divider = base_frequency * duration_ms / 1000;
  virt_timer_set_frequency_divider(virt_timer, frequency_divider);
}