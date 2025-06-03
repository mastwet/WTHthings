#define clk1 21  // 编码器1 CLK引脚
#define dt1 20   // 编码器1 DT引脚
#define sw1 6    // 编码器1 SW引脚

#define clk2 0   // 编码器2 CLK引脚
#define dt2 1    // 编码器2 DT引脚
#define sw2 27   // 编码器2 SW引脚

#define clk3 2   // 编码器3 CLK引脚
#define dt3 3    // 编码器3 DT引脚
#define sw3 26   // 编码器3 SW引脚

// 编码器1变量
volatile int counter1 = 0;
int lastClk1 = HIGH;
int lastSw1State = HIGH;
unsigned long lastSw1Press = 0;

// 编码器2变量
volatile int counter2 = 0;
int lastClk2 = HIGH;
int lastSw2State = HIGH;
unsigned long lastSw2Press = 0;

// 编码器3变量
volatile int counter3 = 0;
int lastClk3 = HIGH;
int lastSw3State = HIGH;
unsigned long lastSw3Press = 0;

void setup() {
  Serial.begin(115200);

  // 初始化编码器引脚（启用内部上拉电阻）
  pinMode(clk1, INPUT_PULLUP);
  pinMode(dt1, INPUT_PULLUP);
  pinMode(sw1, INPUT_PULLUP);

  pinMode(clk2, INPUT_PULLUP);
  pinMode(dt2, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);

  pinMode(clk3, INPUT_PULLUP);
  pinMode(dt3, INPUT_PULLUP);
  pinMode(sw3, INPUT_PULLUP);
}

void loop() {
  // 处理编码器1
  handleEncoder(clk1, dt1, &counter1, &lastClk1, "Encoder1");
  handleButton(sw1, &lastSw1State, &lastSw1Press, "Encoder1 SW");
  
  // 处理编码器2
  handleEncoder(clk2, dt2, &counter2, &lastClk2, "Encoder2");
  handleButton(sw2, &lastSw2State, &lastSw2Press, "Encoder2 SW");
  
  // 处理编码器3
  handleEncoder(clk3, dt3, &counter3, &lastClk3, "Encoder3");
  handleButton(sw3, &lastSw3State, &lastSw3Press, "Encoder3 SW");
}

// 编码器旋转处理函数
void handleEncoder(int clkPin, int dtPin, volatile int *counter, int *lastClk, const char* encoderName) {
  int currentClk = digitalRead(clkPin);
  if (currentClk != *lastClk) {
    if (currentClk == LOW) { // 检测下降沿
      int dtState = digitalRead(dtPin);
      if (dtState == HIGH) {
        (*counter)++;
      } else {
        (*counter)--;
      }
      Serial.print(encoderName);
      Serial.print(": ");
      Serial.println(*counter);
    }
    *lastClk = currentClk;
  }
}

// 按键处理函数
void handleButton(int swPin, int *lastSwState, unsigned long *lastPressTime, const char* buttonName) {
  int currentSw = digitalRead(swPin);
  if (currentSw == LOW && *lastSwState == HIGH) {
    if (millis() - *lastPressTime > 50) { // 去抖动
      Serial.println(buttonName);
      *lastPressTime = millis();
    }
  }
  *lastSwState = currentSw;
}
