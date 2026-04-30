/**
  ******************************************************************************
  * @file     main.c
  * @brief    ESP32-C5-Zero UART asynchronous TX/RX task demo
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Initialize UART1 with configurable baud rate and GPIO pins
  * 2. Create independent FreeRTOS TX and RX tasks
  * 3. Send periodic text data over UART from TX task
  * 4. Receive UART data and print text plus hex dump in RX task
  ******************************************************************************
  */
/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdlib.h> 
#include <string.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define UART_PORT   UART_NUM_1
#define TXD_PIN     11
#define RXD_PIN     12
#define BUF_SIZE    1024

static const char *TAG = "UART";

void rx_task(void *arg) {
    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
    if (data == NULL) {
        ESP_LOGE(TAG, "Malloc failed!");
        vTaskDelete(NULL);
        return;
    }
    
    while (1) {
        // Increase read length judgment
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE - 1, pdMS_TO_TICKS(20));
        if (len > 0) {
            data[len] = '\0';
            ESP_LOGI(TAG, "Received %d bytes: '%s'", len, data);

        }
    }
    free(data);
}

void tx_task(void *arg) {
    while (1) {
        const char* msg = "Hello Waveshare\r\n";
        uart_write_bytes(UART_PORT, msg, strlen(msg));
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void) {
    uart_config_t uart_cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_cfg));

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0));

    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    xTaskCreate(rx_task, "uart_rx", 4096, NULL, 10, NULL);
    xTaskCreate(tx_task, "uart_tx", 4096, NULL, 9, NULL);
}