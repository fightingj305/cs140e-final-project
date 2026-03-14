#include "ads1115.h"
#include "utils.h"
#include "miniuart.h"

// i was gonna use this for the input but its sampling rate is too low so it only reads pots now so we really don't gaf about speed

void ADS1115_Init(ADS1115 *ads) {
    ads->i2c->slave_addr = ADS_ADDR_GND;
    I2C_Init(ads->i2c);
    I2C_Enable(ads->i2c);
    ads->last_register = 0xFF;
    ADS1115_Write_Reg(ads, ADS_CONFIG, ADS_DEFAULT_CONFIG);
}

uint16_t ADS1115_Read_Reg(ADS1115 *ads, ADS_Reg reg) {
    if (ads->last_register != reg) {
        I2C_Send_Data(ads->i2c, 1, &reg);
        ads->last_register = reg;
    }
    uint8_t data[2];
    I2C_Receive_Data(ads->i2c, 2, data);
    return ((uint16_t) data[0] << 8) | (uint16_t) data[1];
}

void ADS1115_Write_Reg(ADS1115 *ads, ADS_Reg reg, uint16_t value) {
    // as per datasheet, write always expects the reg and so we can't do the if check like in read
    uint8_t data[3];
    data[0] = reg;
    data[1] = (uint8_t) (value >> 8);
    data[2] = (uint8_t) (value & 0xFF);

    I2C_Send_Data(ads->i2c, 3, data);
    ads->last_register = reg;
}

uint16_t ADS1115_Read_Channel(ADS1115 *ads, ADS_Channel channel) {
    ADS1115_Write_Reg(ads, ADS_CONFIG, ADS_CONFIG_OS | channel | ADS_DEFAULT_CONFIG);
    while (!(ADS1115_Read_Reg(ads, ADS_CONFIG) & ADS_CONFIG_OS)); // we could do this better with a gpio poll on the alert pin? i lowk can't be bothered tho, enjoy the i2c flood
    return ADS1115_Read_Reg(ads, ADS_CONVERSION);
}
