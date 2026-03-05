#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TIM_BASE 0x2000B000
#define IRQ_EN_BIT 5
#define EN_BIT 7
#define CORE_CLOCK_RATE 250000000

enum {
    TIM_LOAD = TIM_BASE + 0x400,
    TIM_VALUE = TIM_BASE + 0x404,
    TIM_CTRL = TIM_BASE + 0x408,
    TIM_IRQ_CLR = TIM_BASE + 0x40C,
    TIM_RAW_IRQ = TIM_BASE + 0x410,
    TIM_MASK_IRQ = TIM_BASE + 0x414,
    TIM_RELOAD = TIM_BASE + 0x418,
    TIM_PREDIVIDER = TIM_BASE + 0x41C,
};

bool TIM_Pending(void);
void TIM_Clear_Pending(void);
void TIM_Set_Load(uint32_t value);
void TIM_Enable(void);
void TIM_Disable(void);