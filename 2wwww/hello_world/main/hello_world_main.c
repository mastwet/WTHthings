#include "driver/spi_master.h"
#include "driver/gpio.h"

// 定义SPI和74HC165的相关引脚
#define SPI_HOST     SPI2_HOST
#define SPI_SCLK_IO  GPIO_NUM_14
#define SPI_MISO_IO  GPIO_NUM_NC  // 74HC165没有MISO
#define SPI_MOSI_IO  GPIO_NUM_13  // 用于模拟SH_CP（时钟使能）
#define SPI_CS_IO    GPIO_NUM_12  // 用于选通74HC165芯片（如果有多个则需分时复用）
#define DATA_IN_IO   GPIO_NUM_2  // 用于读取74HC165的数据输出引脚（QH' 或 SERIAL OUT）

spi_device_handle_t spi;

void init_spi() {
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SPI_MOSI_IO,
        .miso_io_num = SPI_MISO_IO,
        .sclk_io_num = SPI_SCLK_IO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 8,  // 只传输8位数据
    };

    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1000000, // 根据实际情况调整SPI时钟频率
        .mode = 0, // SPI MODE 0
        .spics_io_num = SPI_CS_IO,
        .queue_size = 1,
    };

    esp_err_t ret = spi_bus_initialize(SPI_HOST, &bus_cfg, 1);
    if (ret != ESP_OK) {
        // 错误处理
    }

    ret = spi_bus_add_device(SPI_HOST, &dev_cfg, &spi);
    if (ret != ESP_OK) {
        // 错误处理
    }
}

uint16_t read74HC165_via_SPI() {
    uint16_t data = 0;
    spi_transaction_t t = {
        .length = 8 * 8,  // 假设需要多次SPI交易以读取全部8位数据
        .tx_buffer = NULL,  // 不需要发送数据
        .rx_buffer = (uint8_t*)&data,  // 直接转换指针类型，而不是使用reinterpret_cast
        .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,  // 注意这种方式不是标准SPI交互
    };

    // 实现74HC165读取的具体时序代码...

    esp_err_t err = spi_device_transmit(spi, &t);
    if (err != ESP_OK) {
        // 错误处理
    }

    return data;
}

void app_main() {
    init_spi();
    while (true) {
        uint16_t keys = read74HC165_via_SPI();
        // 处理读取到的按键状态
    }
}
