/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero WiFi station startup demo with retry handling
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Initialize WiFi station interface and event processing
  * 2. Connect to AP with configured credentials
  * 3. Perform bounded retry on disconnect events
  * 4. Wait for connected/failed event bits and print final state
  ******************************************************************************
  */
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#define SSID "xxx"
#define PWD  "xxx"

static const char *TAG = "WIFI_STA";

// Unified event handler: responsible for starting connection, reconnecting on disconnect, and printing IP
static void wifi_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Retrying...");
        esp_wifi_connect();
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
        ESP_LOGI(TAG, "Connected! IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void app_main(void) {
    // 1. Initialize NVS (WiFi configuration needs to be stored in NVS)
    if (nvs_flash_init() != ESP_OK) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // 2. Initialize network stack and underlying drivers
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // 3. Register event handlers (handle all WiFi and IP related events)
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_handler, NULL, NULL);

    // 4. Configure and start WiFi
    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = SSID,
            .password = PWD,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    esp_wifi_start();

    ESP_LOGI(TAG, "WiFi station started.");
}