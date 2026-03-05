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

ADS1115 ads = {
    .i2c = &i2c,
};