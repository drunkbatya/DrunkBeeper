#include "player.h"
#include "note.h"

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_tim.h>
#include <stddef.h>

typedef struct {
    const Beeper beeper;
    const Note* notes;
    uint32_t notes_size;
    uint32_t index;
    uint32_t count_32s_remaining;
    float volume;
} Channel;

static Channel channels[BeeperCount] = {
    {.beeper = BeeperLow,
     .notes = NULL,
     .notes_size = 0,
     .index = 0,
     .count_32s_remaining = 0,
     .volume = 0},
    {.beeper = BeeperMid,
     .notes = NULL,
     .notes_size = 0,
     .index = 0,
     .count_32s_remaining = 0,
     .volume = 0},
    {.beeper = BeeperHigh,
     .notes = NULL,
     .notes_size = 0,
     .index = 0,
     .count_32s_remaining = 0,
     .volume = 0},
};

void player_load_data(Beeper beeper, const Note* notes, uint32_t notes_size) {
    Channel* channel = &channels[beeper];
    channel->notes = notes;
    channel->notes_size = notes_size;
    channel->index = 0;
    channel->count_32s_remaining = 0;
}

void player_set_volume(Beeper beeper, float volume) {
    Channel* channel = &channels[beeper];
    channel->volume = volume;
}

static void MX_TIM2_Init(float frequency_hz) {
    uint32_t freq_div = 84000000LU / frequency_hz;
    uint32_t prescaler = freq_div / 0x10000LU;
    uint32_t period = freq_div / (prescaler + 1);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM_InitStruct.Prescaler = prescaler;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = period - 1;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM2);
    LL_TIM_ClearFlag_UPDATE(TIM2);
    LL_TIM_EnableIT_UPDATE(TIM2);
}

float player_tempo_to_32s_frequency_hz(float tempo) {
    return (32 / ((float)60 / tempo));
}

void player_init(float tempo) {
    MX_TIM2_Init(player_tempo_to_32s_frequency_hz(tempo));
    beep_init();
    LL_TIM_EnableCounter(TIM2);
}

static void player_32_note_callback(void) {
    for(uint32_t channel_index = 0; channel_index < BeeperCount; channel_index++) {
        Channel* channel = &channels[channel_index];
        if(channel->count_32s_remaining != 0) {
            channel->count_32s_remaining--;
            continue;
        }

        if(channel->index < channel->notes_size) {
            if(channel->notes == NULL) {
                continue;
            }
            const Note* note = &channel->notes[channel->index];
            beep_play(channel->beeper, note->frequency, channel->volume);
            channel->count_32s_remaining = note->duration_in_32s;
            channel->index++;
        } else {
            beep_play(channel->beeper, 0, 0);
        }
    }
}

void TIM2_IRQHandler(void) {
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        player_32_note_callback();
    }
}
