/**
  ******************************************************************************
  * @file     security_demo.c
  * @brief    Integrated security demo implementation for ESP32-C5
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Print compile-time and run-time security status
  * 2. Print privilege-related CSR information
  * 3. Run SHA256 verification and TEE/APM capability checks
  * 4. Trigger optional PMP execute-fault validation
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_secure_boot.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "mbedtls/sha256.h"
#include "mem_security.h"

static const char *status_log_tag = "STATUS";
static const char *crypto_log_tag = "CRYPTO";
static const char *tee_apm_log_tag = "TEE_APM";
static const char *pmp_log_tag = "PMP";

bool esp_flash_encryption_enabled(void);

#if defined(CONFIG_SECURE_BOOT)
static const int cfg_secure_boot = 1;
#else
static const int cfg_secure_boot = 0;
#endif

#if defined(CONFIG_SECURE_FLASH_ENC_ENABLED)
static const int cfg_flash_enc = 1;
#else
static const int cfg_flash_enc = 0;
#endif

#if defined(CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT)
static const int cfg_pmp_idram_split = 1;
#else
static const int cfg_pmp_idram_split = 0;
#endif

#if defined(CONFIG_SOC_APM_SUPPORTED)
static const int soc_apm_supported = 1;
#else
static const int soc_apm_supported = 0;
#endif

#if defined(CONFIG_SOC_APM_SUPPORT_LP_TEE_CTRL)
static const int soc_apm_lp_tee_ctrl = 1;
#else
static const int soc_apm_lp_tee_ctrl = 0;
#endif

#if defined(CONFIG_SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL)
static const int soc_apm_tee_peri_access_ctrl = 1;
#else
static const int soc_apm_tee_peri_access_ctrl = 0;
#endif

#if defined(CONFIG_ESP_TEE)
static const int cfg_tee_build = 1;
#else
static const int cfg_tee_build = 0;
#endif

#if defined(CONFIG_ESP_TEE_ENABLED)
static const int cfg_tee_enabled = 1;
#else
static const int cfg_tee_enabled = 0;
#endif

static void print_privilege(void)
{
#if defined(__riscv)
    uint32_t mstatus = 0;
    asm volatile("csrr %0, mstatus" : "=r"(mstatus));
    ESP_LOGI(status_log_tag, "riscv_csr_mstatus=0x%" PRIx32, mstatus);
#else
    ESP_LOGI(status_log_tag, "riscv_csr_mstatus=unsupported");
#endif
}

static void print_status(void)
{
    uint32_t flash_size = 0;
    esp_err_t ret = esp_flash_get_size(NULL, &flash_size);
    size_t spiram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    bool psram_enc = false;

#if defined(CONFIG_SPIRAM_FETCH_INSTRUCTIONS) || defined(CONFIG_SPIRAM_RODATA)
    psram_enc = esp_flash_encryption_enabled();
#endif

    ESP_LOGI(status_log_tag, "compile_time: secure_boot=%d flash_enc=%d pmp_idram_split=%d",
             cfg_secure_boot,
             cfg_flash_enc,
             cfg_pmp_idram_split);
    ESP_LOGI(status_log_tag, "flash_size=%" PRIu32 " err=0x%x",
             (ret == ESP_OK) ? flash_size : 0,
             (unsigned)ret);
    ESP_LOGI(status_log_tag, "flash_encryption_enabled=%d", esp_flash_encryption_enabled() ? 1 : 0);
    ESP_LOGI(status_log_tag, "secure_boot_enabled=%d", esp_secure_boot_enabled() ? 1 : 0);
    ESP_LOGI(status_log_tag, "spiram_total=%" PRIu32 " encryption=%d", (uint32_t)spiram_total, psram_enc ? 1 : 0);
    ESP_LOGI(status_log_tag, "apm_soc=%d lp_tee_ctrl=%d tee_peri_ctrl=%d tee_build=%d tee_enabled=%d",
             soc_apm_supported,
             soc_apm_lp_tee_ctrl,
             soc_apm_tee_peri_access_ctrl,
             cfg_tee_build,
             cfg_tee_enabled);
}

static void test_hw_crypto(void)
{
    const char *input = "ESP32-C5 Security Verification";
    unsigned char output[32];
    int ret = mbedtls_sha256((const unsigned char *)input, strlen(input), output, 0);

    ESP_LOGI(crypto_log_tag, "hw_crypto_test: SHA256_ret=%d", ret);
    if (ret == 0) {
        ESP_LOGI(crypto_log_tag,
                 "hw_crypto_test: SHA256_result=%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "...",
                 output[0], output[1], output[2], output[3],
                 output[4], output[5], output[6], output[7]);
    }
}

static void check_tee_world(void)
{
#if defined(CONFIG_SOC_APM_SUPPORTED)
    ESP_LOGI(tee_apm_log_tag, "tee_check: SOC_APM_SUPPORTED=y");
#if defined(CONFIG_ESP_TEE_ENABLED)
    ESP_LOGI(tee_apm_log_tag, "tee_check: ESP_TEE_ENABLED=y (Running in TEE environment)");
#else
    ESP_LOGI(tee_apm_log_tag, "tee_check: ESP_TEE_ENABLED=n (Running in Standard environment)");
#endif
#else
    ESP_LOGI(tee_apm_log_tag, "tee_check: SOC_APM_SUPPORTED=n");
#endif
}

static void test_tee_apm(void)
{
#if defined(CONFIG_SOC_APM_SUPPORTED)
    ESP_LOGI(tee_apm_log_tag, "apm_test: Attempting to probe APM enforcement...");
    ESP_LOGI(tee_apm_log_tag, "apm_test: APM hardware is present. Detailed isolation requires TEE firmware.");
#else
    ESP_LOGI(tee_apm_log_tag, "apm_test: APM not supported on this SoC.");
#endif

    check_tee_world();
}

static void pmp_exec_fault_once(void)
{
    void *buf = heap_caps_malloc(64, MALLOC_CAP_8BIT);
    uint32_t inst = 0x00008067;

    if (buf == NULL) {
        ESP_LOGE(pmp_log_tag, "pmp_exec_fault: malloc failed");
        return;
    }

    memcpy(buf, &inst, sizeof(inst));
    ((void (*)(void))buf)();

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void demo_start(void)
{
    print_status();
    print_privilege();
    test_hw_crypto();
    test_tee_apm();

#if ENABLE_FAULT_INJECT
    vTaskDelay(pdMS_TO_TICKS(1000));
    pmp_exec_fault_once();
#endif
}
