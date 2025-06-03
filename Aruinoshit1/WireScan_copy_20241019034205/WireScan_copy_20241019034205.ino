#include <Wire.h>

#define DEVICE_ADDRESS 64  // 设备地址，十进制64，十六进制0x40
#define REGISTER_ADDRESS_08 0x08  // 要读取的寄存器地址0x08

void setup() {
  // 初始化串口，便于调试
  Serial.begin(9600);
  // 初始化I2C通信
  Wire.begin();
  Wire.setClock(10000);
}

void loop() {
  // 启动与设备的I2C通信
  Wire.beginTransmission(DEVICE_ADDRESS);

  // 写入寄存器地址（A6-A0），即0x08
  Wire.write(REGISTER_ADDRESS_08);

  // 结束写操作，开始从设备读取
  Wire.endTransmission(false); // 使用`false`表示保持总线激活，用于后续的读操作

  // 请求从设备读取1个字节的数据（D7-D0），表示按键K0-K7的状态
  Wire.requestFrom(DEVICE_ADDRESS, 1);
  byte data1 = 0;
  if (Wire.available()) {
    data1 = Wire.read();
    Serial.println("按键K0-K7状态:");
    for (int i = 0; i < 8; i++) {
      Serial.print("K");
      Serial.print(i);
      Serial.print(": ");
      Serial.println((data1 & (1 << i)) ? "按下" : "未按下");
    }
  }

  // 发送ACK，继续读取设备发送的第二个字节（D7-D4，D3-D0固定为低电平）
  Wire.requestFrom(DEVICE_ADDRESS, 1);
  byte data2 = 0;
  if (Wire.available()) {
    data2 = Wire.read();
    byte highNibble = data2 >> 4;  // 仅取高4位（D7-D4），表示按键K8-K11的状态
    Serial.println("按键K8-K11状态:");
    for (int i = 0; i < 4; i++) {
      Serial.print("K");
      Serial.print(i + 8);
      Serial.print(": ");
      Serial.println((highNibble & (1 << i)) ? "按下" : "未按下");
    }
  }

  // 发送NACK结束时序
  // 读取操作会在 requestFrom() 自动结束，无需额外调用 endTransmission

  // 暂停1秒，以免数据读取过快
  delay(1000);
}
