#pragma once

#include "esp_err.h"
#include "esp_zigbee_endpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *manufacturer_name;
    const char *model_identifier;
} zcl_basic_manufacturer_info_t;

esp_err_t esp_zcl_utility_add_ep_basic_manufacturer_info(esp_zb_ep_list_t *ep_list, uint8_t endpoint,
                                                        const zcl_basic_manufacturer_info_t *info);

#ifdef __cplusplus
}
#endif
