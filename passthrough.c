#include "hardware.h"
#include "miniuart.h"
#include "i2s.h"
#include "utils.h"
#include <stdint.h>
#include "gpclk.h"

int main() {
    UART_Disable();
    UART_Config(&uart);
    UART_Enable();
    UART_Send_String("Format Conversion Passthrough\n");

    GPCLK_Init(&pcm1808_sck);
    GPCLK_Pin_Init(&pcm1808_sck);
    DELAY(10000000);

    I2S_Init(&i2s);

    for (int i = 0; i < 16; i++) {
        PUT32(PCM_FIFO_A, 0);
    }

    I2S_Clear_Flags();
    I2S_Enable_Comms();
    
    UART_Send_String("Running\n");

    while (1) {
        uint32_t adc_val = I2S_Read_Value();
        
        // Sign extend 24-bit to 32-bit int
        int32_t signed_val = (int32_t)(adc_val << 8) >> 8;
        
        // Amplify
        signed_val = signed_val << 3;
        
        // Clip
        if (signed_val > 8388607) signed_val = 8388607;
        if (signed_val < -8388608) signed_val = -8388608;
        
        // Convert BACK to 24-bit format like ADC uses
        uint32_t output = ((uint32_t)signed_val) & 0x00FFFFFF;
        
        I2S_Send_Value(output);
    }

    return 0;
}