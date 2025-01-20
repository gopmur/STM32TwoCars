#pragma once

#include <stdbool.h>

#include "main.h"

typedef struct _VirtTimer {
  volatile uint32_t counter;
  volatile uint32_t counter_cap;
  void (*period_elapsed_call_back)(void);
  volatile bool enabled;
} VirtTimer;

VirtTimer new_virt_timer(uint32_t frequency_divider,
                         void (*period_elapsed_call_back)(void));
void virt_timer_tick_it(VirtTimer* virt_timer);
void virt_timer_reset_it(VirtTimer* virt_timer);
void virt_timer_stop_it(VirtTimer* virt_timer);
void virt_timer_start_it(VirtTimer* virt_timer);
void virt_timer_set_duration_ms(VirtTimer* virt_timer, uint32_t duration_ms, uint32_t base_frequency);