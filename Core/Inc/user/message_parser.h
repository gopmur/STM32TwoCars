#pragma once

#include "main.h"
#include "rtc.h"

typedef enum _MessageType {
  MESSAGE_TYPE_SET_HOURS,
  MESSAGE_TYPE_SET_MINUTES,
  MESSAGE_TYPE_SET_SECONDS,
  MESSAGE_TYPE_SET_NAME,
  MESSAGE_TYPE_TIME_SYN,
  MESSAGE_TYPE_UNDEFINED,
} MessageType;

typedef union _MessageValue {
  int32_t i32;
  char* char_p;
  RtcTimeSynData time_syn_data;
} MessageValue;

typedef struct _ParsedMessage {
  MessageType type;
  MessageValue value;
} ParsedMessage;

ParsedMessage parse_message(char* message);