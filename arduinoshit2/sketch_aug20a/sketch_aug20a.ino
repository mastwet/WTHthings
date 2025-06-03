#include <Arduino.h>

// 定义LED连接的引脚
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
const int numLeds = sizeof(ledPins) / sizeof(int);

void setup() {
  // 初始化所有LED引脚为输出
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // 循环点亮每一个LED
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], LOW);  // 打开LED
    delay(50);                      // 等待一段时间
    digitalWrite(ledPins[i], HIGH);   // 关闭LED
  }
}