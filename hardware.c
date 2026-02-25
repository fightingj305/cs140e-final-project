#include "hardware.h"

const Pin tx = {14};
const Pin rx = {15};

UART uart = {
    .tx_pin = tx,
    .rx_pin = rx,
    .num_bits = UART_8_BIT,
    .baud_rate = 115200
};