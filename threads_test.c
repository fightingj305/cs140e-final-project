#include "hardware.h"
#include "miniuart.h"
#include "utils.h"
#include <stdint.h>
#include "interrupts.h"
#include "threads.h"


void function_1(){
    int i = 0;
    while(1) {
        UART_Send_String("Function 1: Print ");
        UART_Print_Int(i);
        i++;
        TIM_SYS_Delay_Millis(500);
    }
}

void function_2(){
    int i = 0;
    while(1) {
        UART_Send_String("Function 2: Print ");
        UART_Print_Int(i);
        i++;
        TIM_SYS_Delay_Millis(500);
    }
}

thread thread1 = {
    .function = &function_1,
};

thread thread2 = {
    .function = &function_2,
};

schedule sched = {
    .curr_thread = 0,
    .num_threads = 2,
};

thread *threads[2];
void undefined_instruction_vector(void) {
    UART_Send_String("UNDEF\n");
    while(1);
}

void prefetch_abort_vector(void) {
    UART_Send_String("PREFETCH\n");
    while(1);
}

void data_abort_vector(void) {
    UART_Send_String("DATA ABORT\n");
    while(1);
}

void software_interrupt_vector(void) {
    UART_Send_String("SWI\n");
    while(1);
}

void fast_interrupt_vector(void) {
    UART_Send_String("FIQ\n");
    while(1);
}

void c_interrupt_handler() {
    return;
}

int main() {
    interrupts_off();
    TIM_Disable();
    UART_Disable();
    UART_Config(&uart);
    UART_Enable();
    UART_Send_String("Program start\n");
    
    threads[0] = &thread1;
    threads[1] = &thread2;

    sched.threads = threads;
    
    thread_init(&thread1);
    thread_init(&thread2);


    UART_Send_String("Enabling switching\n");
    
    TIM_Set_Frequency(20);
    TIM_Enable();
    scheduler_start(&sched);
    interrupts_on();
    // should never get here
    
    while (1) {
    }

    return 0;
}