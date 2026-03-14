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

const Pin mosi = {10};
const Pin miso = {9};
const Pin sclk = {11};
const Pin cs0 = {8};
const Pin cs1 = {7};
const Pin dc = {12};

SPI spi = {
    .speed = 40000000, // 40 MHz
    .cpol = CPOL_0,
    .cpha = CPHA_0,
    .msb_first = true,

    .mosi = mosi,
    .miso = miso,
    .sclk = sclk,
    .cs0 = cs0,
    .cs1 = cs1
};

GPCLK pcm1808_sck = {
    .clk_periph = GPCLK0,
    .source = GPCLK_SRC_PLLD,
    .speed = 3125000 * 8
};

ADS1115 ads = {
    .i2c = &i2c,
};


ST7789 st7789 = {
    .spi = &spi,
    .spi_device = SPI_DEVICE_0,
    .dc = dc
};

Pin effect_control = {5};