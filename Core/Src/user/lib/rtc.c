#include "rtc.h"

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