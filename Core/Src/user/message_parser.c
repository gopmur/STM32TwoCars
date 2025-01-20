#include <stdio.h>
#include <string.h>

#include "message_parser.h"

ParsedMessage parse_message(char* message) {
  int32_t value;
  uint32_t chars_read;
  uint8_t values_matched;
  values_matched = sscanf(message, "set_hours(%d)%n", &value, &chars_read);
  if (values_matched == 1 && message[chars_read] == '\0') {
    ParsedMessage parsed_message = {
        .type = MESSAGE_TYPE_SET_HOURS,
        .value = value,
    };
    return parsed_message;
  }
  values_matched = sscanf(message, "set_minutes(%d)%n", &value, &chars_read);
  if (values_matched == 1 && message[chars_read] == '\0') {
    ParsedMessage parsed_message = {
        .type = MESSAGE_TYPE_SET_MINUTES,
        .value = value,
    };
    return parsed_message;
  }
  values_matched = sscanf(message, "set_seconds(%d)%n", &value, &chars_read);
  if (values_matched == 1 && message[chars_read] == '\0') {
    ParsedMessage parsed_message = {
        .type = MESSAGE_TYPE_SET_SECONDS,
        .value = value,
    };
    return parsed_message;
  }
  if (strcmp(message, "start game") == 0) {
    ParsedMessage parse_message = {
      .type = MESSAGE_TYPE_START_GAME,
    };
    return parse_message;
  }
  if (strcmp(message, "restart") == 0) {
    ParsedMessage parse_message = {
      .type = MESSAGE_TYPE_RESTART_GAME,
    };
    return parse_message;
  }
  if (strcmp(message, "menu") == 0) {
    ParsedMessage parse_message = {
      .type = MESSAGE_TYPE_GOTO_MAIN_MENU,
    };
    return parse_message;
  }
  ParsedMessage parsed_message = {
      .type = MESSAGE_TYPE_UNDEFINED,
  };
  return parsed_message;
}