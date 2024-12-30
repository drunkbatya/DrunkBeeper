#include "core.h"
#include "player.h"

#include "music_data.h"

#include <stdbool.h>
#include <stm32f4xx_ll_utils.h>

int main(void) {
    core_init();
    player_init(133);

    player_load_data(BeeperLow, beeper_data_low_arr, beeper_data_low_size);
    player_set_volume(BeeperLow, 0.2);

    player_load_data(BeeperMid, beeper_data_mid_arr, beeper_data_mid_size);
    player_set_volume(BeeperMid, 1);

    player_load_data(BeeperHigh, beeper_data_high_arr, beeper_data_high_size);
    player_set_volume(BeeperHigh, 1);

    while(true) {
    }

    return 0;
}
