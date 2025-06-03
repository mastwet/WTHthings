#include <Versatile_RotaryEncoder.h>

// SET READING PINS ACCORDINGLY TO YOUR ENCODER TO BOARD CONNECTIONS
// Set here your encoder reading pins (Ex.: EC11 with breakout board)
#define clk1 21  // (A3)
#define dt1 20   // (A2)
#define sw1 6    // (A4)

#define clk2 0  // (A3)
#define dt2 1   // (A2)
#define sw2 27  // (A4)

#define clk3 2  // (A3)
#define dt3 3   // (A2)
#define sw3 26  // (A4)

const int buttonPins[] = {9, 8, 7}; // 按钮引脚数组
const int numButtons = 3;           // 按钮数量

int lastButtonStates[numButtons];   // 存储上一次按钮状态

// Functions prototyping to be handled on each Encoder Event
void handleRotate(int8_t rotation);
void handlePressRotate(int8_t rotation);
void handleHeldRotate(int8_t rotation);
void handlePress();
void handleDoublePress();
void handlePressRelease();
void handleLongPress();
void handleLongPressRelease();
void handlePressRotateRelease();
void handleHeldRotateRelease();
void read_key();

// Create a global pointer for the encoder object
Versatile_RotaryEncoder *versatile_encoder1;
Versatile_RotaryEncoder *versatile_encoder2;
Versatile_RotaryEncoder *versatile_encoder3;

void setup() {

  Serial.begin(115200);
  versatile_encoder1 = new Versatile_RotaryEncoder(clk1, dt1, sw1);
  versatile_encoder2 = new Versatile_RotaryEncoder(clk2, dt2, sw2);
  versatile_encoder3 = new Versatile_RotaryEncoder(clk3, dt3, sw3);

  // Load to the encoder all nedded handle functions here (up to 9 functions)
  versatile_encoder1->setHandleRotate(handleRotate);
  versatile_encoder1->setHandlePress(handlePress);

  versatile_encoder2->setHandleRotate(handleRotate);
  versatile_encoder2->setHandlePress(handlePress);

  versatile_encoder3->setHandleRotate(handleRotate);
  versatile_encoder3->setHandlePress(handlePress);

    // 初始化所有按钮引脚为输入模式（启用内部上拉电阻）
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonStates[i] = HIGH; // 初始状态设为高电平
  }

  Serial.println("Ready!");

  // set your own defualt values (optional)
  // versatile_encoder->setInvertedSwitch(true); // inverts the switch behaviour from HIGH to LOW to LOW to HIGH
  // versatile_encoder->setReadIntervalDuration(1); // set 2ms as long press duration (default is 1ms)
  // versatile_encoder->setShortPressDuration(35); // set 35ms as short press duration (default is 50ms)
  // versatile_encoder->setLongPressDuration(550); // set 550ms as long press duration (default is 1000ms)
  // versatile_encoder->setDoublePressDuration(350); // set 350ms as double press duration (default is 250ms)
}

void loop() {

  // Do the encoder reading and processing
  if (versatile_encoder1->ReadEncoder()) {
    // Do something here whenever an encoder action is read
  }
  if (versatile_encoder2->ReadEncoder()) {
    // Do something here whenever an encoder action is read
  }
  if (versatile_encoder3->ReadEncoder()) {
    // Do something here whenever an encoder action is read
  }

  read_key();

}

// Implement your functions here accordingly to your needs

void handleRotate(int8_t rotation) {
  Serial.print("#1 Rotated: ");
  if (rotation > 0)
    Serial.println("Right");
  else
    Serial.println("Left");
}

void handlePress() {
  Serial.println("#4.1 Pressed");
}

void read_key(){
    for (int i = 0; i < numButtons; i++) {
    int currentState = digitalRead(buttonPins[i]);
    
    // 检测状态变化
    if (currentState != lastButtonStates[i]) {
      delay(50); // 简单的防抖动延时
      
      // 再次确认状态
      currentState = digitalRead(buttonPins[i]);
      if (currentState != lastButtonStates[i]) {
        // 更新状态并输出
        lastButtonStates[i] = currentState;
        
        if (currentState == LOW) {
          Serial.print("按钮 ");
          Serial.print(buttonPins[i]);
          Serial.println(" 被按下");
        } else {
          Serial.print("按钮 ");
          Serial.print(buttonPins[i]);
          Serial.println(" 被释放");
        }
      }
    }
}
}