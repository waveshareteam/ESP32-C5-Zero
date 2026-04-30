/**
  ******************************************************************************
  * @file     esp_zb_switch.c
  * @brief    ESP32-C5-Zero Zigbee HA On/Off switch coordinator demo
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Initialize Zigbee coordinator stack and On/Off switch endpoint
  * 2. Handle Zigbee commissioning and network formation signals
  * 3. Discover and bind target On/Off light devices dynamically
  * 4. Send On/Off toggle command when local switch event is triggered
  ******************************************************************************
  */

#include "string.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "ha/esp_zigbee_ha_standard.h"
#include "esp_zb_switch.h"

#if defined ZB_ED_ROLE
#error Define ZB_COORDINATOR_ROLE in idf.py menuconfig to compile light switch source code.
#endif
typedef struct light_bulb_device_params_s {
    esp_zb_ieee_addr_t ieee_addr;
    uint8_t  endpoint;
    uint16_t short_addr;
} light_bulb_device_params_t;

typedef struct {
    esp_zb_ieee_addr_t ieee_addr;
    uint8_t endpoint;
    bool used;
} zb_bound_light_t;

static switch_func_pair_t button_func_pair[] = {
    {GPIO_INPUT_IO_TOGGLE_SWITCH, SWITCH_ONOFF_TOGGLE_CONTROL}
};

static const char *log_tag = "ESP_ZB_ON_OFF_SWITCH";

static zb_bound_light_t s_bound_lights[4];

static bool zb_is_light_bound(const esp_zb_ieee_addr_t ieee_addr, uint8_t endpoint)
{
    size_t i = 0;
    while (i < (sizeof(s_bound_lights) / sizeof(s_bound_lights[0]))) {
        if (!s_bound_lights[i].used) {
            i++;
            continue;
        }
        if (s_bound_lights[i].endpoint != endpoint) {
            i++;
            continue;
        }
        if (memcmp(s_bound_lights[i].ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t)) != 0) {
            i++;
            continue;
        }
        return true;
    }
    return false;
}

static void zb_mark_light_bound(const esp_zb_ieee_addr_t ieee_addr, uint8_t endpoint)
{
    if (zb_is_light_bound(ieee_addr, endpoint)) {
        return;
    }
    size_t i = 0;
    while (i < (sizeof(s_bound_lights) / sizeof(s_bound_lights[0]))) {
        if (s_bound_lights[i].used) {
            i++;
            continue;
        }
        memcpy(s_bound_lights[i].ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t));
        s_bound_lights[i].endpoint = endpoint;
        s_bound_lights[i].used = true;
        return;
    }
}

static void zb_buttons_handler(switch_func_pair_t *button_func_pair)
{
    if (button_func_pair->func == SWITCH_ONOFF_TOGGLE_CONTROL) {
        /* implemented light switch toggle functionality */
        esp_zb_zcl_on_off_cmd_t cmd_req;
        cmd_req.zcl_basic_cmd.src_endpoint = HA_ONOFF_SWITCH_ENDPOINT;
        cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
        cmd_req.on_off_cmd_id = ESP_ZB_ZCL_CMD_ON_OFF_TOGGLE_ID;
        esp_zb_lock_acquire(portMAX_DELAY);
        esp_zb_zcl_on_off_cmd_req(&cmd_req);
        esp_zb_lock_release();
        ESP_EARLY_LOGI(log_tag, "Send 'on_off toggle' command");
    }
}

static esp_err_t deferred_driver_init(void)
{
    ESP_RETURN_ON_FALSE(switch_driver_init(button_func_pair, PAIR_SIZE(button_func_pair), zb_buttons_handler), ESP_FAIL, log_tag,
                        "Failed to initialize switch driver");
    return ESP_OK;
}

static void bdb_start_top_level_commissioning_cb(uint8_t mode_mask)
{
    ESP_RETURN_ON_FALSE(esp_zb_bdb_start_top_level_commissioning(mode_mask) == ESP_OK, , log_tag, "Failed to start Zigbee bdb commissioning");
}

static void bind_cb(esp_zb_zdp_status_t zdo_status, void *user_ctx)
{
    if (!user_ctx) {
        return;
    }
    light_bulb_device_params_t *light = (light_bulb_device_params_t *)user_ctx;

    if (zdo_status != ESP_ZB_ZDP_STATUS_SUCCESS) {
        ESP_LOGW(log_tag, "Bind failed (status: 0x%x)", zdo_status);
        free(light);
        return;
    }

    ESP_LOGI(log_tag, "Bound successfully!");
    zb_mark_light_bound(light->ieee_addr, light->endpoint);
    ESP_LOGI(log_tag, "The light originating from address(0x%x) on endpoint(%d)", light->short_addr, light->endpoint);
    free(light);
}

