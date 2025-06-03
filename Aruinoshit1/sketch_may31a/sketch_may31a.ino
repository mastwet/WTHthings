void setup() {
  Serial.begin(115200);  // 初始化串口通信
  randomSeed(analogRead(0));  // 使用未连接的模拟引脚作为随机种子
}

void loop() {
  // 1. 生成按钮掩码 (7个整数 0或1)
  String dataPacket = "";
  for (int i = 0; i < 7; i++) {
    dataPacket += String(random(0, 2));  // 随机生成0或1
    dataPacket += " ";
  }

  // 2. 生成摇杆数据 (-1.0 ~ 1.0)
  dataPacket += String(random(-100, 101) / 100.0, 2);  // JOY_X
  dataPacket += " ";
  dataPacket += String(random(-100, 101) / 100.0, 2);  // JOY_Y
  dataPacket += " ";

  // 3. 生成加速度计数据 (-2.0 ~ 2.0 g)
  for (int i = 0; i < 3; i++) {
    dataPacket += String(random(-200, 201) / 100.0, 2);  // ACCEL_X/Y/Z
    dataPacket += " ";
  }

  // 4. 生成陀螺仪数据 (-200.0 ~ 200.0 dps)
  for (int i = 0; i < 3; i++) {
    dataPacket += String(random(-20000, 20001) / 100.0, 2);  // GYRO_X/Y/Z
    if (i < 2) dataPacket += " ";
  }

  // 发送完整数据包 (共15个数据点)
  Serial.println(dataPacket);

  // 控制发送频率 (约20ms = 50Hz)
  delay(10);
}
