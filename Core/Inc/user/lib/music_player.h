#pragma once

#include <stdbool.h>

#include "main.h"
#include "pwm.h"

typedef struct _MusicPlayer {
  Pwm* device;
  TIM_HandleTypeDef* timer;
  volatile uint32_t current_note_index;
  volatile int16_t* current_melody;
  volatile uint16_t tempo;
  volatile bool is_active;
  volatile float volume;
  void (*timer_period_elapsed_callback)(struct _MusicPlayer*);
  void (*on_music_ended)(void);
  volatile bool replay;
} MusicPlayer;

MusicPlayer new_music_player(Pwm* device,
                             TIM_HandleTypeDef* timer,
                             void (*on_music_ended)(void));
void music_player_stop_it(MusicPlayer* music_player);
void music_player_play_melody_it(MusicPlayer* music_player,
                                 int16_t* melody,
                                 float volume,
                                 bool replay);
void music_player_play_next_note(MusicPlayer* music_player);
void music_player_resume(MusicPlayer* music_player, float volume, bool replay);