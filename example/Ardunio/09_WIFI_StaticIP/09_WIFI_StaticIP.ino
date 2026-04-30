/**
 ******************************************************************************
 * @file     09_WIFI_StaticIP.ino
 * @brief    ESP32-C5-Zero connect to Wi-Fi with static IP configuration
 * @version  V1.2
 * @date     2026-02-02
 ******************************************************************************
 * Features:
 * 1. Connect ESP32-C5-Zero to configured Wi-Fi network with static IP
 * 2. Configure static IP, gateway, subnet mask, and DNS matching network settings
 * 3. Print connection status and network info via serial monitor
 * 4. Auto-reconnect if Wi-Fi connection drops
 ******************************************************************************
 */

#include <WiFi.h>

// Wi-Fi credentials
const char *ssid = "xxx";                // your Wi-Fi SSID 
const char *password = "xxx";        // your Wi-Fi password

// Static IP configuration (matched to screenshot's network parameters)
IPAddress local_ip(192, 168, 9, 100);   // Static IP for ESP32-C5-Zero 
IPAddress gateway(192, 168, 11, 1);    // Gateway IP 
IPAddress subnet(255, 255, 252, 0);    // Subnet mask 
IPAddress dns(192, 168, 11, 1);        // DNS server 
const uint32_t serial_wait_timeout_ms = 2000;

void setup() {
  // Initialize serial communication (115200 baud rate)
  Serial.begin(115200);
  const uint32_t serial_start_ms = millis();
  while (!Serial && (uint32_t)(millis() - serial_start_ms) < serial_wait_timeout_ms) {
    delay(10);
  }

  // Set Wi-Fi mode to Station (client mode)
  WiFi.mode(WIFI_STA);

  // Apply static IP configuration
  if (!WiFi.config(local_ip, gateway, subnet, dns)) {
    Serial.println("WiFi.config failed");
  }

  // Start Wi-Fi connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi with static IP...");

  // Wait for connection with retry logic
  const uint32_t connect_start_ms = millis();
  uint32_t last_dot_ms = connect_start_ms;
  while (WiFi.status() != WL_CONNECTED && (uint32_t)(millis() - connect_start_ms) < 15000) {
    const uint32_t now = millis();
    if ((uint32_t)(now - last_dot_ms) >= 1000) {
      last_dot_ms = now;
      Serial.print('.');
    }
    delay(10);
  }

  // Print connection result
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nSuccessfully connected to Wi-Fi!");
    Serial.print("Static IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway: ");
    Serial.println(gateway);
    Serial.print("Subnet Mask: ");
    Serial.println(subnet);
  } else {
    Serial.println("\nFailed to connect to Wi-Fi!");
    Serial.print("WiFi Status Code: ");
    Serial.println(WiFi.status()); // Print code for debugging (e.g., 6 = AUTH_FAIL)
  }
}

void loop() {
  static uint32_t last_check_ms = 0;
  const uint32_t now = millis();
  if ((uint32_t)(now - last_check_ms) < 5000) {
    delay(10);
    return;
  }
  last_check_ms = now;

  // Periodically check Wi-Fi connection and auto-reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected, attempting reconnection...");
    WiFi.begin(ssid, password); // Re-initiate connection
    uint8_t retry = 0;
    uint32_t retry_start_ms = millis();
    uint32_t retry_dot_ms = retry_start_ms;
    while (WiFi.status() != WL_CONNECTED && retry < 5) {
      const uint32_t now2 = millis();
      if ((uint32_t)(now2 - retry_dot_ms) >= 1000) {
        retry_dot_ms = now2;
        Serial.print('.');
        retry++;
      }
      delay(10);
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nReconnected to Wi-Fi!");
      Serial.print("Current IP: ");
      Serial.println(WiFi.localIP());
    }
  }
  delay(10);
}
