#ifndef USER_SETUP_H
#define USER_SETUP_H

#define USER_SETUP_LOADED 1
 
// Driver
#define ST7789_2_DRIVER 1

// Display resolution
#define TFT_WIDTH  135
#define TFT_HEIGHT 240
#define TFT_ROTATION 1

// Color order
#define TFT_RGB_ORDER TFT_BGR

// Pins
#define TFT_MISO -1
#define TFT_MOSI 35
#define TFT_SCLK 36
#define TFT_CS   37
#define TFT_DC   34
#define TFT_RST  33
#define TFT_BL   38

// ST7789 commands
#define TFT_SWRST   0x01
#define TFT_CASET   0x2A
#define TFT_PASET   0x2B
#define TFT_RAMWR   0x2C
#define TFT_RAMRD   0x2E
#define TFT_INVOFF  0x20
#define TFT_INVON   0x21

// Other required defines
#define TFT_INIT_DELAY 0x80

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

// Other settings
#define SMOOTH_FONT
#define SPI_FREQUENCY  20000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

#endif 