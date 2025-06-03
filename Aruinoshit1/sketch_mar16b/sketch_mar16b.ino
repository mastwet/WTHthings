#include <FastLED.h>

#define NUM_LEDS 64 // 定义LED的数量为64
#define DATA_PIN 11   // 定义数据输入脚，根据你的实际接线更改

CRGB leds[NUM_LEDS]; // 创建一个CRGB类型的数组来存储LED的颜色信息

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // 初始化LED控制器和引脚
}

void loop() {
  // 点亮所有LED
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White; // 将每个LED设置为白色
  }
  FastLED.show(); // 更新LED状态

  delay(1000); // 延迟一秒

  // 熄灭所有LED
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black; // 将每个LED设置为黑色（熄灭）
  }
  FastLED.show(); // 更新LED状态

  delay(1000); // 延迟一秒后再开始下一轮循环
}