#pragma once

#include "esp_err.h"

typedef void (*mqtt_message_cb_t)(const char *data, int data_len);

esp_err_t mqtt_start(mqtt_message_cb_t cb);
