#include <LiquidCrystal.h> // 引入LCD库

// 定义LCD的针脚，根据您提供的针脚编号进行设置
LiquidCrystal lcd(4, 6, 5, 10, 11, 12, 13); // RS, E, RW, D4, D5, D6, D7

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
}
