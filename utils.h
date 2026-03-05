#pragma once

#include <stdint.h>
#include "miniuart.h"

#define ASSERT(x) if((x) != 1) PANIC("Assertion failed!");
#define DELAY(x) for(volatile uint32_t i = x; i > 0; i--)
#define PANIC(x) do {UART_Send_String(x); while(1);}while(0);

__attribute__((naked))
void PUT32(uint32_t location, uint32_t value);

__attribute__((naked))
volatile uint32_t GET32(uint32_t location);

void put32(volatile uint32_t *addr, uint32_t value);

volatile uint32_t get32(volatile uint32_t *addr);