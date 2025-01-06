#pragma once

/* Keyboard Matrix GPIO Definitions */
// Row pins (outputs)
#define KB_ROW_COUNT 3
#define KB_ROW_0     8
#define KB_ROW_1     9
#define KB_ROW_2     11

// Column pins (inputs)
#define KB_COL_COUNT 7
#define KB_COL_0     13
#define KB_COL_1     15
#define KB_COL_2     3
#define KB_COL_3     4
#define KB_COL_4     5
#define KB_COL_5     6
#define KB_COL_6     7

/* I2C GPIO Definitions */
#define I2C_SDA  38
#define I2C_SCL  39
#define I2C_PORT 1

/* SPI GPIO Definitions */
#define SPI_CS   8
#define SPI_MOSI 6
#define SPI_MISO 5
#define SPI_SCLK 7

/* Legacy ESP32 GPIO Definitions */
#define ESP32_I2C_SDA  25
#define ESP32_I2C_SCL  21
#define ESP32_I2C_PORT 1

#define ESP32_SPI_CS           33
#define ESP32_SPI_MOSI         19
#define ESP32_SPI_MISO         22
#define ESP32_SPI_SCLK_LITE    23  // For ATOM Lite/Matrix
#define ESP32_SPI_SCLK_PSRAM   5   // For ATOM PSRAM 