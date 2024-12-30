#include "core.h"
#include "player.h"

#include <stdbool.h>
#include <stm32f4xx_ll_utils.h>

int main(void) {
    core_init();
    player_init(130);
    while(true) {
    }
    return 0;
}
