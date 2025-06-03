#include "freertos/event_groups.h"
 
/* 定义事件位 */
#define TASK_1_BIT        ( 1 << 0 ) //1
#define TASK_2_BIT        ( 1 << 1 ) //10
#define TASK_3_BIT        ( 1 << 2 ) //100
#define ALL_SYNC_BITS     (TASK_1_BIT | TASK_2_BIT | TASK_3_BIT) //111
 
/* 创建事件组 */
EventGroupHandle_t eg;
 
void setup() {
 
  Serial.begin(112500);
  eg = xEventGroupCreate();
  
  xTaskCreate(
      task1,           /* 任务函数. */
      "task1",        /* 任务名称 */
      10000,                    /* 任务的堆栈大小 */
      NULL,                     /* 任务的参数 */
      3,                        /*任务的优先级 */
      NULL);                    /* 跟踪创建的任务的任务句柄*/
  xTaskCreate(
      task2,           
      "task2",        
      10000,                   
      NULL,                     
      1,                        
      NULL);                    
  xTaskCreate(
      task3,          
      "task3",      
      10000,                    
      NULL,                    
      1,                       
      NULL);                    
}
 
void loop() {
 
}
 
void task1( void * parameter )
{
  for(;;){
    Serial.println("task1 done");
    /* task 2完成打印，因此设置其事件位并等待其他任务完成 */
    EventBits_t uxBits = xEventGroupSync(eg, TASK_1_BIT, ALL_SYNC_BITS, portMAX_DELAY );
    /* 如果其他任务完成，则所有事件位将被设置为*/
    if( ( uxBits & ALL_SYNC_BITS ) == ALL_SYNC_BITS ){
        Serial.println("task 1 - all task done !!!");
    }
  }
  vTaskDelete( NULL );
}
 
void task2( void * parameter )
{
 
  for(;;){
    Serial.println("task2 done");
    /* task 2完成打印，因此设置其事件位 */
    EventBits_t uxBits = xEventGroupSync( eg, TASK_2_BIT, ALL_SYNC_BITS, portMAX_DELAY );
  }
  vTaskDelete( NULL );
}
void task3( void * parameter )
{
  for(;;){
    Serial.println("task3 done");
    /*  task 3完成打印，因此设置其事件位* */
    EventBits_t uxBits = xEventGroupSync( eg, TASK_3_BIT, ALL_SYNC_BITS, portMAX_DELAY );
  }
  vTaskDelete( NULL );
}