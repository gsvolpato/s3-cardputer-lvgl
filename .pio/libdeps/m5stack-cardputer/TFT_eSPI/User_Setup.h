#ifndef USER_SETUP_H
#define USER_SETUP_H

#define USER_SETUP_LOADED 1

// Driver
#define ST7789_DRIVER 1

// Display resolution
#define TFT_WIDTH  135
#define TFT_HEIGHT 240
#define TFT_ROTATION 1

// Color order
#define TFT_RGB_ORDER 1

// Pins
#define TFT_MISO -1
#define TFT_MOSI 35
#define TFT_SCLK 36
#define TFT_CS   37
#define TFT_DC   34
#define TFT_RST  33
#define TFT_BL   38

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
