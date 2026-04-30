/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero toggle multiple GPIO pins with readable GPIO API code
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Configure a list of GPIO pins as outputs
  * 2. Build a pin bitmask for output mode configuration
  * 3. Toggle selected pins together using gpio_set_level in a loop
  * 4. Keep implementation simple and easy to read
  ******************************************************************************
  */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// GPIO pins to toggle: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 23, 24, 25, 28
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<0)  | (1ULL<<1)  | (1ULL<<2)  | (1ULL<<3)  | \
                              (1ULL<<4)  | (1ULL<<5)  | (1ULL<<6)  | (1ULL<<7)  | \
                              (1ULL<<8)  | (1ULL<<9)  | (1ULL<<10) | (1ULL<<11) | \
                              (1ULL<<12) | (1ULL<<23) | (1ULL<<24) | (1ULL<<25) | \
                              (1ULL<<28))

static const char *log_tag = "GPIO_TOGGLE";
static const gpio_num_t gpio_pins[] = {
    GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3,
    GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7,
    GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11,
    GPIO_NUM_12, GPIO_NUM_23, GPIO_NUM_24, GPIO_NUM_25,
    GPIO_NUM_28
};

/**
 * @brief Application entry point
 *
 * Configures selected GPIOs as outputs and toggles them periodically.
 */
void app_main(void)
{
    // Configure all selected GPIOs as push-pull outputs
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    int output_level = 0;
    size_t gpio_count = sizeof(gpio_pins) / sizeof(gpio_pins[0]);
    
    ESP_LOGI(log_tag, "Initialization done. Start toggling GPIO pins synchronously...");

    while (true) {
        // Set all selected pins to the same level
        for (size_t i = 0; i < gpio_count; i++) {
            ESP_ERROR_CHECK(gpio_set_level(gpio_pins[i], output_level));
        }

        // Flip output level for next cycle
        output_level = !output_level;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
