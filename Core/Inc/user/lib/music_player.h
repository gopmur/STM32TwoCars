#pragma once

#include <stdbool.h>

#include "cmsis_os.h"

#include "main.h"
#include "pwm.h"

typedef enum _MusicPlayerCommandType {
  MUSIC_PLAYER_COMMAND_PLAY,
  MUSIC_PLAYER_COMMAND_STOP,
  MUSIC_PLAYER_COMMAND_PUSH,
  MUSIC_PLAYER_COMMAND_SET_TEMPO,
  MUSIC_PLAYER_COMMAND_RESUME,
  MUSIC_PLAYER_COMMAND_NONE,
} MusicPlayerCommandType;

typedef struct _MusicPlayerCommandSetTempoData {
  int16_t tempo;
} MusicPlayerCommandSetTempoData;

typedef struct _MusicPlayerCommandPlayData {
  int16_t* melody;
  float volume;
  bool replay;
} MusicPlayerCommandPlayData;

typedef struct _MusicPlayerCommandPush {
  int16_t* melody;
} MusicPlayerCommandPush;
typedef struct _MusicPlayerCommandStopData {
} MusicPlayerCommandStopData;
typedef struct _MusicPlayerCommandResumeData {
} MusicPlayerCommandResumeData;
typedef union _MusicPlayerCommandData {
  MusicPlayerCommandPlayData play;
  MusicPlayerCommandStopData stop;
  MusicPlayerCommandResumeData resume;
  MusicPlayerCommandPlayData push;
  MusicPlayerCommandSetTempoData set_tempo;
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
  int16_t* stored_melody;
  uint32_t stored_note_index;
  uint16_t stored_tempo;
  bool melody_pushed;
  QueueHandle_t command_queue;
} MusicPlayer;

MusicPlayer* new_music_player(Pwm* device);
void music_player_play(MusicPlayer* music_player,
                       int16_t* melody,
                       float volume,
                       bool replay);
void music_player_stop(MusicPlayer* music_player);
void music_player_set_tempo(MusicPlayer* music_player, int16_t tempo);
void music_player_push(MusicPlayer* music_player, int16_t* melody);