static void user_find_cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx)
{
    if (zdo_status != ESP_ZB_ZDP_STATUS_SUCCESS) {
        return;
    }
    esp_zb_ieee_addr_t ieee_addr;
    esp_zb_ieee_address_by_short(addr, ieee_addr);
    if (zb_is_light_bound(ieee_addr, endpoint)) {
        ESP_LOGI(log_tag, "Found light");
        ESP_LOGI(log_tag, "Skip bind, already bound");
        return;
    }
    ESP_LOGI(log_tag, "Found light");
    esp_zb_zdo_bind_req_param_t bind_req;
    light_bulb_device_params_t *light = (light_bulb_device_params_t *)malloc(sizeof(light_bulb_device_params_t));
    if (!light) {
        ESP_LOGE(log_tag, "No memory for bind context");
        return;
    }
    light->endpoint = endpoint;
    light->short_addr = addr;
    memcpy(light->ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t));
    esp_zb_get_long_address(bind_req.src_address);
    bind_req.src_endp = HA_ONOFF_SWITCH_ENDPOINT;
    bind_req.cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ON_OFF;
    bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
    memcpy(bind_req.dst_address_u.addr_long, light->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    bind_req.dst_endp = endpoint;
    bind_req.req_dst_addr = esp_zb_get_short_address();
    ESP_LOGI(log_tag, "Try to bind On/Off");
    esp_zb_zdo_device_bind_req(&bind_req, bind_cb, (void *)light);
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct)
{
    uint32_t *p_sg_p       = signal_struct->p_app_signal;
    esp_err_t err_status = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = *p_sg_p;
    esp_zb_zdo_signal_device_annce_params_t *dev_annce_params = NULL;
    switch (sig_type) {
    case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(log_tag, "Initialize Zigbee stack");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
        break;
    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
        if (err_status == ESP_OK) {
            ESP_LOGI(log_tag, "Deferred driver initialization %s", deferred_driver_init() ? "failed" : "successful");
            ESP_LOGI(log_tag, "Device started up in %s factory-reset mode", esp_zb_bdb_is_factory_new() ? "" : "non");
            if (esp_zb_bdb_is_factory_new()) {
                ESP_LOGI(log_tag, "Start network formation");
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_FORMATION);
            } else {
                esp_zb_bdb_open_network(180);
                ESP_LOGI(log_tag, "Device rebooted");
            }
        } else {
            ESP_LOGE(log_tag, "Failed to initialize Zigbee stack (status: %s)", esp_err_to_name(err_status));
        }
        break;
    case ESP_ZB_BDB_SIGNAL_FORMATION:
        if (err_status == ESP_OK) {
            esp_zb_ieee_addr_t extended_pan_id;
            esp_zb_get_extended_pan_id(extended_pan_id);
            ESP_LOGI(log_tag, "Formed network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d, Short Address: 0x%04hx)",
                     extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
                     extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
                     esp_zb_get_pan_id(), esp_zb_get_current_channel(), esp_zb_get_short_address());
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
        } else {
            ESP_LOGI(log_tag, "Restart network formation (status: %s)", esp_err_to_name(err_status));
            esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb, ESP_ZB_BDB_MODE_NETWORK_FORMATION, 1000);
        }
        break;
    case ESP_ZB_BDB_SIGNAL_STEERING:
        if (err_status == ESP_OK) {
            ESP_LOGI(log_tag, "Network steering started");
        }
        break;
    case ESP_ZB_ZDO_SIGNAL_DEVICE_ANNCE:
        dev_annce_params = (esp_zb_zdo_signal_device_annce_params_t *)esp_zb_app_signal_get_params(p_sg_p);
        ESP_LOGI(log_tag, "New device commissioned or rejoined (short: 0x%04hx)", dev_annce_params->device_short_addr);
        esp_zb_zdo_match_desc_req_param_t  cmd_req;
        cmd_req.dst_nwk_addr = dev_annce_params->device_short_addr;
        cmd_req.addr_of_interest = dev_annce_params->device_short_addr;
        esp_zb_zdo_find_on_off_light(&cmd_req, user_find_cb, NULL);
        break;
    case ESP_ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS:
        if (err_status == ESP_OK) {
            if (*(uint8_t *)esp_zb_app_signal_get_params(p_sg_p)) {
                ESP_LOGI(log_tag, "Network(0x%04hx) is open for %d seconds", esp_zb_get_pan_id(), *(uint8_t *)esp_zb_app_signal_get_params(p_sg_p));
            } else {
                ESP_LOGW(log_tag, "Network(0x%04hx) closed, devices joining not allowed.", esp_zb_get_pan_id());
            }
        }
        break;
    default:
        ESP_LOGI(log_tag, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type,
                 esp_err_to_name(err_status));
        break;
    }
}

/**
 * @brief Zigbee main task
 *
 * Initializes the Zigbee stack, creates an On/Off switch endpoint,
 * registers the device and enters the Zigbee main loop.
 */
static void esp_zb_task(void *pvParameters)
{
    /* initialize Zigbee stack */
    esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZC_CONFIG();
    esp_zb_init(&zb_nwk_cfg);
    esp_zb_on_off_switch_cfg_t switch_cfg = ESP_ZB_DEFAULT_ON_OFF_SWITCH_CONFIG();
    esp_zb_ep_list_t *esp_zb_on_off_switch_ep = esp_zb_on_off_switch_ep_create(HA_ONOFF_SWITCH_ENDPOINT, &switch_cfg);
    zcl_basic_manufacturer_info_t info = {
        .manufacturer_name = ESP_MANUFACTURER_NAME,
        .model_identifier = ESP_MODEL_IDENTIFIER,
    };

    esp_zcl_utility_add_ep_basic_manufacturer_info(esp_zb_on_off_switch_ep, HA_ONOFF_SWITCH_ENDPOINT, &info);
    esp_zb_device_register(esp_zb_on_off_switch_ep);
    esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);
    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

/**
 * @brief Application entry point
 *
 * Initializes NVS/ Zigbee platform and starts the Zigbee main task.
 */
void app_main(void)
{
    esp_zb_platform_config_t config = {
        .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
    };
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_zb_platform_config(&config));

    xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);
}
