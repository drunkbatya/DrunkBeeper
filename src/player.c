#include "player.h"
#include "beep.h"

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_tim.h>

#define SEMITONE_PAUSE 0xFF

#define NOTE_C4 261.63f
#define NOTE_C4_SEMITONE (4.0f * 12.0f)
#define TWO_POW_TWELTH_ROOT 1.059463094359f

typedef struct {
    float frequency;
    uint32_t duration_in_32s;
} Note;

static Note notes_low[] = {
    {.frequency = 293.66, .duration_in_32s = 32},
    {.frequency = 329.63, .duration_in_32s = 16},
    {.frequency = 0, .duration_in_32s = 16},
    {.frequency = 349.23, .duration_in_32s = 16},
    {.frequency = 349.23, .duration_in_32s = 16},
    {.frequency = 392.0, .duration_in_32s = 16},
    {.frequency = 392.0, .duration_in_32s = 16},
    {.frequency = 440.0, .duration_in_32s = 128},
};

static uint32_t index_low = 0;
static uint32_t count_32s_low = 0;

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
    // LOW
    if(count_32s_low != 0) {
        count_32s_low--;
        return;
    }

    uint32_t low_size = sizeof(notes_low) / sizeof(Note);
    if(index_low < low_size) {
        const Note* note = &notes_low[index_low];
        beep_play(BeeperLow, note->frequency, 60);
        count_32s_low = note->duration_in_32s;
        index_low++;
    } else {
        beep_play(BeeperLow, 0, 60);
    }
}

void TIM2_IRQHandler(void) {
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        player_32_note_callback();
    }
}
