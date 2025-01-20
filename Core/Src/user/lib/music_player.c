#include "music_player.h"
#include "entry.h"
#include "helper.h"
#include "music_player.helper.h"
#include "notes.h"

void music_player_timer_period_elapsed(MusicPlayer* music_player) {
  HAL_TIM_Base_Stop_IT(music_player->timer);
  music_player_play_next_note(music_player);
}

MusicPlayer new_music_player(Pwm* device,
                             TIM_HandleTypeDef* timer,
                             void (*on_music_ended)(void)) {
  MusicPlayer music_player = {
      .device = device,
      .timer = timer,
      .current_melody = NULL,
      .current_note_index = 0,
      .is_active = false,
      .replay = false,
      .tempo = 0,
      .timer_period_elapsed_callback = music_player_timer_period_elapsed,
      .on_music_ended = on_music_ended,
  };
  return music_player;
}

void music_player_play_melody_it(MusicPlayer* music_player,
                                 int16_t* melody,
                                 float volume,
                                 bool replay) {
  if (music_player->is_active, melody == NULL || melody[0] == 0 ||
                                   melody[1] != melody[0] ||
                                   melody[2] == END_OF_MELODY)
    return;
  music_player_load_melody(music_player, melody);
  music_player_resume(music_player, volume, replay);
}

void music_player_resume(MusicPlayer* music_player, float volume, bool replay) {
  music_player->volume = volume;
  if (music_player->is_active || music_player->current_melody == NULL ||
      music_player->tempo == 0)
    return;
  music_player->is_active = true;
  music_player->replay = replay;
  int16_t note = music_player->current_melody[music_player->current_note_index];
  int16_t note_duration_divider =
      music_player->current_melody[music_player->current_note_index + 1];

  uint32_t note_duration_ms =
      get_note_duration_ms(note_duration_divider, music_player->tempo);
  music_player->current_note_index += 2;

  pwm_start(music_player->device);
  buzzer_set_volume(music_player->device, music_player->volume);
  pwm_set_frequency(music_player->device, note);
  start_timer_by_period_ms_it(music_player->timer, note_duration_ms,
                              APB2_CLOCK_FREQUENCY);
}

void music_player_stop_it(MusicPlayer* music_player) {
  music_player->is_active = false;
  pwm_stop(music_player->device);
}

void music_player_play_next_note(MusicPlayer* music_player) {
  if (!music_player->is_active || music_player->current_melody == NULL ||
      music_player->tempo == 0)
    return;
  if (music_player->current_melody[music_player->current_note_index] ==
          END_OF_MELODY &&
      music_player->replay) {
    music_player_stop_it(music_player);
    music_player_play_melody_it(music_player, music_player->current_melody,
                                music_player->volume, music_player->replay);
    return;
  }
  if (music_player->current_melody[music_player->current_note_index] ==
          END_OF_MELODY &&
      !music_player->replay) {
    music_player_stop_it(music_player);
    music_player->on_music_ended();
    return;
  }
  int16_t note = music_player->current_melody[music_player->current_note_index];
  int16_t note_duration_divider =
      music_player->current_melody[music_player->current_note_index + 1];

  uint32_t note_duration_ms =
      get_note_duration_ms(note_duration_divider, music_player->tempo);

  pwm_set_frequency(music_player->device, note);

  music_player->current_note_index += 2;

  start_timer_by_period_ms_it(music_player->timer, note_duration_ms,
                              APB2_CLOCK_FREQUENCY);
}
