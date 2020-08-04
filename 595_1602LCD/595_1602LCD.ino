#include"LiquidCrystal595.h"
//LiquidCrystal(uint8_t sout, uint8_t rck, uint8_t sck, uint8_t enable);
LiquidCrystal lcd(PA7, PB0, PB1 , PA6);


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

