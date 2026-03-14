#include "threads.h"
#include "miniuart.h"
#include "utils.h"

void scheduler_start(schedule *sched) {
    UART_Send_String("starting thread pc: ");
    UART_Print_Hex(sched->threads[0]->ctx.pc);
    UART_Send_String("starting thread sp: ");
    UART_Print_Hex(sched->threads[0]->ctx.sp);
    restore_context(&sched->threads[0]->ctx);
}

#define STACK_CANARY 0xDEADBEEF

void thread_init(thread *t) {
    t->ctx.sp = ((uint32_t)&t->stack[STACK_SIZE - 1]) & ~7;
    t->ctx.lr = 0;
    t->ctx.pc = (uint32_t)t->function;
    t->ctx.cpsr = 0x13;

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

    UART_Send_String("thread_init\n");
    UART_Send_String("  fn:   "); UART_Print_Hex((uint32_t)t->function);
    UART_Send_String("  pc:   "); UART_Print_Hex(t->ctx.pc);
    UART_Send_String("  sp:   "); UART_Print_Hex(t->ctx.sp);
    UART_Send_String("  cpsr: "); UART_Print_Hex(t->ctx.cpsr);
    UART_Send_String("  stack_base: "); UART_Print_Hex((uint32_t)t->stack);
    UART_Send_String("  stack_size: "); UART_Print_Int(STACK_SIZE * 4);
}