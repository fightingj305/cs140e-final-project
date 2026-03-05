#include "i2c.h"

// ADS communication scheme: send i2c address with r/w bit; send register address, send two bytes of data 

enum ADS1115_ADDRS {
    ADS_ADDR_GND = 0b1001000,
    ADS_ADDR_VDD,
    ADS_ADDR_SDA,
    ADS_ADDR_SCL
};

typedef enum ADS1115_REGS { // 16 bit
    ADS_CONVERSION,
    ADS_CONFIG,
    ADS_LO_THRESH,
    ADS_HI_THRESH
} ADS_Reg;

typedef struct ADS1115_t {
    I2C *i2c; // associated I2C instance
    ADS_Reg last_register; // internal state to avoid redundant sends
} ADS1115;

void ADS1115_Init(ADS1115 *ads);
uint16_t ADS1115_Read_Reg(ADS1115 *ads, ADS_Reg reg);
void ADS1115_Write_Reg(ADS1115 *ads, ADS_Reg reg, uint16_t value);