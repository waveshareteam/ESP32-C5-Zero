/**
 ******************************************************************************
 * @file     07_WIFI_DHCP.ino
 * @brief    ESP32-C5-Zero connect to specified Wi-Fi network using DHCP and print IP info
 * @version  V1.1
 * @date     2026-02-02
 ******************************************************************************
 * Features:
 * 1. Connect ESP32-C5-Zero to target Wi-Fi network using provided SSID and password
 * 2. Print connection progress and status via serial monitor
 * 3. Display assigned IP address after successful connection
 * 4. Maintain Wi-Fi connection in loop function
 ******************************************************************************
 */
#include <WiFi.h>

// Wi-Fi network credentials
const char *ssid = "xxx";                // your Wi-Fi SSID
const char *password = "xxx";        // your Wi-Fi password

static volatile bool has_ip = false;

/**
 * @brief Print current station network information.
 * @return void
 */
static void print_net_info()
{
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("MASK: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("GW: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS0: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS1: ");
  Serial.println(WiFi.dnsIP(1));
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

/**
 * @brief Handle WiFi station state events.
 * @param event WiFi event ID.
 * @param info Event-specific payload.
 * @return void
 */
static void on_wifi_event(WiFiEvent_t event, WiFiEventInfo_t info)
{
  (void)info;
  switch (event)
  {
  case ARDUINO_EVENT_WIFI_STA_START:
    Serial.println("WIFI_STA_START");
    break;
  case ARDUINO_EVENT_WIFI_STA_CONNECTED:
    Serial.println("WIFI_STA_CONNECTED");
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    has_ip = true;
    Serial.println("WIFI_STA_GOT_IP");
    print_net_info();
    break;
  case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    has_ip = false;
    Serial.println("WIFI_STA_DISCONNECTED");
    break;
  default:
    break;
  }
}

/**
 * @brief Initialize serial and start DHCP WiFi connection flow.
 * @return void
 */
void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("DHCP start");

  WiFi.onEvent(on_wifi_event);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  Serial.print("SSID: ");
  Serial.println(ssid);

  has_ip = false;
  WiFi.begin(ssid, password);

  const uint32_t deadline = millis() + 20000;
  while (!has_ip)
  {
    if ((int32_t)(millis() - deadline) >= 0)
    {
      break;
    }
    delay(200);
    Serial.print('.');
  }
  Serial.println();

  if (!has_ip) {
    Serial.print("WiFi.status: ");
    Serial.println((int)WiFi.status());
  }
}

/**
 * @brief Maintain WiFi status and retry when disconnected.
 * @return void
 */
void loop() {
  static uint32_t last_retry_ms = 0;
  static bool last_has_ip = false;

  const uint32_t now = millis();

  if (has_ip != last_has_ip)
  {
    last_has_ip = has_ip;
    Serial.print("WiFi.status: ");
    Serial.println((int)WiFi.status());

    if (has_ip) {
      print_net_info();
    }
  }

  if (!has_ip && (uint32_t)(now - last_retry_ms) >= 5000)
  {
    last_retry_ms = now;
    Serial.println("WiFi retry");
    WiFi.disconnect(false);
    delay(10);
    WiFi.begin(ssid, password);
  }

  delay(10);
}
