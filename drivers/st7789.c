#include "st7789.h"


void ST7789_Send_Command(ST7789 *st7789, ST7789_Command command) {
    GPIO_Set(&st7789->dc, ST7789_DC_COMMAND);
    SPI_Transaction(st7789->spi, command);
}

void ST7789_Send_Data_Byte(ST7789 *st7789, uint8_t byte) {
    GPIO_Set(&st7789->dc, ST7789_DC_DATA);
    SPI_Transaction(st7789->spi, byte);
}

void ST7789_Init(ST7789 *st7789) {
    GPIO_Config(&st7789->dc, OUTPUT);
    ST7789_Send_Command(st7789, ST7789_SWRESET);
    ST7789_Send_Command(st7789, ST7789_NORON);
    ST7789_Send_Command(st7789, ST7789_DISPON);
}