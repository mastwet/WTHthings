#include <stdio.h>
#include <string.h>
#include "timtrace.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "global.h"


static QueueHandle_t uart1_queue;
static const char* TAG = "TIMTRACE";

//void handle_ttmessage_to_event(char event_type,char event_number,char event_value)
//{
//    if (timtrace_queue != NULL && uxQueueSpacesAvailable(timtrace_queue) > 0) {
//      message_t midimessage = {
//    		  .type = event_type
//    		  .name = event_number
//    		  .value = event_value
//      };
//      int ret = xQueueSend(timtrace_queue, (void *)&midimessage, 0);
//      if (ret == pdTRUE) {
//      }
//      else if (ret == errQUEUE_FULL) {
//    	  ESP_LOGI(TAG, "The `handle_ttmessage_to_event` was unable to send data into the Queue");
//      }
//    }
//}

void read_from_timtrace(const char *dtmp)
{
    if (dtmp[0] == 0xfa && dtmp[4] == 0xff)
    {
        switch (dtmp[1])
        {
        case 0x01: // 按键事件
            // dtmp[2] 决定是按键按下(0x01)还是释放(0x02)
            switch (dtmp[2])
            {
            // dtmp[3] 包含按键编号
            case 0x01:
               ESP_LOGI(TAG, "Timtrace:Button Pressed: %d", dtmp[3]);
                break;
            case 0x02:
                ESP_LOGI(TAG, "Timtrace:Button Released: %d", dtmp[3]);
                break;
            default:
                break;
            }
            // 在这里处理按键事件
            break;
        case 0xb2: // 旋钮事件
            // dtmp[2] 决定旋钮向左(0xca)还是向右(0xcb)旋转
            switch (dtmp[2])
            {
            // dtmp[3] 包含旋钮编号相关值
            case 0xca:
                ESP_LOGI(TAG, "Timtrace:Knob Rotated Left: %d", dtmp[3]);
                break;
            case 0xcb:
                ESP_LOGI(TAG, "Timtrace:Knob Rotated Right: %d", dtmp[3]);
                break;
            default:
                break;
            }
            break;
        case 0x03: // 另一种类型的按键事件
            // dtmp[3] 决定按键状态，0xa1 表示释放，0xa2 表示按下
            switch (dtmp[3])
            {
                case 0xa1:
                    ESP_LOGI(TAG, "Timtrace:Button Released: %d", dtmp[2]);
                    break;
                case 0xa2:
                    ESP_LOGI(TAG, "Timtrace:Button Pressed: %d", dtmp[2]);
                    break;
            }
            // dtmp[2] 包含按键编号
            break;
        default:
            ESP_LOGI(TAG, "Timtrace:Unknown packet type: %d", dtmp[1]);
            break;
        }
    }
}

void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE);
    for (;;)
    {
        // 等待 UART 事件
        if (xQueueReceive(uart1_queue, (void *)&event, (TickType_t)portMAX_DELAY))
        {
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
            switch (event.type)
            {
            // UART 接收数据的事件
            // 我们最好快速处理数据事件，因为数据事件的数量会比 其他类型的事件多得多。
            // 如果我们在数据事件上花费太多时间，队列可能会满。
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
                read_from_timtrace((const char*)dtmp);
                //ESP_LOGI(TAG, "[DATA EVT]:");
                //uart_write_bytes(EX_UART_NUM, (const char *)dtmp, event.size);
                break;
            // 检测到硬件 FIFO 溢出事件
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                uart_flush_input(EX_UART_NUM);
                xQueueReset(uart1_queue);
                break;
            // UART 环形缓冲区满的事件
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                uart_flush_input(EX_UART_NUM);
                xQueueReset(uart1_queue);
                break;
            // UART RX 断开事件、奇偶校验错误的事件、帧错误的事件不做处理
            // UART_PATTERN_DET这一段被删除了
            // 其他事件
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void timtrace_init(){
    esp_log_level_set(TAG, ESP_LOG_INFO);

    /* 配置 UART 驱动的参数，
     * 通信引脚并安装驱动 */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Install UART driver, and get the queue.
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart1_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);

    // Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);
    // Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(EX_UART_NUM, 5, 6, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);
    // Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(EX_UART_NUM, 20);

}
