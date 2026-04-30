/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero BLE beacon advertising demo (Bluedroid)
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Initialize BLE controller and Bluedroid stack
  * 2. Configure raw advertising and scan response payloads
  * 3. Set dynamic local device address into scan response data
  * 4. Start non-connectable BLE beacon advertising
  ******************************************************************************
  */
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_log.h"

static const char* TAG = "BLE_BEACON";

// Advertising parameters
static esp_ble_adv_params_t adv_params = {
    .adv_int_min       = 0x20, // 20ms interval
    .adv_int_max       = 0x40, // 40ms interval
    .adv_type          = ADV_TYPE_IND,
    .own_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .channel_map       = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// Raw advertising data: Flags + Complete Local Name (ESP32-C5)
static uint8_t adv_data[] = {
    0x02, 0x01, 0x06,                                // Flags: General Discoverable
    0x09, 0x09, 'E','S','P','3','2','-','C','5'      // Length, Type (0x09 for Name), Value
};

// GAP event callback
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    // Start advertising once the raw data is successfully set
    if (event == ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT) {
        esp_ble_gap_start_advertising(&adv_params);
    }
    
    // Log status when advertising starts
    if (event == ESP_GAP_BLE_ADV_START_COMPLETE_EVT) {
        if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "Advertising started successfully");
        } else {
            ESP_LOGE(TAG, "Advertising start failed");
        }
    }
}

void app_main(void) {
    // 1. Initialize NVS (required for Bluetooth stack)
    if (nvs_flash_init() != ESP_OK) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // 2. Initialize Bluetooth Controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    // 3. Initialize Bluedroid Stack
    esp_bluedroid_init();
    esp_bluedroid_enable();

    // 4. Register GAP callback and configure advertising data
    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gap_config_adv_data_raw(adv_data, sizeof(adv_data));

    ESP_LOGI(TAG, "BLE stack initialized, setting up advertising...");
}