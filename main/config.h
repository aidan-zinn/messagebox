#pragma once

// I2C (OLED)
#define PIN_I2C_SDA         4
#define PIN_I2C_SCL         5
#define OLED_I2C_ADDR       0x3C
#define OLED_I2C_FREQ_HZ    400000

// Servo
#define PIN_SERVO           10
#define SERVO_PULSE_STOP_US 1500

// LDR
#define PIN_LDR             3   // ADC1_CH3
#define LDR_THRESHOLD       2000
