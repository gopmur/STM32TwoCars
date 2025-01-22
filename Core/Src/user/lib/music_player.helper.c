#include <stdlib.h>

#include "music_player.helper.h"

uint32_t get_note_duration_ms(int16_t note_duration_divider, uint16_t tempo) {
  const uint32_t note_base_duration_ms = 60000 / tempo;
  uint32_t note_duration_ms =
      note_duration_divider >= 0
          ? note_base_duration_ms / note_duration_divider
          : note_base_duration_ms / abs(note_duration_divider) * 1.5;
  return note_duration_ms;
}