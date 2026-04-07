#include "ssd1306.h"
#include "ssd1306_font.h"
#include <string.h>

#define SSD1306_CMD  0x00
#define SSD1306_DATA 0x40

static esp_err_t ssd1306_write_cmd(ssd1306_t *dev, uint8_t cmd)
{
    uint8_t buf[2] = {SSD1306_CMD, cmd};
    return i2c_master_transmit(dev->i2c_dev, buf, sizeof(buf), 100);
}

esp_err_t ssd1306_init(ssd1306_t *dev, i2c_master_bus_handle_t bus, uint8_t addr)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 400000,
    };
    esp_err_t ret = i2c_master_bus_add_device(bus, &dev_cfg, &dev->i2c_dev);
    if (ret != ESP_OK) return ret;

    // Init sequence
    const uint8_t cmds[] = {
        0xAE,       // display off
        0xD5, 0x80, // clock divide ratio / oscillator frequency
        0xA8, 0x3F, // MUX ratio = 63 (64 lines)
        0xD3, 0x00, // display offset = 0
        0x40,       // display start line = 0
        0x8D, 0x14, // charge pump on
        0x20, 0x00, // horizontal addressing mode
        0xA1,       // segment remap (column 127 = SEG0)
        0xC8,       // COM scan direction remapped
        0xDA, 0x12, // COM pins config for 128x64
        0x81, 0x7F, // contrast = 127
        0xA4,       // display from RAM
        0xA6,       // normal display (not inverted)
        0xAF,       // display on
    };

    for (size_t i = 0; i < sizeof(cmds); i++) {
        ret = ssd1306_write_cmd(dev, cmds[i]);
        if (ret != ESP_OK) return ret;
    }

    ssd1306_clear(dev);
    return ssd1306_flush(dev);
}

void ssd1306_clear(ssd1306_t *dev)
{
    memset(dev->buf, 0, SSD1306_BUF_SIZE);
}

void ssd1306_draw_pixel(ssd1306_t *dev, int x, int y, bool on)
{
    if (x < 0 || x >= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT) return;

    // Each byte in the buffer is a vertical 8-pixel strip.
    // Page = y / 8, bit position = y % 8.
    int idx = x + (y / 8) * SSD1306_WIDTH;
    if (on)
        dev->buf[idx] |= (1 << (y & 7));
    else
        dev->buf[idx] &= ~(1 << (y & 7));
}

void ssd1306_draw_char(ssd1306_t *dev, int x, int y, char c)
{
    if (c < 32 || c > 126) c = '?';

    const uint8_t *glyph = font_5x7[c - 32];
    for (int col = 0; col < FONT_WIDTH; col++) {
        uint8_t line = glyph[col];
        for (int row = 0; row < FONT_HEIGHT; row++) {
            ssd1306_draw_pixel(dev, x + col, y + row, line & (1 << row));
        }
    }
}

void ssd1306_draw_string(ssd1306_t *dev, int x, int y, const char *str)
{
    int cx = x;
    while (*str) {
        if (*str == '\n') {
            cx = x;
            y += FONT_HEIGHT + 1;
            str++;
            continue;
        }
        ssd1306_draw_char(dev, cx, y, *str);
        cx += FONT_WIDTH + 1; // 1px spacing between chars
        str++;
    }
}

esp_err_t ssd1306_flush(ssd1306_t *dev)
{
    // Set column address 0-127, page address 0-7
    ssd1306_write_cmd(dev, 0x21); // column addr
    ssd1306_write_cmd(dev, 0);    // start
    ssd1306_write_cmd(dev, 127);  // end
    ssd1306_write_cmd(dev, 0x22); // page addr
    ssd1306_write_cmd(dev, 0);    // start
    ssd1306_write_cmd(dev, 7);    // end

    // Send framebuffer as one data write.
    // First byte is the data control byte (0x40).
    uint8_t data[1 + SSD1306_BUF_SIZE];
    data[0] = SSD1306_DATA;
    memcpy(&data[1], dev->buf, SSD1306_BUF_SIZE);
    return i2c_master_transmit(dev->i2c_dev, data, sizeof(data), 200);
}
