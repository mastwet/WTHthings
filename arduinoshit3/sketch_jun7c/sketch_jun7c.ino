#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);

// Declare a function type for menu actions.
typedef void (*menuAction)();

struct MenuItem {
  String func_name;     // The name of the function to be displayed
  menuAction func_ptr;  // Function pointer for the function to be executed
};

class Dovemenu {
private:
  MenuItem* menu_items;   // List of menu items
  int menu_size;          // Size of the menu
  int current_index = 0;  // Currently highlighted menu item

public:
  Dovemenu() {}

  void begin(MenuItem* menu_items, int menu_size) {
    this->menu_items = menu_items;
    this->menu_size = menu_size;

  }

  void displayMenu() {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 32, menu_items[current_index].func_name.c_str());
    u8g2.sendBuffer();
  }

  void menu_left() {
    if (current_index < menu_size - 1) {
      current_index++;
    } else {
      current_index = 0;
    }
  }

  void menu_right() {
    if (current_index > 0)
      current_index--;
    else
      current_index = menu_size - 1;
  }

  void addKeyEventHandler(int pin, int event) {

  }

  void loop() {
  }

  void selectItem() {
    menu_items[current_index].func_ptr();
  }
};

Dovemenu dv;

void doSomething(){}
void doSomethingElse(){}

void setup() {
  MenuItem menu_items[] = {
  { "Do something", &doSomething },
  { "Do something else", &doSomethingElse },
};
    u8g2.begin();
    u8g2.setFont(u8g2_font_5x7_tr);  // set the font for the terminal window
    //dv.displayMenu();
    dv.begin(menu_items,2);
}

void loop() {
      dv.displayMenu();
}
