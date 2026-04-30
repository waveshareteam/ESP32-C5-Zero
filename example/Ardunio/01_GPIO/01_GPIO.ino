/**
 ******************************************************************************
 * @file     01_GPIO.ino
 * @brief    ESP32-C5-Zero toggle multiple GPIO pins synchronously using registers
 * @version  V1.0
 * @date     2026-02-02
 ******************************************************************************
 * Features:
 * 1. Initialize a list of GPIO pins as outputs
 * 2. Build bitmask for fast register-based GPIO control
 * 3. Toggle all configured pins synchronously using GPIO registers
 * 4. Pins: 0-12, 23-25, 28 (avoid conflicts with other peripherals)
 ******************************************************************************
 */

#include <Arduino.h>

// Explicit list of participating GPIO pins
static const uint8_t gpio_pins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 23, 24, 25, 28};
static constexpr size_t gpio_pin_count = sizeof(gpio_pins) / sizeof(gpio_pins[0]);

static uint32_t gpio_low = 0;
static uint32_t gpio_high = 0;

void setup() {
  gpio_low = 0;
  gpio_high = 0;
  for (size_t i = 0; i < gpio_pin_count; i++) {
    pinMode(gpio_pins[i], OUTPUT);
    const uint32_t pin = (uint32_t)gpio_pins[i];
    if (pin < 32) {
      gpio_low |= (1UL << pin);
    } else if (pin < 64) {
      gpio_high |= (1UL << (pin - 32));
    }
  }

  // Clear all listed pins to LOW
  REG_WRITE(GPIO_OUT_W1TC_REG, gpio_low);
  REG_WRITE(GPIO_OUT1_W1TC_REG, gpio_high);
}

void loop() {
  REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) ^ gpio_low);
  REG_WRITE(GPIO_OUT1_REG, REG_READ(GPIO_OUT1_REG) ^ gpio_high);
  delay(300);
}
