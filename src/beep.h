#pragma once

typedef enum { BeeperLow, BeeperMid, BeeperHigh, BeeperCount } Beeper;

void beep_init(void);
void beep_play(Beeper beeper, float frequency, float volume);
