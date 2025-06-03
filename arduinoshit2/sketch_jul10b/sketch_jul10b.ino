# include <Arduino.h>

// 定义编码器引脚
const int encoderPinA = 7; // 编码器A相连接的引脚
const int encoderPinB = 8; // 编码器B相连接的引脚

// 编码器计数变量
int encoderCount = 0;

// 上一次A、B通道的状态
int lastStateA = HIGH;
int lastStateB = HIGH;

void setup() {
  // 设置编码器引脚为输入，并启用内部上拉电阻
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  // 初始化串口通信，用于打印计数结果
  Serial.begin(9600);
}

void loop() {
  // 读取当前A、B通道的状态
  int currentStateA = digitalRead(encoderPinA);
  int currentStateB = digitalRead(encoderPinB);

  // 判断是否有状态变化
  if ((currentStateA != lastStateA) || (currentStateB != lastStateB)) {
    // 计算新的状态，判断旋转方向
    int newState = (currentStateA << 1) | currentStateB;
    int lastState = (lastStateA << 1) | lastStateB;

    // 根据AB状态变化计算旋转方向
    if (newState == 3 && lastState == 2) { // 从2到3，逆时针
      encoderCount--;
    } else if (newState == 0 && lastState == 1) { // 从1到0，逆时针
      encoderCount--;
    } else if (newState == 2 && lastState == 3) { // 从3到2，顺时针
      encoderCount++;
    } else if (newState == 1 && lastState == 0) { // 从0到1，顺时针
      encoderCount++;
    }

    // 更新上一次的状态
    lastStateA = currentStateA;
    lastStateB = currentStateB;
  }

  // 每次循环都打印当前的计数
  Serial.println(encoderCount);

  // 可以添加适当的延时以控制检测频率，避免过于频繁的读取
  delay(10); // 例如，每10毫秒检查一次
}