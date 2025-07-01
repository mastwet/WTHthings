/*
 * oled .c
 *
 *  Created on: 2024年2月1日
 *      Author: mastwet
 */

#include "oled.h"


void iic_init(){
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void ssd_1306_test(ssd1306_handle_t s_handle){
    char data_str[10] = {0};
    sprintf(data_str, "C STR");
    ssd1306_draw_string(s_handle, 70, 16, (const uint8_t *)data_str, 16, 1);
    ssd1306_refresh_gram(s_handle);
}

ssd1306_handle_t ssd_1306_init(){
	iic_init();
	ssd1306_handle_t ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_refresh_gram(ssd1306_dev);
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    return ssd1306_dev;
}
