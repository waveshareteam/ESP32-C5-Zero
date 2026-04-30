/**
  ******************************************************************************
  * @file     zcl_utility.c
  * @brief    Zigbee ZCL helper utilities for endpoint basic attributes
  * @version  V1.0
  * @date     2026-03-25
  ******************************************************************************
  * Features:
  * 1. Check whether a target attribute already exists in attribute list
  * 2. Update existing attribute value or add a new one conditionally
  * 3. Locate Basic cluster attribute list for a specified endpoint
  * 4. Fill manufacturer name and model identifier attributes consistently
  ******************************************************************************
  */
#include "zcl_utility.h"

#include "esp_zigbee_attribute.h"
#include "esp_zigbee_cluster.h"
#include "string.h"
#include "zcl/esp_zigbee_zcl_basic.h"
#include "zcl/esp_zigbee_zcl_common.h"

static bool zcl_utility_attr_exists(const esp_zb_attribute_list_t *attr_list, uint16_t attr_id)
{
    const esp_zb_attribute_list_t *it = attr_list;
    while (it) {
        if (it->attribute.id == attr_id) {
            return true;
        }
        it = it->next;
    }
    return false;
}

static esp_err_t zcl_utility_update_or_add_attr(esp_zb_attribute_list_t *attr_list, uint16_t attr_id, const void *value)
{
    if (!attr_list || !value) {
        return ESP_ERR_INVALID_ARG;
    }

    if (zcl_utility_attr_exists(attr_list, attr_id)) {
        return esp_zb_cluster_update_attr(attr_list, attr_id, (void *)value);
    }

    return esp_zb_basic_cluster_add_attr(attr_list, attr_id, (void *)value);
}

esp_err_t esp_zcl_utility_add_ep_basic_manufacturer_info(esp_zb_ep_list_t *ep_list, uint8_t endpoint,
                                                        const zcl_basic_manufacturer_info_t *info)
{
    if (!ep_list || !info) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_zb_cluster_list_t *cluster_list = esp_zb_ep_list_get_ep(ep_list, endpoint);
    if (!cluster_list) {
        return ESP_ERR_NOT_FOUND;
    }

    esp_zb_attribute_list_t *basic_attr_list = esp_zb_cluster_list_get_cluster(
        cluster_list, ESP_ZB_ZCL_CLUSTER_ID_BASIC, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    if (!basic_attr_list) {
        return ESP_ERR_NOT_FOUND;
    }

    esp_err_t err = zcl_utility_update_or_add_attr(basic_attr_list, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,
                                                  info->manufacturer_name);
    if (err != ESP_OK) {
        return err;
    }

    return zcl_utility_update_or_add_attr(basic_attr_list, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,
                                          info->model_identifier);
}
