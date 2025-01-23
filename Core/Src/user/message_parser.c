#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "message_parser.h"

ParsedMessage parse_message(char* message) {
  ParsedMessage parsed_message;

  if (strncmp(message, "set_seconds ", 12) == 0) {
    parsed_message.type = MESSAGE_TYPE_SET_SECONDS;
    sscanf(message, "set_seconds %d", &parsed_message.value.i32);
  } else if (strncmp(message, "set_minutes ", 12) == 0) {
    parsed_message.type = MESSAGE_TYPE_SET_MINUTES;
    sscanf(message, "set_minutes %d", &parsed_message.value.i32);
  } else if (strncmp(message, "set_hours ", 10) == 0) {
    parsed_message.type = MESSAGE_TYPE_SET_HOURS;
    sscanf(message, "set_hours %d", &parsed_message.value.i32);
  } else if (strncmp(message, "set_name ", 9) == 0) {
    parsed_message.type = MESSAGE_TYPE_SET_NAME;
    parsed_message.value.char_p = malloc(sizeof(char) * (strlen(message) - 8));
    sscanf(message, "set_name %s", parsed_message.value.char_p);
  } else {
    parsed_message.type = MESSAGE_TYPE_UNDEFINED;
  }

  return parsed_message;
}