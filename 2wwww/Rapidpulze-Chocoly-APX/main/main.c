#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "main.h"
#include "oled.h"
#include "fa_led.h"
#include "timtrace.h"
#include "global.h"
#include "timer.h"


static const char* TAG = "MAIN";

ssd1306_handle_t ssd1306_handle;
QueueHandle_t timtrace_queue;

//定义当前的工作模式
//RunMode running_mode = MIDI;

//定义当前按键的按下时长
uint64_t key_pressed_time[8] = {0};

void TaskOLED(void *pvParameters);
void TaskFaLed(void *pvParameters);
void TaskMain(void *pvParameters);

void app_main(void)
{
	//ssd1306_handle = ssd_1306_init();
	//fa_led_init();
	//timtrace_init();
	timer_init();
	//device_test();

	//xTaskCreate(uart_event_task, "timtrace_main_loop", 2048, NULL, 12, NULL);

	while(1);
    // timtrace_main_loop:Create a task to handler UART event from ISR
    //xTaskCreate(uart_event_task, "timtrace_main_loop", 2048, NULL, 12, NULL);

}

/*
void TaskMain(void *pvParameters){
	message_t timtrace_message;
	uint32_t pressed_time;
	while(1){
//		timtrace_message = get_timtrace_message();
//		if(timtrace_message != 0){
//			if(running_mode == MIDI){
//				switch(timtrace_message.name){
//					//机械键盘按键消息，直接发送MIDI表中的对应的键值
//
//					//旋钮消息，直接发送对应旋钮的键值
//
//					//按下旋钮和左侧按键开关的消息，开始记录时间
//
//					//如果短按，直接发送对应的键值
//					//如果长按，那么就会进入配置模式，并进入一个死循环中进行不断处理
//				}
//				//midi模式之下发送midi消息队列
//				//最后，通过任务通知的形式来通知led主程序需要更新最新的信息。
//			}
//		}

	}
}
*/

void TaskOLED(void *pvParameters) {
	while(1){

	}
}

void TaskFaLed(void *pvParameters){
	while(1){

	}
}


//message_t* get_timtrace_message(){
//	message_t temp_message;
//    if (timtrace_queue != NULL) {
//      int ret = xQueueReceive(timtrace_queue, &temp_message, portMAX_DELAY);
//      if (ret == pdPASS) {
//    	  return temp_message;
//      }
//      else if (ret == pdFALSE) {
//    	  ESP_LOGI(TAG,"The `get_timtrace_message` was unable to receive data from the Queue");
//      }
//    }
//	return 0;
//}

void device_test(){
	fa_led_test();
	ssd_1306_test(ssd1306_handle);
}
