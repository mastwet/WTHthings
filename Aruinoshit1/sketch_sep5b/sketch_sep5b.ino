#include <Bounce2.h>
 
// 定义编码器引脚
const int encoderPinA = 14;
const int encoderPinB = 15;
const int buttonPin = 4;
 
// 创建编码器对象和按键对象
Bounce encoderButton = Bounce();
Bounce encoderPinAButton = Bounce();
Bounce encoderPinBButton = Bounce();
 
void setup() {
  // 初始化编码器引脚和按键引脚
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
 
  // 设置按键对象的参数
  encoderButton.attach(buttonPin);
  encoderButton.interval(5);  // 设置按键检测间隔，单位为毫秒
 
  encoderPinAButton.attach(encoderPinA);
  encoderPinAButton.interval(5);
 
  encoderPinBButton.attach(encoderPinB);
  encoderPinBButton.interval(5);
}
 
void loop() {
  // 更新按键状态
  encoderButton.update();
  encoderPinAButton.update();
  encoderPinBButton.update();
 
  // 检测按键状态
  if (encoderButton.fell()) {
    // 按键按下事件处理
    Serial.println("Button pressed");
  }
  if (encoderButton.rose()) {
    // 按键释放事件处理
    Serial.println("Button released");
  }
 
  // 检测编码器旋转
  if (encoderPinAButton.fell()) {
    if (encoderPinBButton.read() == HIGH) {
      // 顺时针旋转
      Serial.println("Clockwise rotation");
    } else {
      // 逆时针旋转
      Serial.println("Counterclockwise rotation");
    }
  }
}