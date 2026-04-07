#pragma once

#include "esp_err.h"

// Blocking: inits WiFi STA, connects, waits for IP. Returns ESP_OK or error.
esp_err_t wifi_init_sta(void);
