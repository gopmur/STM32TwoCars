#pragma once

#include "main.h"

void rtc_set_hours(RTC_HandleTypeDef* rtc, uint8_t hours);
void rtc_set_minutes(RTC_HandleTypeDef* rtc, uint8_t minutes);
void rtc_set_seconds(RTC_HandleTypeDef* rtc, uint8_t seconds);
RTC_TimeTypeDef rtc_get_time(RTC_HandleTypeDef* rtc);