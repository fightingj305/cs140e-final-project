#include "hardware.h"

const Pin tx = {14};
const Pin rx = {15};

UART uart = {
    .tx_pin = tx,
    .rx_pin = rx,
    .num_bits = UART_8_BIT,
    .baud_rate = 115200
};

const Pin sda = {2};
const Pin scl = {3};

I2C i2c = {
    .bsc = BSC1,
    .sda = sda,
    .scl = scl,
    .slave_addr = 0x0F,
    .speed_hz = 400000
};


const Pin bclk = {18};
const Pin fs = {19};
const Pin din = {20};
const Pin dout = {21};

I2S i2s = {
    .frame_size = 32,
    .rx_data_size = 24,
    .tx_data_size = 24,
    .bclk = bclk,
    .fs = fs,
    .din = din,
    .dout = dout
};


ADS1115 ads = {
    .i2c = &i2c,
};