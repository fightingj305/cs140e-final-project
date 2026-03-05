#include "hardware.h"
#include "miniuart.h"
#include "ads1115.h"
#include "utils.h"

int main() {
    UART_Disable();
    UART_Config(&uart);
    UART_Enable();
    ADS1115_Init(&ads);
    uint16_t cfg = ADS1115_Read_Reg(&ads, ADS_CONFIG);
    UART_Send_String("Config: ");
    UART_Send_Int(cfg);
    while (1) {
        uint16_t data = ADS1115_Read_Channel(&ads, ADS_AIN0);
        uint16_t data1 = ADS1115_Read_Channel(&ads, ADS_AIN1);
        UART_Send_String("Channel 0 Read: ");
        UART_Send_Int(data);
        UART_Send_Byte('\n');
        UART_Send_String("Channel 1 Read: ");
        UART_Send_Int(data1);
        UART_Send_Byte('\n');
        DELAY(1000000);
    }
    return 0;
}