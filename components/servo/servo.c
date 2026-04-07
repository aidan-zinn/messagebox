#include "servo.h"
#include "driver/ledc.h"

#define SERVO_TIMER       LEDC_TIMER_0
#define SERVO_CHANNEL     LEDC_CHANNEL_0
#define SERVO_FREQ_HZ     50
#define SERVO_RESOLUTION  LEDC_TIMER_14_BIT
#define SERVO_MAX_DUTY    16384  // 2^14
#define SERVO_PERIOD_US   20000  // 1/50Hz = 20ms

static int servo_gpio;

esp_err_t servo_init(int gpio)
{
    servo_gpio = gpio;

    ledc_timer_config_t timer_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = SERVO_TIMER,
        .duty_resolution = SERVO_RESOLUTION,
        .freq_hz = SERVO_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t ret = ledc_timer_config(&timer_cfg);
    if (ret != ESP_OK) return ret;

    ledc_channel_config_t ch_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = SERVO_CHANNEL,
        .timer_sel = SERVO_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = gpio,
        .duty = 0,
        .hpoint = 0,
    };
    return ledc_channel_config(&ch_cfg);
}

esp_err_t servo_set_pulse_us(uint32_t pulse_us)
{
    uint32_t duty = pulse_us * SERVO_MAX_DUTY / SERVO_PERIOD_US;
    esp_err_t ret = ledc_set_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL, duty);
    if (ret != ESP_OK) return ret;
    return ledc_update_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL);
}

void servo_stop(void)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SERVO_CHANNEL);
}
