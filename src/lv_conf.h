#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888) */
#define LV_COLOR_DEPTH 16

/* Use a custom tick source instead of the built-in one (which is HAL_GetTick() for STM32) */
#define LV_TICK_CUSTOM 1

#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/* 1: Draw complex widgets. Might be slower but the widgets look better */
#define LV_DRAW_COMPLEX 1

/* Size of the memory used by `lv_mem_alloc` in bytes (>= 2kB)*/
#define LV_MEM_SIZE (32U * 1024U)

/* Set an address for the memory pool instead of allocating it as a normal array */
#define LV_MEM_CUSTOM 0

/* Use the standard `memcpy` and `memset` instead of LVGL's own functions */
#define LV_MEMCPY_MEMSET_STD 0

/* Enable all widgets */
#define LV_USE_ANIMIMG      1
#define LV_USE_ARC         1
#define LV_USE_BAR         1
#define LV_USE_BTN         1
#define LV_USE_BTNMATRIX   1
#define LV_USE_CANVAS      1
#define LV_USE_CHECKBOX    1
#define LV_USE_DROPDOWN    1
#define LV_USE_IMG         1
#define LV_USE_LABEL       1
#define LV_USE_LINE        1
#define LV_USE_ROLLER      1
#define LV_USE_SLIDER      1
#define LV_USE_SWITCH      1
#define LV_USE_TEXTAREA    1
#define LV_USE_TABLE       1

/* Required for widgets */
#define LV_USE_ANIMATION   1
#define LV_USE_SHADOW      1
#define LV_USE_GROUP       1
#define LV_USE_FLEX        1
#define LV_USE_GRID        1
#define LV_USE_THEME_DEFAULT 1

/* Enable GPU */
#define LV_USE_GPU_ESP32 1

/* Default display refresh period */
#define LV_DISP_DEF_REFR_PERIOD 16

/* Default Dot Per Inch */
#define LV_DPI_DEF 130

/* Maximal horizontal and vertical resolution */
#define LV_HOR_RES_MAX 240
#define LV_VER_RES_MAX 135

/* Enable anti-aliasing (lines, and radiuses will be smoothed) */
#define LV_ANTIALIAS 1

#endif /*LV_CONF_H*/ 