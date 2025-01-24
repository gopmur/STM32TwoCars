#pragma once

#include "cmsis_os.h"

#include "connectivity.uart.h"

#define APB2_CLOCK_FREQUENCY 48000000
#define LCD_WIDTH 20
#define LCD_HEIGHT 4

extern Uart* uart;

void setup();