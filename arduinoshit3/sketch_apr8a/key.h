#include <ShiftIn.h>

#define READ 2
#define BTN0 4
#define BTN1 5
#define BTN2 18

// 按键变量
typedef struct
{
  bool val;
  bool last_val;
} KEY;

// 按键信息
typedef struct
{
  uint8_t id;
  bool pressed;
} KEY_MSG;

// KEY_MSG key_msg={0};
// KEY key[3]={false};

class RPKey
{
public:
  KEY_MSG key_msg;
  KEY key[8];

  RPKey()
  {
    key_msg = {0};
    key[8] = {false};
    key_hal_init();
    key_list_init();
  }

  void key_scan();
  bool key_is_pressed(int key_num);

private:
  bool get_key_val(uint8_t ch);
  void key_list_init();
  void key_hal_init();
  ShiftIn<1> shift;
};