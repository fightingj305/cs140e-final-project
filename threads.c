#include "threads.h"
#include "miniuart.h"
#include "utils.h"

void scheduler_start(schedule *sched) {
    restore_context(&sched->threads[0]->ctx);
}

void thread_init(thread *t) {
    t->ctx.sp = ((uint32_t)&t->stack[STACK_SIZE - 1]) & ~7;
    t->ctx.lr = 0;
    t->ctx.pc = (uint32_t)t->function;
    t->ctx.cpsr = SUPERVISOR_MODE; 

    t->ctx.r0 = 0;
    t->ctx.r1 = 0;
    t->ctx.r2 = 0;
    t->ctx.r3 = 0;
    t->ctx.r4 = 0;
    t->ctx.r5 = 0;
    t->ctx.r6 = 0;
    t->ctx.r7 = 0;
    t->ctx.r8 = 0;
    t->ctx.r9 = 0;
    t->ctx.r10 = 0;
    t->ctx.r11 = 0;
    t->ctx.r12 = 0;
}