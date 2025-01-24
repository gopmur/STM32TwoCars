#include "rtc.h"
#include "connectivity.uart.h"
#include "entry.h"

void rtc_set_hours(RTC_HandleTypeDef* rtc, uint8_t hours) {
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(rtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(rtc, &date, RTC_FORMAT_BIN);
  time.Hours = hours;
  HAL_RTC_SetTime(rtc, &time, RTC_FORMAT_BIN);
}

void rtc_set_minutes(RTC_HandleTypeDef* rtc, uint8_t minutes) {
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(rtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(rtc, &date, RTC_FORMAT_BIN);
  time.Minutes = minutes;
  HAL_RTC_SetTime(rtc, &time, RTC_FORMAT_BIN);
};

void rtc_set_seconds(RTC_HandleTypeDef* rtc, uint8_t seconds) {
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(rtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(rtc, &date, RTC_FORMAT_BIN);
  time.Seconds = seconds;
  HAL_RTC_SetTime(rtc, &time, RTC_FORMAT_BIN);
}

void rtc_syn(RTC_HandleTypeDef* rtc, RtcTimeSynData syn_time) {
  RTC_TimeTypeDef time;
  time.Hours = syn_time.hour;
  time.Minutes = syn_time.minute;
  time.Seconds = syn_time.second;
  RTC_DateTypeDef date;
  date.Year = syn_time.year;
  date.Month = syn_time.month;
  date.Date = syn_time.day;
  date.WeekDay = 0;
  HAL_RTC_SetTime(rtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_SetDate(rtc, &date, RTC_FORMAT_BIN);
}

RTC_TimeTypeDef rtc_get_time(RTC_HandleTypeDef* rtc) {
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(rtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(rtc, &date, RTC_FORMAT_BIN);
  return time;
}

RTC_DateTypeDef rtc_get_date(RTC_HandleTypeDef* rtc) {
  RTC_TimeTypeDef _time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(rtc, &_time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(rtc, &date, RTC_FORMAT_BIN);
  return date;
}