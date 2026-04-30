/**
 ******************************************************************************
 * @file     06_WIFI_AP.ino
 * @brief    ESP32-C5-Zero act as WiFi Access Point and monitor client connections
 * @version  V1.1
 * @date     2026-02-02
 ******************************************************************************
 * Features:
 * 1. Create WiFi Access Point with configured SSID and password
 * 2. Monitor client connection/disconnection events via WiFi event handler
 * 3. Print connected device's MAC address, assigned IP address via serial port
 * 4. Provide real-time status feedback for network events
 ******************************************************************************
 */

#include <WiFi.h>

// WiFi Access Point credentials
const char *ssid = "CQ793";       // your AP name
const char *password = "123456789";   // your AP password

/**
 * @brief Handle WiFi AP events.
 */
void on_wifi_event(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      Serial.println("New device connected!");
      Serial.print("MAC Address: ");
      // Use Serial.printf to easily format the 6-byte MAC array in one line
      Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
                    info.wifi_ap_staconnected.mac[0], info.wifi_ap_staconnected.mac[1],
                    info.wifi_ap_staconnected.mac[2], info.wifi_ap_staconnected.mac[3],
                    info.wifi_ap_staconnected.mac[4], info.wifi_ap_staconnected.mac[5]);
      break;

    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      Serial.println("Device disconnected from AP.");
      break;

    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
      Serial.print("Assigned IP to device: ");
      // Convert the IP struct directly to an Arduino IPAddress object and print
      Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
      break;

    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Best practice: Register the event handler BEFORE starting the AP 
  // so you don't miss any immediate connection events.
  WiFi.onEvent(on_wifi_event);

  // Start the Access Point
  Serial.println("Starting WiFi Access Point...");
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Failed to start WiFi AP! Halting system.");
    while (true); // Halt if AP fails to start
  }

  Serial.println("WiFi Access Point initialized successfully!");
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP()); // Also print the AP's own IP address
}

void loop() {
  // Nothing to do here, events are handled asynchronously in the background
  delay(1000); 
}
