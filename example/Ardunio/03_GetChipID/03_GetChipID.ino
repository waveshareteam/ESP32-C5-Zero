/**
 ******************************************************************************
 * @file     03_GetChipID.ino
 * @brief    Derive 32-bit chip ID from ESP32-C5-Zero MAC address
 * @version  V1.0
 * @date     2026-02-02
 ******************************************************************************
 * Features:
 * 1. Read full 48-bit ESP32-C5-Zero MAC address from efuse
 * 2. Compress MAC into a 32-bit chip ID
 * 3. Print chip model, revision, core count and chip ID via serial
 * 4. Provide a simple unique identifier for switch...case or logging
 ******************************************************************************
 */

void setup() {
  Serial.begin(115200);
}

void loop() {
  const uint64_t mac = ESP.getEfuseMac();
  uint32_t chip_id = 0;
  for (uint8_t i = 0; i < 24; i = (uint8_t)(i + 8)) {
    chip_id |= (uint32_t)(((mac >> (40 - i)) & 0xff) << i);
  }

  Serial.printf("ESP32-C5-Zero Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  Serial.print("Chip ID: ");
  Serial.println(chip_id);
  delay(3000);
}
