// 定義控制腳位
const int loadPin = 10;   // SH/LD 腳位
const int clockPin = 11; // CLK 腳位
const int dataPin = 12;  // QH 腳位（數據輸入）

void setup() {
  // 初始化控制腳位
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);

  // 啟動序列
  digitalWrite(loadPin, HIGH);
  digitalWrite(clockPin, LOW);

  // 開啟序列通信
  Serial.begin(9600);
}

void loop() {
  // 載入數據到移位暫存器
  digitalWrite(loadPin, LOW);
  delayMicroseconds(5); // 確保信號穩定
  digitalWrite(loadPin, HIGH);

  // 讀取24位數據（三個74HC165，每個8位）
  unsigned long data = shiftInData(24);

  // 顯示讀取的數據
  Serial.print("讀取的數據: ");
  Serial.println(data, BIN); // 以二進制顯示
  delay(500); // 延遲半秒
}

unsigned long shiftInData(int bits) {
  unsigned long value = 0;
  for(int i = 0; i < bits; i++) {
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(1); // 短暫延遲以確保時鐘信號穩定
    value = (value << 1) | digitalRead(dataPin);
    digitalWrite(clockPin, LOW);
    delayMicroseconds(1);
  }
  return value;
}
