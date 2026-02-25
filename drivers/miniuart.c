#include "miniuart.h"
#include "utils.h"

void UART_Enable() {
    
    uint32_t old_state = GET32(AUX_ENABLES);
    PUT32(AUX_ENABLES, old_state | 1);
    
}
void UART_Disable() {
    
    uint32_t old_state = GET32(AUX_ENABLES);
    PUT32(AUX_ENABLES, old_state & (~1));
    
}
void UART_Config(UART *uart) {
    
    GPIO_Config(&uart->tx_pin, AF5);
    GPIO_Config(&uart->rx_pin, AF5);
    const uint32_t reg_value = (TIM_CLOCK_RATE / (8 * uart->baud_rate)) - 1;
    
    // PUT32(AUX_MU_CNTL_REG, 0);  // doing this is pointless
    PUT32(AUX_MU_IER_REG, 0); // not necessary but i'll humor them
    PUT32(AUX_MU_IIR_REG, 0b11 << 1); // clear both FIFOs
    PUT32(AUX_MU_LCR_REG, uart->num_bits == UART_8_BIT ? 0b11 : 0b00);
    PUT32(AUX_MU_BAUD_REG, reg_value);
    PUT32(AUX_MU_CNTL_REG, 0b11);
    
}

void UART_Send_Byte(const uint8_t byte) {
    
    while (!UART_TX_Ready());
    PUT32(AUX_MU_IO_REG, byte);
        
}

void UART_Send_String(char *str) {
    for (char *c = str; *c != '\0'; c++) {
        UART_Send_Byte(*c);
    }
}

uint8_t UART_Receive_Byte() {
    
    while(!UART_RX_Ready());
    return (uint8_t) (GET32(AUX_MU_IO_REG) & 0xFF);
    
}

void UART_Flush_TX() {
    
    while (!UART_TX_Empty());
    
}

bool UART_TX_Empty() {
    return ((GET32(AUX_MU_LSR_REG) >> 6) & 1) == 1;
}

bool UART_TX_Ready() {
    return ((GET32(AUX_MU_LSR_REG) >> 5) & 1) == 1;
}

bool UART_RX_Ready() {
    return (GET32(AUX_MU_LSR_REG) & 1) == 1;
}
