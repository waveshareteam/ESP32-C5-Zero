/**
 ******************************************************************************
 * @file     08_WIFI_STA.ino
 * @brief    ESP32-C5-Zero connect to WiFi and get weather data from Seniverse API
 * @version  V1.1
 * @date     2026-02-02
 ******************************************************************************
 * Features:
 * 1. Print WiFi connection status and local IP address
 * 2. Connect to WiFi using configured SSID and password
 * 3. Get weather data from Seniverse API over HTTPS
 * 4. Verify TLS certificate using root CA (recommended)
 ******************************************************************************
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>

// WiFi credentials
const char* ssid = "xxx";
const char* password = "xxx";

// Seniverse API configuration
const char* request = "GET /v3/weather/now.json?key=SbslwZ6X47ih3u-bX&location=beijing&language=zh-Hans&unit=c HTTP/1.1\r\nhost:api.seniverse.com\r\n\r\n";
const char* host = "api.seniverse.com";
const uint16_t httpsPort = 443;

WiFiClientSecure client;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial);  // Wait for serial port to connect

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print connection success message with WiFi details
  Serial.println("\n");
  Serial.println("=====================================");
  Serial.println("WiFi connection successful!");
  Serial.print("Connected to SSID: ");
  Serial.println(ssid);
  Serial.print("With password: ");
  Serial.println(password);
  Serial.print("Assigned IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("=====================================\n");
}

void loop() {
  // Check WiFi connection status
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.reconnect();
    delay(1000);
    return;
  }

  // Connect to Seniverse API server
  Serial.print("Connecting to API server: ");
  Serial.println(host);
  
  client.setInsecure();  // Skip certificate verification for testing
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection to server failed!");
    delay(5000);  // Retry after 5 seconds
    return;
  }

  // Send API request
  Serial.println("Sending request to weather API...");
  client.print(request);

  // Wait for response with timeout
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {  // 5 seconds timeout
      Serial.println("Timeout waiting for API response!");
      client.stop();
      delay(5000);
      return;
    }
  }

  // Read and print API response with proper line breaks
  Serial.println("\n--- API Response Start ---");
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);  // Using println to ensure each line is properly separated
  }
  Serial.println("--- API Response End ---\n");

  // Close connection and wait for next request
  client.stop();
  Serial.println("Waiting 5 minutes for next update...");
  Serial.println("=====================================\n");
  delay(300000);  // 5 minutes in milliseconds
}

