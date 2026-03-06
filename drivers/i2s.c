#include "i2s.h"
#include "utils.h"

void PCM_Clock_Setup() {
    PUT32(CM_PCMCTL, CM_PASSWD | CM_CTL_KILL);

    while (GET32(CM_PCMCTL) & CM_CTL_BUSY);

    PUT32(CM_PCMDIV,
        CM_PASSWD |
        (162 << CM_DIV_DIVI_SHIFT) |
        (3113 << CM_DIV_DIVF_SHIFT));

    PUT32(CM_PCMCTL, CM_PASSWD | CM_SRC_PLLD | CM_CTL_MASH_1 | CM_CTL_ENAB);
    while (!(GET32(CM_PCMCTL) & CM_CTL_BUSY));  // wait for stable clock
}

void I2S_Init(I2S *i2s) {
    ASSERT(i2s->rx_data_size <= 32 && i2s->tx_data_size <= 32);
    ASSERT((i2s->frame_size & 0x3FF) == i2s->frame_size);
    PCM_Clock_Setup();
    PUT32(PCM_CS_A, PCM_CS_EN);
    PUT32(PCM_MODE_A, PCM_MODE_CLK_DIS); // disable clock prior to configuring mode?
    PUT32(PCM_MODE_A, PCM_MODE_FSI | (((i2s->frame_size * 2)-1) << PCM_MODE_FLEN_BIT) | (i2s->frame_size << PCM_MODE_FSLEN_BIT));
    uint32_t rx_data_size = ((i2s->rx_data_size - 8) << PCM_CH1_WID_BIT);
    uint32_t tx_data_size = ((i2s->tx_data_size - 8) << PCM_CH1_WID_BIT);
    if (i2s->rx_data_size >= 24) {
        rx_data_size = PCM_CH1_WEX | (i2s->rx_data_size - 24) << PCM_CH1_WID_BIT;
    }
    if (i2s->tx_data_size >= 24) {
        tx_data_size = PCM_CH1_WEX | (i2s->tx_data_size - 24) << PCM_CH1_WID_BIT;
    }
    PUT32(PCM_RXC_A, PCM_CH1_EN | PCM_CH1_POS_1 | rx_data_size); // pos 1 for i2s
    PUT32(PCM_TXC_A, PCM_CH1_EN | PCM_CH1_POS_1 | tx_data_size);
    I2S_Clear_Flags();
    UART_Send_String("Fifo about to clear");
    I2S_Clear_FIFO();
    UART_Send_String("Fifo clear");

    if (i2s->bclk.pin_num == 18 && i2s->fs.pin_num == 19 && i2s->din.pin_num == 20 && i2s->dout.pin_num == 21) {
        GPIO_Config(&i2s->bclk, AF0);
        GPIO_Config(&i2s->fs, AF0);
        GPIO_Config(&i2s->din, AF0);
        GPIO_Config(&i2s->dout, AF0);
    }
    else if (i2s->bclk.pin_num == 28 && i2s->fs.pin_num == 29 && i2s->din.pin_num == 30 && i2s->dout.pin_num == 31) {
        GPIO_Config(&i2s->bclk, AF2);
        GPIO_Config(&i2s->fs, AF2);
        GPIO_Config(&i2s->din, AF2);
        GPIO_Config(&i2s->dout, AF2);
    }
    else {
        PANIC("Invalid I2S GPIO CONFIG");
    }
}

void I2S_Wait_2() {
    if (GET32(PCM_MODE_A) & PCM_MODE_CLK_DIS)
        return;

    uint32_t current_value = GET32(PCM_CS_A);
    PUT32(PCM_CS_A, current_value ^ PCM_CS_SYNC); // invert the current sync val; wait for it to match
    while (GET32(PCM_CS_A) != (current_value ^ PCM_CS_SYNC));
}

void I2S_Clear_Flags() {
    RMW_OR(PCM_CS_A, PCM_CS_RXSYNC | PCM_CS_TXSYNC | PCM_CS_RXERR | PCM_CS_TXERR);
}

void I2S_Clear_FIFO() {
    RMW_OR(PCM_CS_A, PCM_CS_RXCLR | PCM_CS_TXCLR);
    I2S_Wait_2();
}

void I2S_Enable_Comms() {
    RMW_OR(PCM_CS_A, PCM_CS_RXON | PCM_CS_TXON);
}
void I2S_Disable_Comms() {
    RMW_AND(PCM_CS_A, ~(PCM_CS_RXON | PCM_CS_TXON));
}

bool I2S_TX_Full() {
    return (GET32(PCM_CS_A) & PCM_CS_TXD) == 0;
}

bool I2S_RX_Ready() {
    return (GET32(PCM_CS_A) & PCM_CS_RXD) != 0;
}

void I2S_Send_Value(uint32_t frame) {
    while (I2S_TX_Full());
    PUT32(PCM_FIFO_A, frame);
    while (I2S_TX_Full());
    PUT32(PCM_FIFO_A, 0);
}

uint32_t I2S_Read_Value() {
    uint32_t out;
    while (!I2S_RX_Ready());
    out = GET32(PCM_FIFO_A);
    while (!I2S_RX_Ready());
    GET32(PCM_FIFO_A);
    return out;
}