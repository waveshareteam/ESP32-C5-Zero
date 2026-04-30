/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero memory/security verification demo entry
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Run integrated security demo flow through demo_start()
  * 2. Execute selected checks based on compile-time feature switches
  * 3. Keep system alive to observe runtime security test output
  * 4. Provide a minimal entry point for memory safety verification demos
  ******************************************************************************
  */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mem_security.h"

/**
 * @brief Main application entry, runs security demo and keeps task alive
 */
void app_main(void)
{
    demo_start();

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
