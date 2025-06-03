#include <ShiftIn.h> // 引入ShiftIn库

// 创建一个ShiftIn实例，参数2表示有2个74HC165芯片级联
ShiftIn<2> shift;

// 定义芯片相关的引脚
const int pLoadPin = 47; // 并行加载或复位引脚
const int clockEnablePin = 42; // 时钟使能引脚（如果库支持的话）
const int dataPin = 45; // 数据输出引脚（连接至第一个74HC165的SER引脚）
const int clockPin = 48; // 时钟引脚（连接至所有74HC165的CLK引脚）

int last_sta[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 

void setup() {
  Serial.begin(115200); // 初始化Serial通信，波特率为9600

  // 初始化ShiftIn对象
  shift.begin(pLoadPin, clockEnablePin, dataPin, clockPin);
  
  // 若库未自动处理加载操作，可以在开始时加载一次初始状态
  // shift.loadState();
}

int remap(int num){
  // 输入序列
  //int input[] = {12, 13, 14, 15, 11, 10, 9, 8, 4, 5, 6, 7, 3, 2, 1, 0};
  int input[] = {15,14,13,12,8,9,10,11,7,6,5,4,0,1,2,3};
  return input[num];
}

// 假设这是16个按键对应的16个不同的CC编号
const int midiCcNumbers[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

void echoValues() {
  for(int i = 0; i < shift.getDataWidth(); i++) {
    if(last_sta[i] != shift.state(i)){
      if(shift.state(i) == LOW){
        MIDI.sendControlChange(midiCcNumbers[buttonNumber], 127, MIDI_CHANNEL_OMNI); // 发送CC信号
      }
      else{
        MIDI.sendControlChange(midiCcNumbers[buttonNumber], 0, MIDI_CHANNEL_OMNI); // 发送CC信号
      }
      //Serial.printf("%d pressed!",remap(i));
    }
    last_sta[i] = shift.state(i);
  }
  Serial.println();

}

void loop() {
  // 更新所有按键状态并检查是否有按键状态变化
  if(shift.update()) {
    echoValues();
  }
  
  // 延迟一段时间再检测，减少不必要的输出
  delay(10); // 可根据需求调整延迟时间
}