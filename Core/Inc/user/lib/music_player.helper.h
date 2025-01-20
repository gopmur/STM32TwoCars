#pragma once

#include "main.h"
#include "music_player.h"

void music_player_load_melody(MusicPlayer *music_player, int16_t* melody);
uint32_t get_note_duration_ms(int16_t note_duration_divider, uint16_t tempo);