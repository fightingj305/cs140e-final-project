#pragma once

#include <stdint.h>

#define ASSERT(x) while((x) != 1)
#define DELAY(x) for(volatile uint32_t i = x; i > 0; i--)

__attribute__((naked))
void PUT32(uint32_t location, uint32_t value);

__attribute__((naked))
volatile uint32_t GET32(uint32_t location);

void put32(volatile uint32_t *addr, uint32_t value);

volatile uint32_t get32(volatile uint32_t *addr);