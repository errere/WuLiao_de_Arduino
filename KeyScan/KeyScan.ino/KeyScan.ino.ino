/*键盘左边ctrl    ---   KEY_LEFT_CTRL
键盘左边shift   ---   KEY_LEFT_SHIFT  
键盘左边alt     ---   KEY_LEFT_ALT
键盘左边win     ---   KEY_LEFT_GUI  
键盘右边ctrl    ---   KEY_RIGHT_CTRL
键盘右边shift   ---   KEY_RIGHT_SHIFT
键盘右边alt     ---   KEY_RIGHT_ALT
键盘右边win     ---   KEY_RIGHT_GUI
方向键上        ---   KEY_UP_ARROW
方向键下        ---   KEY_DOWN_ARROW
方向键左        ---   KEY_LEFT_ARROW
方向键右        ---   KEY_RIGHT_ARROW
空格键          ---   KEY_BACKSPACE
tab键           ---   KEY_TAB
回车键          ---   KEY_RETURN
esc键           ---   KEY_ESC
insert键        ---   KEY_INSERT
delete键        ---   KEY_DELETE
page up键       ---   KEY_PAGE_UP 
page down键     ---   KEY_PAGE_DOWN
home键          ---   KEY_HOME
end键           ---   KEY_END
capslock键      ---   KEY_CAPS_LOCK 
F1              ---   KEY_F1
F2              ---   KEY_F2
F3              ---   KEY_F3
F4              ---   KEY_F4
F5              ---   KEY_F5
F6              ---   KEY_F6
F7              ---   KEY_F7
F8              ---   KEY_F8
F9              ---   KEY_F9
*/
#include <LiquidCrystal.h>
#include <USBComposite.h>
#include <stdint.h>

const uint8_t rs = PA3, en = PA4, d4 = PB1, d5 = PB0, d6 = PB10, d7 = PB11;
const byte ROWS = 3;                                //four rows
const byte COLS = 4;                                //three columns
const uint8_t rowPins[ROWS] = {PB12, PB13, PB14};   //connect to the row pinouts of the keypad
const uint8_t colPins[COLS] = {PB7, PB6, PB5, PB4}; //connect to the column pinouts of the keypad

const uint8_t Reg_Sout = PA2;
const uint8_t Reg_SCK = PA1;
const uint8_t Reg_RCK = PA0;

USBHID HID;
HIDKeyboard Keyboard(HID);
USBCompositeSerial CompositeSerial;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
HardwareTimer timer(1);

uint8_t kbMat[3][4] = {{0}};
uint8_t KeyPanCounter = 0;

uint8_t KBLED = 0;

uint16_t LLL = 0x01;

//free
char FreeKeyMap[3][4] = {
    {'1', '2', '3', '*'},
    {'4', '5', '6', '0'},
    {'7', '8', '9', '#'}};
uint8_t FreeDispX[3][4] = {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {0, 1, 2, 3}};
uint8_t FreeDispY[3][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {1, 1, 1, 1}};

uint8_t FreeKeyDownLast[3][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}};

//game
char *GAME_MICRO_TEXT[13] = {
    "        ",
    //GTA
    "1 K INVF",
    "1 K INVC",
    "1 K BUY",
    "1 K dc3s",
    //mc
    "1 K GM1",
    "1 K GM0",
    "1KFILL1",
    "1KFILL2",

    "1 KNR3",
    "1 KNR3",
    "1 KNR3",
    "1 KNR3"};

void off();
uint8_t KeypanRead();
void loopF();
void dispLED();
void LEDTRIG();

void point(char c);
void pointS(char *s);

void R595Write(uint16_t L);

