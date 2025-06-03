#include <Versatile_RotaryEncoder.h>

// 编码器引脚配置
#define ENCODER1_CLK 21
#define ENCODER1_DT 20
#define ENCODER1_SW 6

#define ENCODER2_CLK 0
#define ENCODER2_DT 1
#define ENCODER2_SW 27

#define ENCODER3_CLK 2
#define ENCODER3_DT 3
#define ENCODER3_SW 26

// 创建三个编码器实例
Versatile_RotaryEncoder *encoder[3];

// 通用事件处理函数
void handleEncoderEvent(uint8_t encoderID, const char* eventType, int8_t rotation = 0) {
    Serial.print("#");
    Serial.print(encoderID + 1);
    Serial.print(" ");
    Serial.print(eventType);
    
    if (rotation != 0) {
        rotation > 0 ? Serial.println(" Right") : Serial.println(" Left");
    } else {
        Serial.println();
    }
}

// 旋转通用处理
void handleRotate(int8_t rotation) {
    // 通过引脚状态判断当前编码器
    for(uint8_t i=0; i<3; i++){
          handleEncoderEvent(i, "Rotated", rotation);
    }
}

// 按钮事件处理（示例处理长按）
void handleLongPress() {
    for(uint8_t i=0; i<3; i++){
          handleEncoderEvent(i, "Long press");
    }
}

// 其他事件处理函数类似...

void setup() {
    Serial.begin(9600);

    // 初始化编码器实例
    encoder[0] = new Versatile_RotaryEncoder(ENCODER1_CLK, ENCODER1_DT, ENCODER1_SW);
    encoder[1] = new Versatile_RotaryEncoder(ENCODER2_CLK, ENCODER2_DT, ENCODER2_SW);
    encoder[2] = new Versatile_RotaryEncoder(ENCODER3_CLK, ENCODER3_DT, ENCODER3_SW);

    // 设置通用回调函数
    for(uint8_t i=0; i<3; i++){
        encoder[i]->setHandleRotate(handleRotate);
        encoder[i]->setHandleLongPress(handleLongPress);
        // 设置其他需要的事件回调...
    }

    Serial.println("System Ready");
}

void loop() {
    // 轮询所有编码器
    for(uint8_t i=0; i<3; i++){
        encoder[i]->ReadEncoder();
    }
}
