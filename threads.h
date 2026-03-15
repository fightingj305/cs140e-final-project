#pragma once

#include "stdint.h"
#include "interrupts.h"

#define SUPERVISOR_MODE 0x13

typedef struct context_t {
    uint32_t r0;   // offset 0
    uint32_t r1;   // offset 4
    uint32_t r2;   // offset 8
    uint32_t r3;   // offset 12
    uint32_t r4;   // offset 16
    uint32_t r5;   // offset 20
    uint32_t r6;   // offset 24
    uint32_t r7;   // offset 28
    uint32_t r8;   // offset 32
    uint32_t r9;   // offset 36
    uint32_t r10;  // offset 40
    uint32_t r11;  // offset 44
    uint32_t r12;  // offset 48
    uint32_t sp;   // offset 52
    uint32_t lr;   // offset 56
    uint32_t pc;   // offset 60
    uint32_t cpsr; // offset 64
} context;

#define STACK_SIZE 4096

typedef struct thread_t {
    void (*function)(void);
    context ctx;
    uint32_t stack[STACK_SIZE];
} thread;

typedef struct schedule_t {
    uint32_t num_threads;
    uint32_t curr_thread;
    thread **threads;
} schedule;

extern schedule sched;

void restore_context(context *ctx);
void thread_init(thread *t);

void scheduler_start(schedule *sched);
void thread_yield(schedule *sched);