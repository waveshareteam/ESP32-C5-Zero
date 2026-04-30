/**
 ******************************************************************************
 * @file     05_UART.ino
 * @brief    ESP32-C5-Zero UART loopback example (TX: GPIO 11, RX: GPIO 12)
 * @version  V1.0
 * @date     2026-02-02
 ******************************************************************************
 * Features:
 * 1. Configure Serial1 with custom TX/RX GPIO pins
 * 2. Echo back all received bytes on the same UART port
 * 3. Demonstrate basic UART loopback test on ESP32-C5-Zero
 * 4. Keep main loop responsive with a small delay
 ******************************************************************************
 */

#include <Arduino.h>

#define RX_PIN 12
#define TX_PIN 11
#define BAUDRATE 115200

void setup() {
  Serial1.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial1.println("UART Initialized. TX:11, RX:12");
}

void loop() {
  while (Serial1.available() > 0) {
    Serial1.write(Serial1.read());
  }
  delay(1);
}
