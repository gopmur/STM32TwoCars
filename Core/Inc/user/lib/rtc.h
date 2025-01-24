#pragma once

#include "main.h"

typedef struct _RtcTimeSynData {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} RtcTimeSynData;


void rtc_set_hours(RTC_HandleTypeDef* rtc, uint8_t hours);
void rtc_set_minutes(RTC_HandleTypeDef* rtc, uint8_t minutes);
void rtc_set_seconds(RTC_HandleTypeDef* rtc, uint8_t seconds);
void rtc_syn(RTC_HandleTypeDef* rtc, RtcTimeSynData syn_time);
RTC_TimeTypeDef rtc_get_time(RTC_HandleTypeDef* rtc);
RTC_DateTypeDef rtc_get_date(RTC_HandleTypeDef* rtc);