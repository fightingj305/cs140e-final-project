#include "hardware.h"
#include "miniuart.h"
#include "i2s.h"
#include "utils.h"
#include <stdint.h>
#include "gpclk.h"
#include "interrupts.h"

#define DISPLAY_ROWS       TFT_HEIGHT
#define DOWNSAMPLE         4
#define SAMPLE_BUFFER_SIZE 4096

volatile int32_t sample_buffer[SAMPLE_BUFFER_SIZE];
volatile uint32_t sample_write = 0;
uint32_t sample_read = 0;

void __attribute__((interrupt("IRQ"))) interrupt_vector(void) {
    uint32_t adc_val = I2S_Read_Value();

    int32_t signed_val = (int32_t)(adc_val << 8) >> 8;
    signed_val = signed_val << 3;
    
    if (signed_val > 8388607)  signed_val =  8388607;
    if (signed_val < -8388608) signed_val = -8388608;

    sample_buffer[sample_write & (SAMPLE_BUFFER_SIZE - 1)] = signed_val;
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

int main() {
    UART_Disable();
    UART_Config(&uart);
    UART_Enable();
    UART_Send_String("ST test\n");

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

    I2S_Enable_IRQ();
    I2S_Enable_Comms();
    I2S_Clear_Flags();
    UART_Send_String("Running\n");

    uint16_t black = ST7789_RGB_To_16(0, 0, 0);
    uint16_t blue  = ST7789_RGB_To_16(0, 0, 255);
    int32_t cur_frame[DISPLAY_ROWS];
    for (int i = 0; i < DISPLAY_ROWS; i++) {
        cur_frame[i] = TFT_WIDTH / 2;
    }
    uint16_t row[TFT_WIDTH];
    while (1) {
        // --- collect a full frame of samples ---
        for (int i = 0; i < DISPLAY_ROWS; i++) {
            uint32_t target = sample_read + DOWNSAMPLE;
            while ((int32_t)(sample_write - target) < 0);

            if (sample_write - sample_read > SAMPLE_BUFFER_SIZE) {
                sample_read = sample_write - SAMPLE_BUFFER_SIZE / 2;
            }

            sample_read = target;
            cur_frame[i] = sample_to_x(
                sample_buffer[(sample_read - 1) & (SAMPLE_BUFFER_SIZE - 1)]
            );
        }

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
    }
    return 0;
}