void setup()
{
      uint8_t temp = 0;
      // put your setup code here, to run once:
      lcd.begin(8, 2);
      pinMode(PB14, OUTPUT);
      pinMode(PB13, OUTPUT);
      pinMode(PB12, OUTPUT);

      pinMode(PB7, INPUT_PULLDOWN);
      pinMode(PB6, INPUT_PULLDOWN);
      pinMode(PB5, INPUT_PULLDOWN);
      pinMode(PB4, INPUT_PULLDOWN);

      pinMode(Reg_Sout, OUTPUT);
      pinMode(Reg_SCK, OUTPUT);
      pinMode(Reg_RCK, OUTPUT);

      digitalWrite(Reg_SCK, 0);
      digitalWrite(Reg_RCK, 0);
      digitalWrite(Reg_Sout, 0);

      HID.begin(CompositeSerial, HID_KEYBOARD);
      Keyboard.begin();

      lcd.setCursor(0, 0);
      lcd.print("F1= FREE");
      lcd.setCursor(0, 1);
      lcd.print("F2= GAME");
      delay(200);
      timer.pause();
      timer.setPrescaleFactor(10000);
      timer.setOverflow(720);
      timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
      timer.setCompare(TIMER_CH1, 720);
      timer.attachCompare1Interrupt(LEDTRIG);
      timer.refresh();
      timer.resume();
      do
      {
            temp = KeypanRead();
            delay(1);
      } while (temp == 0);

      if (kbMat[0][0] == 1)
      {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("FREE");
            delay(500);
            lcd.clear();
            for (;;)
                  loopF();
      }
      
      if (kbMat[0][1] == 1)
      {
            //Keyboard.press('a');
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("GAME");
            delay(500);
            lcd.clear();
            //Keyboard.release('a');
      }
      if (kbMat[0][2] == 1)
      {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("SERIAL");
            delay(500);
            //lcd.clear();
            for (;;)
                  loopS();
      }
}

void loopF()
{
      dispLED();

      for (uint8_t i = 0; i < 3; i++)
      {
            off();
            digitalWrite(rowPins[i], 1);
            for (uint8_t j = 0; j < 4; j++)
            {
                  lcd.setCursor(FreeDispX[i][j], FreeDispY[i][j]);
                  if (digitalRead(colPins[j]) == 1)
                  {
                        Keyboard.press(FreeKeyMap[i][j]);
                        lcd.print(FreeKeyMap[i][j]);
                        FreeKeyDownLast[i][j] = 1;
                  }
                  else
                  {
                        if (FreeKeyDownLast[i][j] == 1)
                        {
                              FreeKeyDownLast[i][j] = 0;
                              Keyboard.release(FreeKeyMap[i][j]);
                              lcd.print(' ');
                        }
                  }
            }
      }
}

