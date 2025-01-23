#pragma once

#include "main.h"

typedef enum _MessageType {
  MESSAGE_TYPE_SET_HOURS,
  MESSAGE_TYPE_SET_MINUTES,
  MESSAGE_TYPE_SET_SECONDS,
  MESSAGE_TYPE_SET_NAME,
  MESSAGE_TYPE_UNDEFINED,
} MessageType;

typedef union _MessageValue {
  int32_t i32;
  char* char_p;
} MessageValue;

typedef struct _ParsedMessage {
  MessageType type;
  MessageValue value;
} ParsedMessage;

ParsedMessage parse_message(char* message);