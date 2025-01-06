#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <lvgl.h>
#include "cardputer_gpios.h"
#include "skull_logo_240x135.h"
#include "menu.h"
#include "display_config.h"

// Initialize TFT display
TFT_eSPI tft = TFT_eSPI();

// Convert HSV to RGB color
uint16_t hsvToRgb565(uint8_t h, uint8_t s, uint8_t v) {
    uint8_t r, g, b;
    
    uint8_t i = h / 43;
    uint8_t f = (h - (i * 43)) * 6;
    uint8_t p = (v * (255 - s)) >> 8;
    uint8_t q = (v * (255 - ((s * f) >> 8))) >> 8;
    uint8_t t = (v * (255 - ((s * (255 - f)) >> 8))) >> 8;

    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }

    // Swap R and B channels for BGR order
    return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
}

void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    int16_t byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
            }
            if (byte & 0x80) {
                tft.drawPixel(x + i, y + j, color);
            }
        }
    }
}

void displayLogo() {
    // Draw skull with rainbow effect for 2 seconds
    uint32_t startTime = millis();
    while (millis() - startTime < 2000) {
        static uint8_t hue = 0;
        uint16_t color = hsvToRgb565(hue, 255, 255);
        drawBitmap(0, 0, skull_logo_240x135, 240, 135, color);
        hue += 15;
        delay(16); // Approximately 60fps
    }
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}
  
void setup() {
    Serial.begin(115200);
    Serial.println("Booting up");
    
    
    Serial.println("Initializing display");
    tft.init();
    
    Serial.println("TFT INVERSION OFF");
    tft.writecommand(TFT_INVON);
    
    Serial.println("TFT ROTATION");
    tft.setRotation(TFT_ROTATION);
    
    
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    
    tft.fillScreen(TFT_BLACK);
    
    
    Serial.println("Displaying logo");
    displayLogo();
    
    // Initialize LVGL
    lv_init();
    
    // Initialize display buffer for LVGL
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf1[TFT_HEIGHT * 10];
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, TFT_HEIGHT * 10);
    
    // Initialize LVGL display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_HEIGHT;
    disp_drv.ver_res = TFT_WIDTH;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    // Create the menu
    Serial.println("Displaying menu...");
    createMenu();
}

void loop() {
    lv_timer_handler(); // Handle LVGL tasks
    check_keyboard();   // Check for keyboard input
    delay(5);
} 