#pragma once

#include <stdint.h>
#include "miniuart.h"

#define CPU_HZ        700000000UL   // 700MHz
#define CYCLES_PER_US (CPU_HZ / 1000000UL)  // 700
#define CYCLES_PER_MS (CPU_HZ / 1000UL)     // 700000

#define ASSERT(x) \
    do { \
        if(!(x)) PANIC("Assertion failed: " #x); \
    } while(0)

#define DELAY(cycles) do { \
        for (volatile uint32_t i = (cycles); i > 0; i--); \
    } while(0)

#define DELAY_US(x) DELAY((x) * CYCLES_PER_US)

#define DELAY_MS(x) DELAY((x) * CYCLES_PER_MS)

#define PANIC(msg) do { \
        UART_Send_String("PANIC: "); \
        UART_Send_String(msg); \
        UART_Send_String(" at "); \
        UART_Send_String(__FILE__); \
        UART_Send_String(":"); \
        UART_Send_Int(__LINE__); \
        UART_Send_String("\n"); \
        while(1); \
    } while(0)

__attribute__((naked))
void PUT32(uint32_t location, uint32_t value);

__attribute__((naked))
volatile uint32_t GET32(uint32_t location);

void put32(volatile uint32_t *addr, uint32_t value);

volatile uint32_t get32(volatile uint32_t *addr);

void RMW_OR(uint32_t reg, uint32_t mask);
void RMW_AND(uint32_t reg, uint32_t mask);