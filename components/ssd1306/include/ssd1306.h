#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"
#include <stdbool.h>

#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 64
#define SSD1306_BUF_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

typedef struct {
    i2c_master_dev_handle_t i2c_dev;
    uint8_t buf[SSD1306_BUF_SIZE];
} ssd1306_t;

esp_err_t ssd1306_init(ssd1306_t *dev, i2c_master_bus_handle_t bus, uint8_t addr);

void ssd1306_clear(ssd1306_t *dev);
void ssd1306_draw_pixel(ssd1306_t *dev, int x, int y, bool on);
void ssd1306_draw_char(ssd1306_t *dev, int x, int y, char c);
void ssd1306_draw_string(ssd1306_t *dev, int x, int y, const char *str);

esp_err_t ssd1306_flush(ssd1306_t *dev);
