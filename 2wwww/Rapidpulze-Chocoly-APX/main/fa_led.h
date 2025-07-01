/*
 * fa_led.h
 *
 *  Created on: 2024年2月1日
 *      Author: mastwet
 */

#ifndef MAIN_FA_LED_H_
#define MAIN_FA_LED_H_

#include <stdio.h>
#include "esp_err.h"
#include "driver/rmt_tx.h"

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_GPIO_NUM      11

#define EXAMPLE_LED_NUMBERS         16
#define EXAMPLE_CHASE_SPEED_MS      50


/**
 * @brief Type of led strip encoder configuration
 */
typedef struct {
    uint32_t resolution; /*!< Encoder resolution, in Hz */
} led_strip_encoder_config_t;

/**
 * @brief Create RMT encoder for encoding LED strip pixels into RMT symbols
 *
 * @param[in] config Encoder configuration
 * @param[out] ret_encoder Returned encoder handle
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_ERR_NO_MEM out of memory when creating led strip encoder
 *      - ESP_OK if creating encoder successfully
 */
esp_err_t rmt_new_led_strip_encoder(const led_strip_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);


void fa_led_init();
void fa_led_test();

void fa_led_clean();
void fa_set_rgb_show(char number,char r,char g,char b);
void fa_set_rgb(char number,char r,char g,char b);
void fa_show();

#endif /* MAIN_FA_LED_H_ */
