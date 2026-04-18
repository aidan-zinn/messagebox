#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "config.h"
#include "ssd1306.h"
#include "servo.h"
#include "wifi.h"
#include "mqtt.h"

static const char *TAG = "messagebox";

static ssd1306_t oled;
static bool s_oled_ok = false;

static void on_message(const char *data, int data_len)
{
    char msg[256];
    int len = data_len < (int)sizeof(msg) - 1 ? data_len : (int)sizeof(msg) - 1;
    memcpy(msg, data, len);
    msg[len] = '\0';

    ESP_LOGI(TAG, "Message: %s", msg);

    if (s_oled_ok) {
        ssd1306_clear(&oled);
        ssd1306_draw_string(&oled, 0, 0, msg);
        ssd1306_flush(&oled);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Messagebox starting");

    // Init OLED (optional — skip if not wired up)
    s_oled_ok = false;
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus;
    if (i2c_new_master_bus(&bus_cfg, &bus) == ESP_OK &&
        ssd1306_init(&oled, bus, OLED_I2C_ADDR) == ESP_OK) {
        s_oled_ok = true;
        ESP_LOGI(TAG, "OLED initialized");
        ssd1306_draw_string(&oled, 0, 0, "Connecting...");
        ssd1306_flush(&oled);
    } else {
        ESP_LOGW(TAG, "OLED not found, continuing without display");
    }

    // Connect to WiFi
    ESP_ERROR_CHECK(wifi_init_sta());

    // Start MQTT
    ESP_ERROR_CHECK(mqtt_start(on_message));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
