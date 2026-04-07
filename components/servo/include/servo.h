#pragma once

#include "esp_err.h"
#include <stdint.h>

esp_err_t servo_init(int gpio);
esp_err_t servo_set_pulse_us(uint32_t pulse_us);
void servo_stop(void);
