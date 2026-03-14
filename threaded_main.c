#include "hardware.h"
#include "miniuart.h"
#include "i2s.h"
#include "utils.h"
#include <stdint.h>
#include "gpclk.h"
#include "interrupts.h"
#include "threads.h"
#include "effects.h"

#define DISPLAY_ROWS       TFT_HEIGHT
#define DOWNSAMPLE         2
#define SAMPLE_BUFFER_SIZE 1024

volatile int32_t sample_buffer[SAMPLE_BUFFER_SIZE];
volatile uint32_t sample_write = 0;
volatile bool effects_on = true;
uint32_t sample_read = 0;

void c_interrupt_handler(void) {
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

static inline int32_t sample_to_x(int32_t s) {
    int32_t x = (TFT_WIDTH / 2) + (s / (8388607 / (TFT_WIDTH / 2)));
    if (x < 0)          x = 0;
    if (x >= TFT_WIDTH) x = TFT_WIDTH - 1;
    return x;
}

void potentiometer_process();
void display_process();

thread thread1 = {
    .function = &display_process,
};

thread thread2 = {
    .function = &potentiometer_process,
};

void potentiometer_process() {
    while(1) {
        UART_Send_String("pot loop\n");
        
        uint16_t read_value0 = ADS1115_Read_Channel(&ads, ADS_AIN0);
        uint16_t read_value1 = ADS1115_Read_Channel(&ads, ADS_AIN1);
        uint16_t read_value2 = ADS1115_Read_Channel(&ads, ADS_AIN2);
        uint16_t read_value3 = ADS1115_Read_Channel(&ads, ADS_AIN3);

        effects_on = GPIO_Read(&effect_control);
        effects_on ? UART_Send_String("Effects on") : UART_Send_String("Effects off");

        set_cutoff(read_value0);
        set_overdrive(read_value1);
        set_delay_time(read_value2);
        set_delay_feedback(read_value3);
        
        TIM_SYS_Delay_Millis(20);
    }
}

void display_process() {
    uint16_t black = ST7789_RGB_To_16(0, 0, 0);
    uint16_t blue  = ST7789_RGB_To_16(0, 0, 255);
    int32_t cur_frame[DISPLAY_ROWS];
    
    for (int i = 0; i < DISPLAY_ROWS; i++) {
        cur_frame[i] = TFT_WIDTH / 2;
    }
    
    uint16_t row[TFT_WIDTH];
    
    while (1) {
        UART_Send_String("disp loop\n");
        for (int i = 0; i < DISPLAY_ROWS; i++) {
            // Don't wait - just grab the latest samples
            uint32_t available = sample_write - sample_read;
            if (available >= DOWNSAMPLE) {
                sample_read += DOWNSAMPLE;
            }
            // else reuse last sample
            
            if (sample_write - sample_read > SAMPLE_BUFFER_SIZE) {
                sample_read = sample_write - SAMPLE_BUFFER_SIZE / 2;
            }

            cur_frame[i] = sample_to_x(
                sample_buffer[(sample_read - 1) & (SAMPLE_BUFFER_SIZE - 1)]
            );
        }
        
        TIM_Disable_IRQ();
        
        ST7789_Set_Range(&st7789, 0, TFT_WIDTH - 1, 0, DISPLAY_ROWS - 1);

        uint8_t cmd = ST7789_RAMWR;
        GPIO_Set(&st7789.dc, ST7789_DC_COMMAND);
        SPI_Send_Multi(st7789.spi_device, 1, &cmd);
        GPIO_Set(&st7789.dc, ST7789_DC_DATA);

        for (int i = 0; i < DISPLAY_ROWS - 1; i++) {
            for (int x = 0; x < TFT_WIDTH; x++)
                row[x] = black;

            int32_t x0 = cur_frame[i];
            int32_t x1 = cur_frame[i + 1];
            int32_t lo = x0 < x1 ? x0 : x1;
            int32_t hi = x0 > x1 ? x0 : x1;
            for (int x = lo; x <= hi; x++)
                row[x] = blue;

            SPI_Send_Multi(st7789.spi_device, TFT_WIDTH * 2, (uint8_t *)row);
        }
        TIM_Enable_IRQ();
    }
}

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

int main() {
    interrupts_off();
    TIM_Disable();

    UART_Disable();
    UART_Config(&uart);
    UART_Enable();
    
    UART_Send_String("*** BOOT ***\n");

    UART_Send_String("ST test\n");
    
    ADS1115_Init(&ads);    
    ST7789_Init(&st7789);

    uint16_t buffer[TFT_BUFFER_SIZE];
    for (uint32_t i = 0; i < TFT_BUFFER_SIZE; i++)
        buffer[i] = ST7789_RGB_To_16(0, 0, 0);
    ST7789_Write_Buffer(&st7789, buffer);

    GPCLK_Init(&pcm1808_sck);
    GPCLK_Pin_Init(&pcm1808_sck);
    TIM_SYS_Delay_Millis(500);

    I2S_Init(&i2s);
    for (int i = 0; i < 16; i++) PUT32(PCM_FIFO_A, 0);
    while (GET32(PCM_CS_A) & PCM_CS_RXR) (void)GET32(PCM_FIFO_A);
    
    threads[0] = &thread1;
    threads[1] = &thread2;
    sched.threads = threads;
    
    thread_init(&thread1);
    thread_init(&thread2);

    UART_Send_String("Enabling switching\n");
    I2S_Enable_Comms();
    I2S_Clear_Flags(); 
    I2S_Enable_IRQ();

    GPIO_Config(&effect_control, INPUT);

    TIM_Set_Frequency(100);
    TIM_Enable();
    scheduler_start(&sched);

    return 0;
}