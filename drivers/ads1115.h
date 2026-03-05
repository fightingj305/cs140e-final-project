#pragma once

#include "i2c.h"

// ADS communication scheme: send i2c address with r/w bit; send register address, send two bytes of data 

enum ADS1115_Config_Bits {
    ADS_CONFIG_OS                = 1 << 15,

    ADS_CONFIG_MUX_MASK          = 0x7 << 12,
    ADS_CONFIG_MUX_DIFF_0_1      = 0x0 << 12,
    ADS_CONFIG_MUX_DIFF_0_3      = 0x1 << 12,
    ADS_CONFIG_MUX_DIFF_1_3      = 0x2 << 12,
    ADS_CONFIG_MUX_DIFF_2_3      = 0x3 << 12,
    ADS_CONFIG_MUX_SINGLE_0      = 0x4 << 12,
    ADS_CONFIG_MUX_SINGLE_1      = 0x5 << 12,
    ADS_CONFIG_MUX_SINGLE_2      = 0x6 << 12,
    ADS_CONFIG_MUX_SINGLE_3      = 0x7 << 12,

    ADS_CONFIG_PGA_MASK          = 0x7 << 9,
    ADS_CONFIG_PGA_6_144V        = 0x0 << 9,
    ADS_CONFIG_PGA_4_096V        = 0x1 << 9,
    ADS_CONFIG_PGA_2_048V        = 0x2 << 9,
    ADS_CONFIG_PGA_1_024V        = 0x3 << 9,
    ADS_CONFIG_PGA_0_512V        = 0x4 << 9,
    ADS_CONFIG_PGA_0_256V        = 0x5 << 9,
    ADS_CONFIG_PGA_0_256V_2      = 0x6 << 9,
    ADS_CONFIG_PGA_0_256V_3      = 0x7 << 9,

    ADS_CONFIG_MODE_CONTINUOUS   = 0 << 8,
    ADS_CONFIG_MODE_SINGLESHOT   = 1 << 8,

    ADS_CONFIG_DR_MASK           = 0x7 << 5,
    ADS_CONFIG_DR_8SPS           = 0x0 << 5,
    ADS_CONFIG_DR_16SPS          = 0x1 << 5,
    ADS_CONFIG_DR_32SPS          = 0x2 << 5,
    ADS_CONFIG_DR_64SPS          = 0x3 << 5,
    ADS_CONFIG_DR_128SPS         = 0x4 << 5,
    ADS_CONFIG_DR_250SPS         = 0x5 << 5,
    ADS_CONFIG_DR_475SPS         = 0x6 << 5,
    ADS_CONFIG_DR_860SPS         = 0x7 << 5,

    ADS_CONFIG_COMP_MODE_TRAD    = 0 << 4,
    ADS_CONFIG_COMP_MODE_WINDOW  = 1 << 4,

    ADS_CONFIG_COMP_POL_LOW      = 0 << 3,
    ADS_CONFIG_COMP_POL_HIGH     = 1 << 3,

    ADS_CONFIG_COMP_LAT_NONLAT   = 0 << 2,
    ADS_CONFIG_COMP_LAT_LATCH    = 1 << 2,

    ADS_CONFIG_COMP_QUE_1        = 0x0,
    ADS_CONFIG_COMP_QUE_2        = 0x1,
    ADS_CONFIG_COMP_QUE_4        = 0x2,
    ADS_CONFIG_COMP_DISABLE      = 0x3
};

enum ADS1115_ADDRS {
    ADS_ADDR_GND = 0b1001000,
    ADS_ADDR_VDD,
    ADS_ADDR_SDA,
    ADS_ADDR_SCL
};

typedef enum ADS1115_REGS { // 8 bit addresses, 16 bit registers
    ADS_CONVERSION,
    ADS_CONFIG,
    ADS_LO_THRESH,
    ADS_HI_THRESH
} ADS_Reg;

typedef enum ADS1115_CHANNELS { // 16 bit
    ADS_AIN0 = ADS_CONFIG_MUX_SINGLE_0,
    ADS_AIN1 = ADS_CONFIG_MUX_SINGLE_1,
    ADS_AIN2 = ADS_CONFIG_MUX_SINGLE_2,
    ADS_AIN3 = ADS_CONFIG_MUX_SINGLE_3
} ADS_Channel;

// PGA 4.096 V allows us to read up to 4.096V; max 16-bit signed value of 32768 = 4.096 so 3.3V reads 26400 theoretically
#define ADS_DEFAULT_CONFIG ADS_CONFIG_PGA_4_096V | ADS_CONFIG_MODE_SINGLESHOT | ADS_CONFIG_DR_250SPS | ADS_CONFIG_COMP_DISABLE 

typedef struct ADS1115_t {
    I2C *i2c; // associated I2C instance
    ADS_Reg last_register; // internal state to avoid redundant sends
} ADS1115;

void ADS1115_Init(ADS1115 *ads);
uint16_t ADS1115_Read_Reg(ADS1115 *ads, ADS_Reg reg);
void ADS1115_Write_Reg(ADS1115 *ads, ADS_Reg reg, uint16_t value);
uint16_t ADS1115_Read_Channel(ADS1115 *ads, ADS_Channel channel);