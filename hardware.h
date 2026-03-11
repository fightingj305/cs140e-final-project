#pragma once

#include "gpio.h"
#include "miniuart.h"
#include "i2c.h"
#include "ads1115.h"
#include "i2s.h"
#include "gpclk.h"
#include "spi.h"
#include "st7789.h"

extern UART uart;
extern const Pin rx;
extern const Pin tx;

extern I2C i2c;
extern const Pin sda;
extern const Pin scl;

extern I2S i2s;
extern const Pin bclk;
extern const Pin fs;
extern const Pin din;
extern const Pin dout;

extern SPI spi;
extern const Pin mosi;
extern const Pin miso;
extern const Pin sclk;
extern const Pin cs0;
extern const Pin cs1;
extern const Pin dc;

extern GPCLK pcm1808_sck;

extern ADS1115 ads;

extern ST7789 st7789;