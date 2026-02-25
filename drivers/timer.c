#include "timer.h"
#include "interrupts.h"
#include "utils.h"

void TIM_Enable(void) {
    PUT32(TIM_CTRL, (1 << IRQ_EN_BIT) + (1 << EN_BIT) + (1 << 1));
    PUT32(IRQ_ENABLE_BASIC, 0x01);
    PUT32(TIM_PREDIVIDER, 0);
}

void TIM_Disable(void) {
    PUT32(TIM_CTRL, 0);
}

void TIM_Set_Load(uint32_t count) {
    PUT32(TIM_LOAD, count);
}

bool TIM_Pending(void){
    return GET32(TIM_MASK_IRQ) & 1;
}

void TIM_Clear_Pending(void) {
    PUT32(TIM_IRQ_CLR, 1);
}