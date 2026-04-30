/**
 ******************************************************************************
 * @file     02_RainbowRGB.ino
 * @brief    ESP32-C5-Zero drive WS2812 with smooth Rainbow Wheel effect
 * @version  V1.1
 ******************************************************************************
 */

#include <Arduino.h>

// Built-in ESP32 helper to drive WS2812
extern void neopixelWrite(uint8_t pin, uint8_t red, uint8_t green, uint8_t blue);

static constexpr uint8_t WS2812_PIN = 27;

/**
 * @brief Generate RGB colors based on a position (0-255).
 * This is the direct port of the MicroPython 'wheel' function.
 */
void set_wheel_color(uint8_t pos) {
  uint8_t r, g, b;
  
  if (pos < 85) {
    r = pos * 3;
    g = 255 - pos * 3;
    b = 0;
  } else if (pos < 170) {
    pos -= 85;
    r = 255 - pos * 3;
    g = 0;
    b = pos * 3;
  } else {
    pos -= 170;
    r = 0;
    g = pos * 3;
    b = 255 - pos * 3;
  }
  
  // Write the calculated color to the LED
  neopixelWrite(WS2812_PIN, r, g, b);
}

void setup() {
  pinMode(WS2812_PIN, OUTPUT);
  // Turn off at startup
  neopixelWrite(WS2812_PIN, 0, 0, 0);
}

void loop() {
  static uint8_t wheel_pos = 0;      // Current color position (0-255)
  static uint32_t last_update_ms = 0;

  const uint32_t now = millis();

  if (now - last_update_ms >= 20) {
    last_update_ms = now;

    // Calculate and set the color
    set_wheel_color(wheel_pos);

    // Increment position and wrap around at 255
    wheel_pos++; 
  }
}