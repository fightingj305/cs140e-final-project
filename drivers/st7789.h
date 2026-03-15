#include "spi.h"

#define ST7789_DC_DATA HIGH
#define ST7789_DC_COMMAND LOW

#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_BUFFER_SIZE TFT_WIDTH * TFT_HEIGHT

#define ST7789_BLACK 0
#define ST7789_BLUE (0x1F << 8)

typedef struct ST7789_t {
    SPI *spi;
    SPI_Device spi_device;
    Pin dc; // data/command pin (woo i love 5 wire spi)
    uint16_t *buffer;
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


enum ST7789_COLMOD_Settings {
    ST7789_12_BIT      = 0b011,
    ST7789_16_BIT      = 0b101,
    ST7789_18_BIT      = 0b110,
    ST7789_16_M_BIT    = 0b111,

    ST7789_RGB_65K     = 0b101 << 4,
    ST7789_RGB_262K    = 0b110 << 4,
};

void ST7789_Send_Command(ST7789 *st7789, ST7789_Command command);
void ST7789_Send_Data_Byte(ST7789 *st7789, uint8_t byte);

void ST7789_Init(ST7789 *st7789);
// sets a range of rows and columns to write to
void ST7789_Set_Range(ST7789 *st7789, uint16_t lcol, uint16_t rcol, uint16_t trow, uint16_t brow);
// draws a block of w x h from x, y (with 0,0 top left) and a buffer of 16-bit 5-6-5 RGB values 
void ST7789_Write_Range(ST7789 *st7789, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *buffer);
uint16_t ST7789_RGB_To_16(uint8_t r, uint8_t g, uint8_t b);

void ST7789_Set_Pixel(ST7789 *st7789, uint16_t *buffer, uint32_t x, uint32_t y, uint16_t val);
void ST7789_Write_Buffer(ST7789 *st7789, uint16_t *buffer);