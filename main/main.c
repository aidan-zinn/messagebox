#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "config.h"

static const char *TAG = "messagebox";

void app_main(void)
{
    ESP_LOGI(TAG, "Messagebox starting");
    ESP_LOGI(TAG, "Pins: SDA=%d SCL=%d Servo=%d LDR=%d",
             PIN_I2C_SDA, PIN_I2C_SCL, PIN_SERVO, PIN_LDR);

    int count = 0;
    while (1) {
        ESP_LOGI(TAG, "hello world %d", count++);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
