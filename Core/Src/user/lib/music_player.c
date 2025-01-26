#include <malloc.h>

#include "entry.h"
#include "helper.h"
#include "music_player.h"
#include "music_player.helper.h"
#include "notes.h"

void music_player_load_melody(MusicPlayer* music_player, int16_t* melody) {
  music_player->current_melody = melody;
  music_player->tempo = melody[0];
  music_player->current_note_index = 2;
}

void music_player_store(MusicPlayer* music_player) {
  music_player->stored_melody = music_player->current_melody;
  music_player->stored_note_index = music_player->current_note_index;
  music_player->stored_tempo = music_player->tempo;
}

void music_player_load_stored(MusicPlayer* music_player) {
  music_player->current_melody = music_player->stored_melody;
  music_player->current_note_index = music_player->stored_note_index;
  music_player->tempo = music_player->stored_tempo;
}

bool music_player_play_next_note(MusicPlayer* music_player) {
  int16_t note = music_player->current_melody[music_player->current_note_index];
  if (note == END_OF_MELODY && music_player->melody_pushed) {
    music_player->melody_pushed = false;
    music_player_load_stored(music_player);
    note = music_player->current_melody[music_player->current_note_index];
  }
  if (note == END_OF_MELODY && !music_player->replay) {
    return false;
  }
  if (note == END_OF_MELODY) {
    music_player->current_note_index = 2;
  }
  int16_t duration_divider =
      music_player->current_melody[music_player->current_note_index + 1];
  uint32_t duration_ms =
      get_note_duration_ms(duration_divider, music_player->tempo);
  music_player->current_note_index += 2;
  if (xSemaphoreTake(music_player->volume_mutex, portMAX_DELAY)) {
    pwm_set_frequency(music_player->device, note);
    buzzer_set_volume(music_player->device, music_player->volume);
    xSemaphoreGive(music_player->volume_mutex);
  }
  osDelay(duration_ms);
  return true;
}

void music_player_thread(MusicPlayer* music_player) {
  MusicPlayerCommand command;
  while (true) {
    // here we are in the waiting for command state
    xQueueReceive(music_player->command_queue, &command, portMAX_DELAY);
    if (command.type != MUSIC_PLAYER_COMMAND_PLAY) {
      continue;
    }
    music_player->is_active = true;
    music_player_load_melody(music_player, command.data.play.melody);
    music_player->volume = command.data.play.volume;
    music_player->replay = command.data.play.replay;
    pwm_start(music_player->device);
    while (music_player_play_next_note(music_player)) {
      command.type = MUSIC_PLAYER_COMMAND_NONE;
      xQueueReceive(music_player->command_queue, &command, 0);
      if (command.type == MUSIC_PLAYER_COMMAND_STOP) {
        break;
      }
      if (command.type == MUSIC_PLAYER_COMMAND_PUSH &&
          !music_player->melody_pushed) {
        music_player->melody_pushed = true;
        music_player_store(music_player);
        music_player_load_melody(music_player, command.data.push.melody);
      } else if (command.type == MUSIC_PLAYER_COMMAND_SET_TEMPO &&
                 music_player->melody_pushed) {
        music_player->stored_tempo = command.data.set_tempo.tempo;
      } else if (command.type == MUSIC_PLAYER_COMMAND_SET_TEMPO &&
                 !music_player->melody_pushed) {
        music_player->tempo = command.data.set_tempo.tempo;
      }
    }
    music_player->is_active = false;
    pwm_stop(music_player->device);
  }
}

MusicPlayer* new_music_player(Pwm* device) {
  MusicPlayer* music_player = malloc(sizeof(MusicPlayer));

  music_player->device = device;
  music_player->current_melody = NULL;
  music_player->current_note_index = 0;
  music_player->is_active = false;
  music_player->replay = false;
  music_player->tempo = 0;
  music_player->command_queue = xQueueCreate(4, sizeof(MusicPlayerCommand));
  music_player->melody_pushed = false;
  music_player->volume_mutex = xSemaphoreCreateMutex();

  osThreadDef(musicPlayerThread, music_player_thread, osPriorityNormal, 0,
              128 * 2);
  osThreadCreate(osThread(musicPlayerThread), music_player);
  return music_player;
}

void music_player_play(MusicPlayer* music_player,
                       int16_t* melody,
                       float volume,
                       bool replay) {
  if (melody == NULL || melody[0] == 0 || melody[1] != melody[0] ||
      melody[2] == END_OF_MELODY)
    return;
  MusicPlayerCommand command = {
      .type = MUSIC_PLAYER_COMMAND_PLAY,
      .data.play =
          {
              .melody = melody,
              .volume = volume,
              .replay = replay,
          },
  };
  xQueueSend(music_player->command_queue, &command, 0);
}

void music_player_stop(MusicPlayer* music_player) {
  MusicPlayerCommand command = {
      .type = MUSIC_PLAYER_COMMAND_STOP,
  };
  xQueueSend(music_player->command_queue, &command, 0);
}

void music_player_resume(MusicPlayer* music_player) {
  MusicPlayerCommand command = {
      .type = MUSIC_PLAYER_COMMAND_RESUME,
  };
  xQueueSend(music_player->command_queue, &command, 0);
}

void music_player_set_tempo(MusicPlayer* music_player, int16_t tempo) {
  MusicPlayerCommand command = {
      .type = MUSIC_PLAYER_COMMAND_SET_TEMPO,
      .data.set_tempo.tempo = tempo,
  };
  xQueueSend(music_player->command_queue, &command, portMAX_DELAY);
}

void music_player_push(MusicPlayer* music_player, int16_t* melody) {
  MusicPlayerCommand command = {
      .type = MUSIC_PLAYER_COMMAND_PUSH,
      .data.push.melody = melody,
  };
  xQueueSend(music_player->command_queue, &command, portMAX_DELAY);
}

void music_player_set_volume(MusicPlayer* music_player, float volume) {
  volume = MIN(volume, 100);
  volume = MAX(volume, 0);
  if (xSemaphoreTake(music_player->volume_mutex, portMAX_DELAY)) {
    buzzer_set_volume(music_player->device, volume);
    music_player->volume = volume;
    xSemaphoreGive(music_player->volume_mutex);
  }
}