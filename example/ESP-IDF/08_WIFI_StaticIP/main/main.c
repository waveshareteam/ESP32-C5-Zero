/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero WiFi station demo with static IPv4 configuration
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Initialize WiFi station interface and event handlers
  * 2. Disable DHCP client and set static IP/gateway/netmask
  * 3. Connect to AP with configured SSID/password and retry policy
  * 4. Report final connection result via event group bits
  ******************************************************************************
  */
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/inet.h"

#define SSID "xxx"
#define PWD  "xxx"

static const char *TAG = "WIFI";

// Simple event handler
static void handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Retry connecting...");
        esp_wifi_connect();
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* ev = (ip_event_got_ip_t*) data;
        ESP_LOGI(TAG, "Connected! IP: " IPSTR, IP2STR(&ev->ip_info.ip));
    }
}

void app_main(void) {
    // 1. Quick NVS Init
    if (nvs_flash_init() != ESP_OK) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // 2. Network Stack & Event Loop
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();

    // 3. Set Static IP
    esp_netif_dhcpc_stop(netif);
    esp_netif_ip_info_t ip_info = {
        .ip.addr = ipaddr_addr("192.168.1.77"),
        .gw.addr = ipaddr_addr("192.168.1.1"),
        .netmask.addr = ipaddr_addr("255.255.255.0")
    };
    esp_netif_set_ip_info(netif, &ip_info);
    ESP_LOGI(TAG, "Static IP applied: ip=" IPSTR ", gw=" IPSTR ", mask=" IPSTR,
             IP2STR(&ip_info.ip), IP2STR(&ip_info.gw), IP2STR(&ip_info.netmask));

    // 4. WiFi Init with minimal config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // Register all-in-one handler
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &handler, NULL, NULL);

    // 5. Config SSID/PWD and Start
    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = SSID,
            .password = PWD,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    esp_wifi_start();

    ESP_LOGI(TAG, "WiFi static IP config done.");
}