void loop()
{
      dispLED();
      uint8_t c = KeypanRead();
      if (c)
      {
            lcd.setCursor(0, 1);
            lcd.print("SEND");
            lcd.setCursor(0, 0);
            //     "1 K INVF",
            //     "1 K INVC",
            //     "1 K BUY",
            //     "1 K dc3s",

            //     "1 K GM1",
            //     "1 K GM0",
            //     "1KFILL1",
            //     "1KFILL2",

            //     "1 KNR3",
            //     "1 KNR3",
            //     "1 KNR3",
            //     "1 KNR3"};
            if (kbMat[0][0])
            {
                  lcd.print(GAME_MICRO_TEXT[1]);
                  point('m');
                  delay(20);
                  for(uint8_t i = 0;i < 12;++i)
                  {
                  point(KEY_UP_ARROW);
                  delay(25);
                  }
                  point(KEY_RETURN);
                  delay(25);
                  point(KEY_DOWN_ARROW);
                  delay(25);
                  point(KEY_RETURN);
            }
            else if (kbMat[0][1])
            {
                  lcd.print(GAME_MICRO_TEXT[2]);
                  point('m');
                  delay(20);
                  for(uint8_t i = 0;i < 12;++i)
                  {
                    point(KEY_UP_ARROW);
                  delay(25);
                  }
                  point(KEY_RETURN);
                  delay(25);
                  point(KEY_DOWN_ARROW);
                  delay(25);
                  point(KEY_DOWN_ARROW);
                  delay(25);
                  point(KEY_RETURN);
            }
            else if (kbMat[0][2])
            {
                  lcd.print(GAME_MICRO_TEXT[3]);
                  Keyboard.press(KEY_RETURN);
                  delay(50);
                  Keyboard.release(KEY_RETURN);
            }
            else if (kbMat[0][3])
            {
                  lcd.print(GAME_MICRO_TEXT[4]);

                  timer.pause();
                  delay(100);

                  Keyboard.press('e');
                  delay(20);
                  Keyboard.release('e');

                  delay(4940);

                  Keyboard.press('s');
                  delay(20);
                  Keyboard.release('s');

                  delay(100);
                  timer.setPrescaleFactor(10000);
                  timer.setOverflow(720);
                  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
                  timer.setCompare(TIMER_CH1, 720);
                  timer.attachCompare1Interrupt(LEDTRIG);
                  timer.refresh();
                  timer.resume();
            }
            //--------------------------
            else if (kbMat[1][0])
            {
                  lcd.print(GAME_MICRO_TEXT[5]);
                  pointS("/gamemode creative");
                  delay(50);
                  point(KEY_RETURN);
            }
            else if (kbMat[1][1])
            {
                  lcd.print(GAME_MICRO_TEXT[6]);
                  pointS("/gamemode survival");
                  delay(50);
                  point(KEY_RETURN);
            }
            else if (kbMat[1][2])
            {
                  lcd.print(GAME_MICRO_TEXT[7]);
                  pointS("/fill ");
                  delay(50);
                  point(KEY_TAB);
                  delay(50);
                  point(' ');
                  delay(50);
                  point(KEY_TAB);
                  delay(50);
                  point(' ');
                  delay(50);
                  point(KEY_TAB);
                  delay(50);
                  point(' ');
                  point(KEY_RETURN);
            }
            else if (kbMat[1][3])
            {
                  lcd.print(GAME_MICRO_TEXT[8]);
                  point('/');
                  delay(50);
                  point(KEY_UP_ARROW);
                  delay(50);
                  point(' ');
                  delay(50);
                  point(KEY_TAB);
                  delay(50);
                  point(' ');
                  delay(50);
                  point(KEY_TAB);
                  delay(50);
                  point(' ');
                  delay(50);
                  point(KEY_TAB);
                  delay(50);
                  point(' ');
            }
            //--------------------------
            else if (kbMat[2][0])
            {
                  lcd.print(GAME_MICRO_TEXT[9]);
                  timer.pause();
                  delay(100);

                  Keyboard.press(' ');
                  delay(1000);
                  Keyboard.press('d');
                  Keyboard.press('s');
                  delay(15);
                  Keyboard.release(' ');
                  delay(100);
                  Keyboard.release('d');
                  Keyboard.release('s');

                  delay(100);
                  timer.setPrescaleFactor(10000);
                  timer.setOverflow(720);
                  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
                  timer.setCompare(TIMER_CH1, 720);
                  timer.attachCompare1Interrupt(LEDTRIG);
                  timer.refresh();
                  timer.resume();
            }
            else if (kbMat[2][1])
            {
                  lcd.print(GAME_MICRO_TEXT[10]);
                  lcd.print(GAME_MICRO_TEXT[9]);
                  timer.pause();
                  delay(100);

                  Keyboard.press(' ');
                  delay(1000);
                  Keyboard.press('d');
                  //Keyboard.press('s');
                  delay(15);
                  Keyboard.release(' ');
                  delay(100);
                  Keyboard.release('d');
                  //Keyboard.release('s');

                  delay(100);
                  timer.setPrescaleFactor(10000);
                  timer.setOverflow(720);
                  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
                  timer.setCompare(TIMER_CH1, 720);
                  timer.attachCompare1Interrupt(LEDTRIG);
                  timer.refresh();
                  timer.resume();
            }
            else if (kbMat[2][2])
            {
                  lcd.print(GAME_MICRO_TEXT[11]);
                  timer.pause();
                  delay(100);

                  Keyboard.press(' ');
                  delay(1000);
                  Keyboard.press('w');
                  //Keyboard.press('s');
                  delay(15);
                  Keyboard.release(' ');
                  delay(100);
                  Keyboard.release('w');
                  //Keyboard.release('s');

                  delay(100);
                  timer.setPrescaleFactor(10000);
                  timer.setOverflow(720);
                  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
                  timer.setCompare(TIMER_CH1, 720);
                  timer.attachCompare1Interrupt(LEDTRIG);
                  timer.refresh();
                  timer.resume();
            }
            else if (kbMat[2][3])
            {
                  lcd.print(GAME_MICRO_TEXT[12]);
                  timer.pause();
                  delay(100);

                  Keyboard.press(' ');
                  delay(1000);
                  Keyboard.press('s');
                  //Keyboard.press('s');
                  delay(15);
                  Keyboard.release(' ');
                  delay(100);
                  Keyboard.release('s');
                  //Keyboard.release('s');

                  delay(100);
                  timer.setPrescaleFactor(10000);
                  timer.setOverflow(720);
                  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
                  timer.setCompare(TIMER_CH1, 720);
                  timer.attachCompare1Interrupt(LEDTRIG);
                  timer.refresh();
                  timer.resume();
            }
            lcd.setCursor(0, 0);
            lcd.print(GAME_MICRO_TEXT[0]);
            lcd.setCursor(0, 1);
            lcd.print("    ");
      }
}

