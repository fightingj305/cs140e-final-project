#include "st7789.h"
#include "utils.h"

void ST7789_Send_Command(ST7789 *st7789, ST7789_Command command) {
    GPIO_Set(&st7789->dc, ST7789_DC_COMMAND);
    uint8_t cmd = (uint8_t)command;
    SPI_Send_Multi(st7789->spi_device, 1, &cmd);
}

void ST7789_Send_Data_Byte(ST7789 *st7789, uint8_t byte) {
    GPIO_Set(&st7789->dc, ST7789_DC_DATA);
    SPI_Send_Multi(st7789->spi_device, 1, &byte);
}

void ST7789_Set_Range(ST7789 *st7789, uint16_t lcol, uint16_t rcol, uint16_t trow, uint16_t brow) {
    ST7789_Send_Command(st7789, ST7789_CASET);
    ST7789_Send_Data_Byte(st7789, lcol >> 8);
    ST7789_Send_Data_Byte(st7789, lcol);
    ST7789_Send_Data_Byte(st7789, rcol >> 8);
    ST7789_Send_Data_Byte(st7789, rcol);
    ST7789_Send_Command(st7789, ST7789_RASET);
    ST7789_Send_Data_Byte(st7789, trow >> 8);
    ST7789_Send_Data_Byte(st7789, trow);
    ST7789_Send_Data_Byte(st7789, brow >> 8);
    ST7789_Send_Data_Byte(st7789, brow);
}

// draws a block of w x h from x, y (with 0,0 top left) and a buffer of 16-bit 5-6-5 RGB values 
void ST7789_Write_Range(ST7789 *st7789, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *buffer) {
    ST7789_Set_Range(st7789, x, x + w - 1, y, y + h - 1);
    
    ST7789_Send_Command(st7789, ST7789_RAMWR);

    GPIO_Set(&st7789->dc, ST7789_DC_DATA);
    SPI_Send_Multi(st7789->spi_device, (uint32_t)w * h * 2, (uint8_t *)buffer);
}

void ST7789_Set_Pixel(ST7789 *st7789, uint16_t *buffer, uint32_t x, uint32_t y, uint16_t val) {
    ASSERT(x < TFT_WIDTH && y < TFT_HEIGHT);
    uint32_t index = y * TFT_WIDTH + x;
    buffer[index] = val;
}

void ST7789_Write_Buffer(ST7789 *st7789, uint16_t *buffer) {
    ST7789_Write_Range(st7789, 0, 0, TFT_WIDTH, TFT_HEIGHT, buffer);
}

uint16_t ST7789_RGB_To_16(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t rgb = (r & 0xF8) << 8 | (g & 0xFC) << 5 | (b & 0xF8) >> 3;
    return (rgb >> 8) | (rgb << 8); // we swap the bytes since ARM is little-endian but the dislpay expends big-endian transmissions
}

void ST7789_Init(ST7789 *st7789) {
    SPI_Config(st7789->spi);
    GPIO_Config(&st7789->dc, OUTPUT);
    ST7789_Send_Command(st7789, ST7789_SWRESET);
    UART_Send_String("Resetting\n");
    TIM_SYS_Delay_Millis(120);
    ST7789_Send_Command(st7789, ST7789_SLPOUT);
    UART_Send_String("Exiting Sleep\n");
    TIM_SYS_Delay_Millis(120);
    ST7789_Send_Command(st7789, ST7789_NORON);
    ST7789_Send_Command(st7789, ST7789_COLMOD);
    ST7789_Send_Data_Byte(st7789, ST7789_16_BIT | ST7789_RGB_65K);
    ST7789_Send_Command(st7789, ST7789_WRDISBV);
    ST7789_Send_Data_Byte(st7789, 0xFF);
    ST7789_Send_Command(st7789, ST7789_WRCTRLD);
    ST7789_Send_Data_Byte(st7789, (1 << 2) | (1 << 5));
    ST7789_Send_Command(st7789, ST7789_DISPON);
    ST7789_Send_Command(st7789, ST7789_INVON);
    UART_Send_String("Display On\n");
}