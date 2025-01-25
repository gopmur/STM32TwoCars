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
  } else if (strncmp(message, "time_syn ", 9) == 0) {
    parsed_message.type = MESSAGE_TYPE_TIME_SYN;
    sscanf(message, "time_syn %d/%d/%d,%d:%d:%d",
           &parsed_message.value.time_syn_data.year,
           &parsed_message.value.time_syn_data.month,
           &parsed_message.value.time_syn_data.day,
           &parsed_message.value.time_syn_data.hour,
           &parsed_message.value.time_syn_data.minute,
           &parsed_message.value.time_syn_data.second);
  } else if (strncmp(message, "set_difficulty ", 15) == 0) {
    parsed_message.type = MESSAGE_TYPE_SET_DIFFICULTY;
    sscanf(message, "set_difficulty %d", &parsed_message.value.i32);
  } else if (strncmp(message, "set_starting_health ", 20) == 0) {
    parsed_message.type = MESSAGE_TYPE_SET_STARTING_HEALTH;
    sscanf(message, "set_starting_health %d", &parsed_message.value.i32);
  } else if (strncmp(message, "set_speed ", 10) == 0) {
    parsed_message.type = MESSAGE_TYPE_SET_SPEED;
    sscanf(message, "set_speed %d", &parsed_message.value.i32);
  } else if (strncmp(message, "set_music ", 10) == 0) {
    parsed_message.type = MESSAGE_TYPE_SET_MUSIC;
    sscanf(message, "set_music %d", &parsed_message.value.i32);
  } else {
    parsed_message.type = MESSAGE_TYPE_UNDEFINED;
  }

  return parsed_message;
}