void loopS()
{
      //CompositeSerial

      for (uint8_t i = 0; i < 3; i++)
      {
            off();
            digitalWrite(rowPins[i], 1);
            for (uint8_t j = 0; j < 4; j++)
            {
                  lcd.setCursor(FreeDispX[i][j], FreeDispY[i][j]);
                  if (digitalRead(colPins[j]) == 1)
                  {
                        delay(10);
                        if (digitalRead(colPins[j]) == 1)
                        {
                              while (digitalRead(colPins[j]) == 1)
                                    ;
                              CompositeSerial.print(FreeKeyMap[i][j]);
                        }
                  }
            }
      }
}

void LEDTRIG()
{
      R595Write(LLL);
      if(LLL & 0x8000)LLL = 0x01;
      LLL<<=1;
      
}

void off()
{
      digitalWrite(PB14, 0);
      digitalWrite(PB13, 0);
      digitalWrite(PB12, 0);
}

uint8_t KeypanRead()
{
      KeyPanCounter = 0;
      for (uint8_t i = 0; i < 3; ++i)
      {
            off();
            digitalWrite(rowPins[i], 1);
            for (uint8_t j = 0; j < 4; ++j)
            {
                  kbMat[i][j] = digitalRead(colPins[j]);
                  if (kbMat[i][j] == 1)
                        KeyPanCounter++;
            }
      }
      return KeyPanCounter;
}

void dispLED()
{
      KBLED = Keyboard.getLEDs();
      lcd.setCursor(5, 1);
      if ((KBLED & 0b001))
      {
            lcd.print('N');
      }
      else
      {
            lcd.print(' ');
      }
      lcd.setCursor(6, 1);
      if ((KBLED & 0b010))
      {
            lcd.print('C');
      }
      else
      {
            lcd.print(' ');
      }
      lcd.setCursor(7, 1);
      if ((KBLED & 0b100))
      {
            lcd.print('S');
      }
      else
      {
            lcd.print(' ');
      }
}

void point(char c)
{
      Keyboard.press(c);
      delay(20);
      Keyboard.release(c);
}

void pointS(char *s)
{
      while (*s)
      {
            Keyboard.press(*s);
            delay(20);
            Keyboard.release(*s);
            delay(20);
            *s++;
      }
}

void R595Write(uint16_t L)
{
      pinMode(Reg_Sout, OUTPUT);
      pinMode(Reg_SCK, OUTPUT);
      pinMode(Reg_RCK, OUTPUT);
      for (uint8_t i = 0; i < 16; i++)
      {
            digitalWrite(Reg_Sout, ((L & 0x01) ? 1 : 0));
            L >>= 1;
            digitalWrite(Reg_SCK, 1);
            delay(1);
            digitalWrite(Reg_SCK, 0);
            delay(1);
      }
      digitalWrite(Reg_RCK, 1);
      delay(1);
      digitalWrite(Reg_RCK, 0);
}
