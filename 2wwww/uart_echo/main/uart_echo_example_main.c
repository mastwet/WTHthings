#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

// MIDI配置参数
#define MIDI_UART_PORT_NUM    UART_NUM_1    // 使用UART1
#define MIDI_BAUD_RATE        31250         // 标准MIDI波特率
#define MIDI_TXD_PIN          GPIO_NUM_17   // 根据硬件连接修改
#define MIDI_CHANNEL          0             // MIDI通道0-15
#define NOTE_C4               60            // 中央C的音符编号

// 任务配置
#define TASK_STACK_SIZE       2048
#define PRIORITY              10

static const char *TAG = "MIDI Demo";

// 发送MIDI Note On消息
void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t msg[3] = {
        0x90 | (channel & 0x0F), // Note On状态字节
        note & 0x7F,              // 音符编号
        velocity & 0x7F           // 力度
    };
    uart_write_bytes(MIDI_UART_PORT_NUM, (const char*)msg, sizeof(msg));
}

// 发送MIDI Note Off消息
void midi_note_off(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t msg[3] = {
        0x80 | (channel & 0x0F), // Note Off状态字节
        note & 0x7F,              // 音符编号
        velocity & 0x7F           // 力度
    };
    uart_write_bytes(MIDI_UART_PORT_NUM, (const char*)msg, sizeof(msg));
}

static void midi_task(void *arg)
{
    // UART配置
    uart_config_t uart_config = {
        .baud_rate = MIDI_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // 安装UART驱动并配置引脚
    ESP_ERROR_CHECK(uart_driver_install(MIDI_UART_PORT_NUM, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(MIDI_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(MIDI_UART_PORT_NUM, MIDI_TXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    bool note_state = false;

    while (1) {
        if (note_state) {
            // 发送Note Off（也可以发送力度为0的Note On）
            midi_note_off(MIDI_CHANNEL, NOTE_C4, 0);
            ESP_LOGI(TAG, "Sent Note Off: C4");
        } else {
            // 发送Note On
            midi_note_on(MIDI_CHANNEL, NOTE_C4, 127);
            ESP_LOGI(TAG, "Sent Note On: C4");
        }

        note_state = !note_state;
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 等待1秒
    }
}

void app_main(void)
{
    xTaskCreate(midi_task, "midi_task", TASK_STACK_SIZE, NULL, PRIORITY, NULL);
}
