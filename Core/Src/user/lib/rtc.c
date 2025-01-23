#include "rtc.h"

void rtc_set_hours(RTC_HandleTypeDef* rtc, uint8_t hours) {
  RTC_TimeTypeDef time = rtc_get_time(rtc);
  time.Hours = hours;
  HAL_RTC_SetTime(rtc, &time, RTC_FORMAT_BIN);
}

void rtc_set_minutes(RTC_HandleTypeDef* rtc, uint8_t minutes) {
  RTC_TimeTypeDef time = rtc_get_time(rtc);
  time.Minutes = minutes;
  HAL_RTC_SetTime(rtc, &time, RTC_FORMAT_BIN);
};

void rtc_set_seconds(RTC_HandleTypeDef* rtc, uint8_t seconds) {
  RTC_TimeTypeDef time = rtc_get_time(rtc);
  time.Seconds = seconds;
  HAL_RTC_SetTime(rtc, &time, RTC_FORMAT_BIN);
}

RTC_TimeTypeDef rtc_get_time(RTC_HandleTypeDef* rtc) {
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(rtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(rtc, &date, RTC_FORMAT_BIN);
  return time;
}