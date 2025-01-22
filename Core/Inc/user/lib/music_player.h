#pragma once

#include <stdbool.h>

#include "cmsis_os.h"

#include "main.h"
#include "pwm.h"

typedef enum _MusicPlayerCommandType {
  MusicPlayerCommandPlay,
  MusicPlayerCommandStop,
  Resume,
} MusicPlayerCommandType;

typedef struct _MusicPlayerCommandPlayData {
  int16_t* melody;
  float volume;
  bool replay;
} MusicPlayerCommandPlayData;

typedef struct _MusicPlayerCommandStopData {
} MusicPlayerCommandStopData;
typedef struct _MusicPlayerCommandResumeData {
} MusicPlayerCommandResumeData;
typedef union _MusicPlayerCommandData {
  MusicPlayerCommandPlayData play;
  MusicPlayerCommandStopData stop;
  MusicPlayerCommandResumeData resume;
} MusicPlayerCommandData;

typedef struct _MusicPlayerCommand {
  MusicPlayerCommandType type;
  MusicPlayerCommandData data;
} MusicPlayerCommand;

typedef struct _MusicPlayer {
  Pwm* device;
  volatile uint32_t current_note_index;
  volatile int16_t* current_melody;
  volatile uint16_t tempo;
  volatile bool is_active;
  volatile float volume;
  volatile bool replay;
  QueueHandle_t command_queue;
} MusicPlayer;

MusicPlayer *new_music_player(Pwm* device);
void music_player_play(MusicPlayer* music_player,
                       int16_t* melody,
                       float volume,
                       bool replay);
void music_player_stop(MusicPlayer* music_player);