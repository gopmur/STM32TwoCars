#pragma once

#include "main.h"

typedef enum _MessageType {
  MESSAGE_TYPE_SET_HOURS,
  MESSAGE_TYPE_SET_MINUTES,
  MESSAGE_TYPE_SET_SECONDS,
  MESSAGE_TYPE_START_GAME,
  MESSAGE_TYPE_RESTART_GAME,
  MESSAGE_TYPE_GOTO_MAIN_MENU,
  MESSAGE_TYPE_UNDEFINED,
} MessageType;

typedef struct _ParsedMessage {
  MessageType type;
  int32_t value;
} ParsedMessage;

ParsedMessage parse_message(char* message);