/*
 * timer.c
 *
 *  Created on: 2024年2月9日
 *      Author: mastwet
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "global.h"

SemaphoreHandle_t xSemaphore;
g_timer_count_element_t g_tick;
QueueHandle_t queue = 0;

typedef struct {
    uint64_t g_timer_count;
} g_timer_count_element_t;

typedef struct {
    uint64_t event_count;
} example_queue_element_t;

static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    // Stop timer the sooner the better
    gptimer_stop(timer);
    // Retrieve the count value from event data
    example_queue_element_t ele = {
        .event_count = edata->count_value
    };
    // Optional: send the event data to other task by OS queue
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return high_task_awoken == pdTRUE;
}

void TaskMillis(void *pvParameters){
	   // 在任务开始时创建信号量，初始计数值为1
	    xSemaphore = xSemaphoreCreateBinary();
	    if( xSemaphore != NULL )
	    {
	        // 信号量创建成功，可以进行相关操作
	        while( 1 )
	        {
	            if( xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE )
	            {
                // 已获取信号量，执行临界区代码
                if (xQueueReceive(queue, &ele, pdMS_TO_TICKS(2000))) {
                    ESP_LOGI(TAG, "Timer alarmed, count=%llu", ele.event_count);
                } else {
                    ESP_LOGW(TAG, "Missed one count event");
                }

	            // 请求获得信号量

	                // ... 这里是需要保护的代码段 ...
	                // 信号量使用完毕，释放它
	                xSemaphoreGive(xSemaphore);
	            }
	            else{}
	        }
	    }
	    else
	    {
	        // 信号量创建失败，进行错误处理
	    	ESP_LOGE(TAG, "xSemaphore create error");
	    }
}


void timer_init(){

	ESP_LOGI(TAG, "Starting Timer");
    example_queue_element_t ele;
    queue = xQueueCreate(10, sizeof(example_queue_element_t));
    if (!queue) {
        ESP_LOGE(TAG, "Creating queue failed");
        return;
    }

    ESP_LOGI(TAG, "Create timer handle");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb,
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));
    ESP_LOGI(TAG, "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_LOGI(TAG, "Start timer, stop it at alarm event");
    gptimer_alarm_config_t alarm_config1 = {
        .alarm_count = 1000000, // period = 1s
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));
    ESP_ERROR_CHECK(gptimer_start(gptimer));


	xTaskCreate(TaskMillis, "ticker", 2048, NULL, 12, NULL);

}

uint64_t millis(){
    // 请求获得信号量
    if( xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE )
    {
    	return g_tick.g_timer_count
        xSemaphoreGive(xSemaphore);
    }
    else{
    	millis();
    }
}

