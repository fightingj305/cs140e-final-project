#include "timer.h"
#include "interrupts.h"
#include "utils.h"

void TIM_Enable(void) {
    PUT32(TIM_CTRL, TIM_CTRL_TIMER_INTERRUPT | TIM_CTRL_TIMER_ENABLE | TIM_CTRL_23BIT_COUNTER);
    PUT32(TIM_PREDIVIDER, 0);
    IRQ_Enable_Interrupt(IRQ_ARM_TIMER_INT);
}

void TIM_Disable(void) {
    RMW_AND(TIM_CTRL, ~TIM_CTRL_TIMER_ENABLE);
    IRQ_Disable_Interrupt(IRQ_ARM_TIMER_INT);
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

void TIM_SYS_Delay_Millis(uint32_t ms) {
    uint32_t start = GET32(TIM_SYS_CLO);
    uint32_t wait = ms * 1000;

    while ((GET32(TIM_SYS_CLO) - start) < wait);
}