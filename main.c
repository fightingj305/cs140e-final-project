#include "hardware.h"
#include "miniuart.h"
#include "i2s.h"
#include "utils.h"
#include <stdint.h>
#include "gpclk.h"
#include "interrupts.h"
#include "threads.h"
#include "effects.h"

#define CONTEXT_SWITCH_FREQ 100

#define DOWNSAMPLE         4
#define SAMPLE_BUFFER_SIZE 16384

volatile int32_t sample_buffer[SAMPLE_BUFFER_SIZE];
volatile uint32_t sample_write = 0;
volatile bool effects_on = true;
uint32_t sample_read = 0;

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

void c_interrupt_handler(void) {
    if (IRQ_Check_Interrupt(IRQ_PCM_INT)) {
        uint32_t adc_val = I2S_Read_Value();

        int32_t signed_val = (int32_t)(adc_val << 8) >> 8;
        signed_val = signed_val << 2;
        
        if (effects_on) {
            signed_val = process_signal(signed_val);
        }
        uint32_t write_idx = sample_write & (SAMPLE_BUFFER_SIZE - 1);
        sample_buffer[write_idx] = signed_val;
        sample_write++;
        
        I2S_Send_Value(((uint32_t)signed_val) & 0x00FFFFFF);
        I2S_Clear_IRQ();
    }
    else {
        PANIC("Unknown interrupt!");
    }
}

static inline int32_t sample_to_x(int32_t s) {
    int32_t x = (TFT_WIDTH / 2) + (s / (8388607 / (TFT_WIDTH / 2)));
    if (x < 0)          x = 0;
    if (x >= TFT_WIDTH) x = TFT_WIDTH - 1;
    return x;
}

void potentiometer_process() {
    while(1) {
        UART_Send_String("Pot\n");
        uint16_t read_value0 = ADS1115_Read_Channel(&ads, ADS_AIN0);
        set_cutoff(read_value0); // lpf

        uint16_t read_value1 = ADS1115_Read_Channel(&ads, ADS_AIN1);
        set_overdrive(read_value1); // clipping
        
        uint16_t read_value2 = ADS1115_Read_Channel(&ads, ADS_AIN2);
        set_delay_time(read_value2);
        
        uint16_t read_value3 = ADS1115_Read_Channel(&ads, ADS_AIN3);
        set_delay_feedback(read_value3);

        effects_on = GPIO_Read(&effect_control);
    }
}

void display_process() {
    int32_t cur_frame[TFT_HEIGHT];
    
    for (int i = 0; i < TFT_HEIGHT; i++) { // initialize buffer at midline
        cur_frame[i] = TFT_WIDTH / 2;
    }
    
    uint16_t row[TFT_WIDTH];
    
    while (1) {
        UART_Send_String("Disp\n");
        for (int i = 0; i < TFT_HEIGHT; i++) {
            uint32_t available = sample_write - sample_read;
            if (available >= DOWNSAMPLE) {
                sample_read += DOWNSAMPLE; // downsample!
            }
            
            if (sample_write - sample_read > SAMPLE_BUFFER_SIZE) {
                sample_read = sample_write - SAMPLE_BUFFER_SIZE; // catch up if we are too far behind
            }

            cur_frame[i] = sample_to_x(
                sample_buffer[(sample_read - 1) % SAMPLE_BUFFER_SIZE]
            );
        }

        
        ST7789_Set_Range(&st7789, 0, TFT_WIDTH - 1, 0, TFT_HEIGHT - 1);

        ST7789_Send_Command(&st7789, ST7789_RAMWR);

        GPIO_Set(&st7789.dc, ST7789_DC_DATA);

        for (int i = 0; i < TFT_HEIGHT - 1; i++) {
            int32_t x0 = cur_frame[i];
            int32_t x1 = cur_frame[i + 1];
            int32_t lo = x0 < x1 ? x0 : x1;
            int32_t hi = x0 > x1 ? x0 : x1;
            for (int x = 0; x < TFT_WIDTH; x++) {
                if (lo <= x && x <= hi) {
                    row[x] = ST7789_BLUE;
                }
                else {
                    row[x] = ST7789_BLACK;
                }
            }
            TIM_Disable_IRQ(); // small critical section just to make sure the rows write evenly.
            SPI_Send_Multi(st7789.spi_device, TFT_WIDTH * sizeof(uint16_t), (uint8_t *)row);
            TIM_Enable_IRQ();
        }
    }
}

thread thread1 = {
    .function = &display_process,
};

thread thread2 = {
    .function = &potentiometer_process,
};

schedule sched = {
    .curr_thread = 0,
    .num_threads = 2,
};

thread *threads[2];

int main() {
    interrupts_off();
    TIM_Disable();

    UART_Disable();
    UART_Config(&uart);
    UART_Enable();
    
    ADS1115_Init(&ads);
    GPIO_Config(&effect_control, INPUT);
    ST7789_Init(&st7789);

    GPCLK_Init(&pcm1808_sck);
    GPCLK_Pin_Init(&pcm1808_sck);
    TIM_SYS_Delay_Millis(200);

    I2S_Init(&i2s);
    while (!I2S_TX_Full()) {
        I2S_Send_Value(0);
    }
    
    I2S_Enable_Comms();
    I2S_Clear_Flags(); 
    I2S_Enable_IRQ();

    threads[0] = &thread1;
    threads[1] = &thread2;
    sched.threads = threads;
    
    thread_init(&thread1);
    thread_init(&thread2);

    TIM_Set_Frequency(CONTEXT_SWITCH_FREQ);
    TIM_Enable();

    scheduler_start(&sched);

    return 0;
}