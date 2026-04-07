#pragma once

#include <stdint.h>

#define FONT_WIDTH  5
#define FONT_HEIGHT 7

// 5x7 font, ASCII 32-126. Each character is 5 bytes, one per column,
// LSB is top row. Only the lower 7 bits of each byte are used.
extern const uint8_t font_5x7[][FONT_WIDTH];
