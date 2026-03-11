#include "spi.h"

#define ST7789_DC_DATA HIGH
#define ST7789_DC_COMMAND LOW

typedef struct ST7789_t {
    SPI *spi;
    Pin dc; // data/command pin (woo i love 5 wire spi)
} ST7789;

typedef enum ST7789_Command_t {
    ST7789_NOP        = 0x00,
    ST7789_SWRESET    = 0x01,
    ST7789_RDDID      = 0x04,
    ST7789_RDDST      = 0x09,
    ST7789_RDDPM      = 0x0A,
    ST7789_RDDMADCTL  = 0x0B,
    ST7789_RDDCOLMOD  = 0x0C,
    ST7789_RDDIM      = 0x0D,
    ST7789_RDDSM      = 0x0E,
    ST7789_RDDSDR     = 0x0F,
    ST7789_SLPIN      = 0x10,
    ST7789_SLPOUT     = 0x11,
    ST7789_PTLON      = 0x12,
    ST7789_NORON      = 0x13,
    ST7789_INVOFF     = 0x20,
    ST7789_INVON      = 0x21,
    ST7789_GAMSET     = 0x26,
    ST7789_DISPOFF    = 0x28,
    ST7789_DISPON     = 0x29,
    ST7789_CASET      = 0x2A,
    ST7789_RASET      = 0x2B,
    ST7789_RAMWR      = 0x2C,
    ST7789_RAMRD      = 0x2E,
    ST7789_PTLAR      = 0x30,
    ST7789_VSCRDEF    = 0x33,
    ST7789_TEOFF      = 0x34,
    ST7789_TEON       = 0x35,
    ST7789_MADCTL     = 0x36,
    ST7789_VSCRSADD   = 0x37,
    ST7789_IDMOFF     = 0x38,
    ST7789_IDMON      = 0x39,
    ST7789_COLMOD     = 0x3A,
    ST7789_RAMWRC     = 0x3C,
    ST7789_RAMRDC     = 0x3E,
    ST7789_TESCAN     = 0x44,
    ST7789_RDTESCAN   = 0x45,
    ST7789_WRDISBV    = 0x51,
    ST7789_RDDISBV    = 0x52,
    ST7789_WRCTRLD    = 0x53,
    ST7789_RDCTRLD    = 0x54,
    ST7789_WRCACE     = 0x55,
    ST7789_RDCABC     = 0x56,
    ST7789_WRCABCMB   = 0x5E,
    ST7789_RDCABCMB   = 0x5F,
    ST7789_RDABCSDR   = 0x68,
    ST7789_RDID1      = 0xDA,
    ST7789_RDID2      = 0xDB,
    ST7789_RDID3      = 0xDC,
} ST7789_Command;

void ST7789_Send_Command(ST7789 *st7789, ST7789_Command command);
void ST7789_Send_Data_Byte(ST7789 *st7789, uint8_t byte);

void ST7789_Init(ST7789 *st7789);