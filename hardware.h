#pragma once

#include "gpio.h"
#include "miniuart.h"
#include "i2c.h"
#include "ads1115.h"
#include "i2s.h"

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

extern ADS1115 ads;