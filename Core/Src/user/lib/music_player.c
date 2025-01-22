#include "music_player.h"
#include "entry.h"
#include "helper.h"
#include "music_player.helper.h"
#include "notes.h"

void music_player_load_melody(MusicPlayer* music_player, int16_t* melody) {
  music_player->current_melody = melody;
  music_player->tempo = melody[0];
  music_player->current_note_index = 2;
}

bool music_player_play_next_note(MusicPlayer* music_player) {
  int16_t note = music_player->current_melody[music_player->current_note_index];
  if (note == END_OF_MELODY) {
    return false;
  }
  int16_t duration_divider =
      music_player->current_melody[music_player->current_note_index + 1];
  uint32_t duration_ms =
      get_note_duration_ms(duration_divider, music_player->tempo);
  music_player->current_note_index += 2;
  pwm_set_frequency(music_player->device, note);
  pwm_set_duty_cycle(music_player->device, 50);
  osDelay(duration_ms);
  return true;
}

void music_player_thread(MusicPlayer* music_player) {
  MusicPlayerCommand command;
  while (true) {
    // here we are in the waiting for command state
    xQueueReceive(music_player->command_queue, &command, portMAX_DELAY);
    if (command.type != Play) {
      continue;
    }
    music_player->is_active = true;
    music_player_load_melody(music_player, command.data.melody);
    music_player->volume = command.data.volume;
    pwm_start(music_player->device);
    while (music_player_play_next_note(music_player)) {
      xQueueReceive(music_player->command_queue, &command, 0);
      if (command.type == Stop) {
        music_player->is_active = false;
        break;
      }
    }
  }
}

MusicPlayer new_music_player(Pwm* device) {
  MusicPlayer music_player = {
      .device = device,
      .current_melody = NULL,
      .current_note_index = 0,
      .is_active = false,
      .replay = false,
      .tempo = 0,
      .command_queue = xQueueCreate(3, sizeof(MusicPlayerCommand)),
  };

  osThreadDef(music_player, music_player_thread, osPriorityNormal, 0, 128);
  osThreadCreate(osThread(music_player), &music_player);
  return music_player;
}

void music_player_play(MusicPlayer* music_player,
                       int16_t* melody,
                       float volume) {
  if (music_player->is_active || melody == NULL || melody[0] == 0 ||
      melody[1] != melody[0] || melody[2] == END_OF_MELODY)
    return;
  MusicPlayerCommand command = {
      .type = Play,
      .data =
          {
              .melody = melody,
              .volume = volume,
              .replay = false,
          },
  };
  xQueueSend(music_player->command_queue, &command, 0);
}

// void music_player_play_melody_it(MusicPlayer* music_player,
//                                  int16_t* melody,
//                                  float volume,
//                                  bool replay) {
//   if (music_player->is_active, melody == NULL || melody[0] == 0 ||
//                                    melody[1] != melody[0] ||
//                                    melody[2] == END_OF_MELODY)
//     return;
//   music_player_load_melody(music_player, melody);
//   music_player_resume(music_player, volume, replay);
// }

// void music_player_resume(MusicPlayer* music_player, float volume, bool
// replay) {
//   music_player->volume = volume;
//   if (music_player->is_active || music_player->current_melody == NULL ||
//       music_player->tempo == 0)
//     return;
//   music_player->is_active = true;
//   music_player->replay = replay;
//   int16_t note =
//   music_player->current_melody[music_player->current_note_index]; int16_t
//   note_duration_divider =
//       music_player->current_melody[music_player->current_note_index + 1];

//   uint32_t note_duration_ms =
//       get_note_duration_ms(note_duration_divider, music_player->tempo);
//   music_player->current_note_index += 2;

//   pwm_start(music_player->device);
//   buzzer_set_volume(music_player->device, music_player->volume);
//   pwm_set_frequency(music_player->device, note);
//   start_timer_by_period_ms_it(music_player->timer, note_duration_ms,
//                               APB2_CLOCK_FREQUENCY);
// }

// void music_player_stop_it(MusicPlayer* music_player) {
//   music_player->is_active = false;
//   pwm_stop(music_player->device);
// }

// void music_player_play_next_note(MusicPlayer* music_player) {
//   if (!music_player->is_active || music_player->current_melody == NULL ||
//       music_player->tempo == 0)
//     return;
//   if (music_player->current_melody[music_player->current_note_index] ==
//           END_OF_MELODY &&
//       music_player->replay) {
//     music_player_stop_it(music_player);
//     music_player_play_melody_it(music_player, music_player->current_melody,
//                                 music_player->volume,
//                                 music_player->replay);
//     return;
//   }
//   if (music_player->current_melody[music_player->current_note_index] ==
//           END_OF_MELODY &&
//       !music_player->replay) {
//     music_player_stop_it(music_player);
//     music_player->on_music_ended();
//     return;
//   }
//   int16_t note =
//   music_player->current_melody[music_player->current_note_index]; int16_t
//   note_duration_divider =
//       music_player->current_melody[music_player->current_note_index + 1];

//   uint32_t note_duration_ms =
//       get_note_duration_ms(note_duration_divider, music_player->tempo);

//   pwm_set_frequency(music_player->device, note);

//   music_player->current_note_index += 2;

//   start_timer_by_period_ms_it(music_player->timer, note_duration_ms,
//                               APB2_CLOCK_FREQUENCY);
// }
