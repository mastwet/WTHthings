#include <Arduino.h>
#include "key.h"

bool RPKey::get_key_val(uint8_t ch) {
  shift.update();
  switch (ch) {
    case 0:
      return shift.state(0);
      break;
    case 1:
      return shift.state(1);
      break;
    case 2:
      return shift.state(2);
      break;
    case 3:
      return shift.state(3);
      break;
    case 4:
      return shift.state(4);
      break;
    case 5:
      return shift.state(5);
      break;
    case 6:
      return shift.state(6);
      break;
    case 7:
      return shift.state(7);
      break;
    default:
      break;
  }
}

void RPKey::key_hal_init() {
  shift.begin(36, 21, 35, 37);
}

void RPKey::key_list_init() {
  for (uint8_t i = 0; i < (sizeof(key) / sizeof(KEY)); ++i) {
    key[i].val = key[i].last_val = get_key_val(i);
  }
}

void RPKey::key_scan() {
  for (uint8_t i = 0; i < (sizeof(key) / sizeof(KEY)); ++i) {
    key[i].val = get_key_val(i);        // 获取键值
    if (key[i].last_val != key[i].val)  // 发生改变
    {
      key[i].last_val = key[i].val;  // 更新状态
      if (key[i].val == LOW) {
        key_msg.id = i;
        key_msg.pressed = true;
      }
      else{
        key_msg.pressed = false;
      }
    }
  }
}

bool RPKey::key_is_pressed(int key_num){
  if(key_msg.id == key_num && key_msg.pressed == true){
    return true;
  }
  else{
    return false;
  }
}