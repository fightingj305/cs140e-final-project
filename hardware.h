#pragma once

#include "gpio.h"
#include "miniuart.h"
#include "i2c.h"

extern UART uart;
extern const Pin rx;
extern const Pin tx;

extern I2C i2c;
extern const Pin sda;
extern const Pin scl;
