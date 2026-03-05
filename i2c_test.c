#include "hardware.h"
#include "miniuart.h"
#include "i2c.h"
#include "utils.h"

int main() {
    UART_Disable();
    UART_Config(&uart);
    UART_Enable();
    I2C_Init(&i2c);
    I2C_Enable(&i2c);
    while (1) {
        uint8_t byte = 0xF;
        I2C_Send_Data(&i2c, 1, &byte);
        UART_Send_String("Sending.\n");
        DELAY(1000000);
    }
    return 0;
}