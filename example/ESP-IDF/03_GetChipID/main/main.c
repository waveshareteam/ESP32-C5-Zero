/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero chip information query and print demo
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Read basic chip information (target, cores, revision)
  * 2. Decode and print available chip features
  * 3. Query flash size and flash type information
  * 4. Read and print default MAC address as unique ID
  ******************************************************************************
  */
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"

static const char *log_tag = "GET_CHIP_ID";

/**
 * @brief Application entry
 *
 * Prints basic SoC information, flash size and default MAC address.
 */
void app_main(void)
{
    ESP_LOGI(log_tag, "--- Chip information example ---");

    /* Get chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    /* Print target and core count */
    ESP_LOGI(log_tag, "Target: %s", CONFIG_IDF_TARGET);
    ESP_LOGI(log_tag, "Cores: %d", chip_info.cores);

    /* Get and print flash size */
    uint32_t flash_size;
    esp_flash_get_size(NULL, &flash_size);
    ESP_LOGI(log_tag, "Flash size: %" PRIu32 " MB (%s)",
           flash_size / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    /* Print revision */
    ESP_LOGI(log_tag, "Revision: %d", chip_info.revision);

    /* Use default MAC address as a unique ID */
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    ESP_LOGI(log_tag, "Unique ID (MAC): %02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    ESP_LOGI(log_tag, "------------------------");

    /* Run forever */
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
