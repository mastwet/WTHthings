/*
 * oled.h
 *
 *  Created on: 2024年2月1日
 *      Author: mastwet
 */

#ifndef MAIN_OLED_H_
#define MAIN_OLED_H_

#include "ssd1306.h"

#define I2C_MASTER_SCL_IO 4        /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 2        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */

ssd1306_handle_t ssd_1306_init();
void iic_init();
void ssd_1306_test(ssd1306_handle_t s_handle);

#endif /* MAIN_OLED_H_ */
