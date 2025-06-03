#include <Arduino.h>
#include <FreeRTOS.h>
#include <U8g2lib.h>
//#include <Versatile_RotaryEncoder.h>
#include "Adafruit_Keypad.h"
#include "key.h"
#include "encoder.h"
#include "main.h"
#include "PCF8574.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ OLED_SCL, /* data=*/ OLED_SDA, /* reset=*/ U8X8_PIN_NONE);



TwoWire I2Cone = TwoWire(0);
PCF8574 pcf8574(&I2Cone, 0x20);

//Versatile_RotaryEncoder *encoder_1;
//Versatile_RotaryEncoder *encoder_2;
//Versatile_RotaryEncoder *encoder_3;
//Versatile_RotaryEncoder *encoder_4;

TaskHandle_t keypad_read_task_handle;
TaskHandle_t OLED_Read_Task_Handle;

//管理encoder的值
int encoder_value[8] = {0};

void setup() {
    Serial.begin(115200);
    //init_button();
    //encoder_init();

    u8g2.begin();


//    // This variant of task creation can also specify on which core it will be run (only relevant for multi-core ESPs)
//    xTaskCreatePinnedToCore(
//            TaskKeypad
//            ,  "4x4 Keypad Task"
//            ,  1024  // Stack size
//            ,  NULL  // When no parameter is used, simply pass NULL
//            ,  2  // Priority
//            ,  &keypad_read_task_handle // With task handle we will be able to manipulate with this task.
//            ,  ARDUINO_RUNNING_CORE // Core on which the task will run
//    );
    delay(1000);
    // This variant of task creation can also specify on which core it will be run (only relevant for multi-core ESPs)

        // This variant of task creation can also specify on which core it will be run (only relevant for multi-core ESPs)
//    xTaskCreatePinnedToCore(
//            TaskKeypad
//            ,  "4x4 Keypad Task"
//            ,  1024  // Stack size
//            ,  NULL  // When no parameter is used, simply pass NULL
//            ,  2  // Priority
//            ,  &keypad_read_task_handle // With task handle we will be able to manipulate with this task.
//            ,  ARDUINO_RUNNING_CORE // Core on which the task will run
//    );
    // configSTACK_DEPTH_TYPE ulMaxStackSize = configMINIMAL_STACK_SIZE * 10; // 示例，实际中应该根据系统情况设定
    xTaskCreate(
            TaskOLED
            ,  "Task Blink" // A name just for humans
            ,  1024        // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
            ,  nullptr // Task parameter which can modify the task behavior. This must be passed as pointer to void.
            ,  1  // Priority
            ,  nullptr // Task handle is not used here - simply pass NULL
    );
    delay(1000);
}

void loop() {
    delay(10000);
    //show_oled();
    //read_button();
    //read_encoder();
    //read_keypad();
}

void init_button(){
    I2Cone.begin(4,3); // SDA pin 16, SCL pin 17, 400kHz frequency
    delay(1000);

    // Set pinMode to OUTPUT
    for(int i=0;i<8;i++) {
        pcf8574.pinMode(i, INPUT);
    }

    Serial.print("Init pcf8574...");
    if (pcf8574.begin()){
        Serial.println("OK");
    } else {
        Serial.println("KO");
    }
    Serial.println("START");
}

int encoder_direct = 0;

void output_value(int num,int direct){
    if(direct == 1){
        encoder_value[num]++;
    }
    else if(direct == -1){
        encoder_value[num]--;
    }
    Serial.printf("Encoder %d value:%d",num,encoder_value[num]);
}

void handlePress(){

}

void handleRotate(int8_t rotation) {
    if (rotation > 0){
        encoder_direct = 1;
    }
    else{
        encoder_direct = -1;
    }
}

//void encoder_init(){
//    encoder_1 = new Versatile_RotaryEncoder(clk1, dt1, sw1);
//    encoder_2 = new Versatile_RotaryEncoder(clk2, dt2, sw2);
//    encoder_3 = new Versatile_RotaryEncoder(clk3, dt3, sw3);
//    encoder_4 = new Versatile_RotaryEncoder(clk4, dt4, sw4);
//
//    encoder_1->setHandleRotate(handleRotate);
//    encoder_2->setHandleRotate(handleRotate);
//    encoder_3->setHandleRotate(handleRotate);
//    encoder_4->setHandleRotate(handleRotate);
////    encoder_1->setHandlePress(handlePress);
////    encoder_2->setHandlePress(handlePress);
////    encoder_3->setHandlePress(handlePress);
////    encoder_4->setHandlePress(handlePress);
//    Serial.println("Encoder OK!");
//}

//void read_encoder(){
//
//    if (encoder_1->ReadEncoder()) {
//        output_value(0,encoder_direct);
//        encoder_direct = 0;
//    }
//
//    if (encoder_2->ReadEncoder()) {
//        output_value(1,encoder_direct);
//        encoder_direct = 0;
//    }
//
//    if (encoder_3->ReadEncoder()) {
//        output_value(2,encoder_direct);
//        encoder_direct = 0;
//    }
//
//    if (encoder_4->ReadEncoder()) {
//        output_value(3,encoder_direct);
//        encoder_direct = 0;
//    }
//}



void read_button(){
    PCF8574::DigitalInput di = pcf8574.digitalReadAll();
    Serial.print("READ VALUE FROM PCF P1: ");
    Serial.print(di.p0);
    Serial.print(" - ");
    Serial.print(di.p1);
    Serial.print(" - ");
    Serial.print(di.p2);
    Serial.print(" - ");
    Serial.println(di.p3);
    Serial.print(di.p4);
    Serial.print(" - ");
    Serial.print(di.p5);
    Serial.print(" - ");
    Serial.print(di.p6);
    Serial.print(" - ");
    Serial.println(di.p7);
}




void TaskOLED( void *pvParameters ){
    u8g2.firstPage();
    for(;;){
        u8g2.drawHLine(0,0,10);
        u8g2.drawHLine(0,31,10);

        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.drawStr(0,24,"Hello World!");
        u8g2.nextPage();
        delay(10);
    }
}
