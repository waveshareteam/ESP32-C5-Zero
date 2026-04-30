/**
 ******************************************************************************
 * @file     04_BLE.ino
 * @brief    ESP32-C5-Zero start simple BLE advertising with device name
 * @version  V1.0
 * @date     2026-02-02
 ******************************************************************************
 * Features:
 * 1. Initialize NimBLE stack on ESP32-C5-Zero
 * 2. Configure scan response data with custom device name
 * 3. Start BLE advertising so phones can discover the board
 * 4. Print advertising status via serial monitor
 ******************************************************************************
 */

#include <NimBLEDevice.h>

void setup() {
  Serial.begin(115200);

  NimBLEDevice::init("ESP32-C5-Zero");

  NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();

  NimBLEAdvertisementData scan_response;
  scan_response.setName("ESP32-C5-Zero");   // This is the name shown on the phone
  advertising->setScanResponseData(scan_response);

  const bool ok = advertising->start();
  if (ok) {
    Serial.println("BLE advertising started");
  } else {
    Serial.println("BLE advertising start failed");
  }
}

void loop() {
  delay(10);
}
