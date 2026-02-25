#include "utils.h"

void PUT32(uint32_t location, uint32_t value) {
    asm volatile (
        "str r1, [r0]\n\t"
        "bx lr\n\t"
    );
}

volatile uint32_t GET32(uint32_t location) {
    asm volatile (
        "ldr r0, [r0]\n\t"
        "bx lr\n\t"
    );
}

void put32(volatile uint32_t *addr, uint32_t value) {
    PUT32((uint32_t) addr, value);
}

volatile uint32_t get32(volatile uint32_t *addr) {
    return GET32((uint32_t) addr);
}