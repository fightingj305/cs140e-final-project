#include "utils.h"
#include "timer.h"
#include "miniuart.h"
#include "interrupts.h"
#include "gpio.h"
#include "hardware.h"
#include <stdlib.h>


void __attribute__((interrupt("UNDEF"))) undefined_instruction_vector(void) {
    while (1);
}

void __attribute__((interrupt("SWI"))) software_interrupt_vector(void) {
    return;
}

void __attribute__((interrupt("ABORT"))) prefetch_abort_vector(void) {
    while (1);
}

void __attribute__((interrupt("ABORT"))) data_abort_vector(void) {
    while (1);
}

void __attribute__((interrupt("IRQ"))) interrupt_vector(void) {
    return;
}

void __attribute__((interrupt("IRQ"))) fast_interrupt_vector(void) {
    return;
}