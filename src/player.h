#pragma once

#include "beep.h"
#include "note.h"
#include <stdint.h>

void player_init(float tempo);
void player_load_data(Beeper beeper, const Note* notes, uint32_t notes_size);
void player_set_volume(Beeper beeper, float volume);
