/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero WiFi soft-AP startup and station event demo
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Initialize WiFi in AP mode with configurable SSID/password
  * 2. Register AP station connect/disconnect event handler
  * 3. Apply AP security mode and channel settings
  * 4. Start soft-AP and keep the system alive in a FreeRTOS loop
  ******************************************************************************
  */
  
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"

#define SSID "xxx"
#define PWD  "xxx"  // Must be at least 8 characters

static const char *TAG = "WIFI_AP";

// Minimal AP event handler for station join/leave notifications.
static void wifi_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    switch (id) {
    case WIFI_EVENT_AP_STACONNECTED: {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) data;
        ESP_LOGI(TAG, "New Station! MAC: " MACSTR, MAC2STR(event->mac));
        break;
    }
    case WIFI_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "Station left.");
        break;
    default:
        break;
    }
}

void app_main(void) {
    // 1. Initialize NVS (required for AP mode as well).
    if (nvs_flash_init() != ESP_OK) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // 2. Initialize network stack and create the default AP netif.
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    // 3. Initialize WiFi driver and register event handler.
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_handler, NULL, NULL);

    // 4. Configure AP parameters with designated initializer.
    wifi_config_t ap_cfg = {
        .ap = {
            .ssid = SSID,
            .password = PWD,
            .ssid_len = 0,             // Use null-terminated SSID string length.
            .channel = 1,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };
    
    // Switch to open mode when password is empty.
    if (PWD[0] == '\0') ap_cfg.ap.authmode = WIFI_AUTH_OPEN;

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_cfg);
    esp_wifi_start();

    ESP_LOGI(TAG, "AP Mode started. SSID: %s", SSID);
}
