#pragma once

#include "cmsis_os.h"

#define APB2_CLOCK_FREQUENCY 48000000

extern osThreadId button_interrupt_thread_handle;

void setup();