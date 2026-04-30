/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero WS2812 RGB blink demo using RMT peripheral
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Initialize one WS2812 LED strip channel
  * 2. Configure RMT timing parameters for LED signaling
  * 3. Set RGB pixel values and refresh output repeatedly
  * 4. Demonstrate periodic color switching (red/green/blue)
  ******************************************************************************
  */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"

#define WS2812_PIN 27

/**
 * @brief Rainbow conversion function
 */
void led_strip_set_wheel(led_strip_handle_t led_strip, uint8_t pos) {
    uint32_t r, g, b;
    if (pos < 85) {
        r = pos * 3; g = 255 - pos * 3; b = 0;
    } else if (pos < 170) {
        pos -= 85;
        r = 255 - pos * 3; g = 0; b = pos * 3;
    } else {
        pos -= 170;
        r = 0; g = pos * 3; b = 255 - pos * 3;
    }
    // IDF's led_strip_set_pixel accepts R, G, B parameters
    led_strip_set_pixel(led_strip, 0, r, g, b);
}

void app_main(void)
{
    /* 1. Init LED Strip (keep your configuration) */
    led_strip_config_t strip_config = {
        .strip_gpio_num = WS2812_PIN,
        .max_leds = 1,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .led_model = LED_MODEL_WS2812,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };

    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    uint8_t wheel_pos = 0;

    /* 2. Main loop: smooth gradient */
    while (true) {
        // Calculate current color
        led_strip_set_wheel(led_strip, wheel_pos);
        
        // Refresh LED
        led_strip_refresh(led_strip);

        // Increment position (uint8_t automatically wraps to 0 after 255)
        wheel_pos++;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}