#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "config.h"
#include "ssd1306.h"
#include "servo.h"

static const char *TAG = "messagebox";

static ssd1306_t oled;

void app_main(void)
{
    ESP_LOGI(TAG, "Messagebox starting");

    // Init I2C bus
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));

    // Init OLED
    ESP_ERROR_CHECK(ssd1306_init(&oled, bus, OLED_I2C_ADDR));
    ESP_LOGI(TAG, "OLED initialized");

    ssd1306_draw_string(&oled, 0, 0, "Hello World");
    ssd1306_draw_string(&oled, 0, 16, "Hello World");
    ESP_ERROR_CHECK(ssd1306_flush(&oled));

    // Init servo
    ESP_ERROR_CHECK(servo_init(PIN_SERVO));
    ESP_LOGI(TAG, "Servo initialized");

    // Test: spin CW for 2 seconds, then stop
    ESP_LOGI(TAG, "Servo test: spinning...");
    servo_set_pulse_us(1000);
    vTaskDelay(pdMS_TO_TICKS(2000));
    servo_stop();
    ESP_LOGI(TAG, "Servo test done");

    int count = 0;
    while (1) {
        ESP_LOGI(TAG, "heartbeat %d", count++);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
