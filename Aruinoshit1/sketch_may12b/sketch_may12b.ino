// 定义按钮引脚
const int buttonPins[7] = {2, 3, 4, 5, 6, 7, 8};

void setup() {
  Serial.begin(9600);

  // 设置按钮为输入
  for (int i = 0; i < 7; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);  // 使用内部上拉电阻
  }

  // 模拟引脚无需设置 pinMode，默认是输入
}

void loop() {
  // 显示按钮状态
  Serial.println("Button States:");

  Serial.print("A: "); Serial.println(digitalRead(buttonPins[0]) == LOW ? "Pressed" : "Not Pressed");
  Serial.print("B: "); Serial.println(digitalRead(buttonPins[1]) == LOW ? "Pressed" : "Not Pressed");
  Serial.print("C: "); Serial.println(digitalRead(buttonPins[2]) == LOW ? "Pressed" : "Not Pressed");
  Serial.print("D: "); Serial.println(digitalRead(buttonPins[3]) == LOW ? "Pressed" : "Not Pressed");
  Serial.print("E: "); Serial.println(digitalRead(buttonPins[4]) == LOW ? "Pressed" : "Not Pressed");
  Serial.print("F: "); Serial.println(digitalRead(buttonPins[5]) == LOW ? "Pressed" : "Not Pressed");
  Serial.print("K (Joystick): "); Serial.println(digitalRead(buttonPins[6]) == LOW ? "Pressed" : "Not Pressed");

  // 读取摇杆值
  int xAxis = analogRead(A0);
  int yAxis = analogRead(A1);

  Serial.print("Joystick X: "); Serial.println(xAxis);
  Serial.print("Joystick Y: "); Serial.println(yAxis);

  Serial.println("----------------------------");

  delay(500); // 每隔半秒读取一次
}
