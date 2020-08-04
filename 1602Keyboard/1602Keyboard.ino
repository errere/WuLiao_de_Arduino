#include "LiquidCrystal595.h"
#include <stdint.h>

#define KEY_ENTER PB15
#define KEY_LEFT PB4
#define KEY_RIGHT PB5
#define KEY_SELECT PB7
#define KEY_BACKSPACE PB8

#define LETTER_TABLE_LEN 103

#define WINDOW_OFF_MAX 97

#define SERIAL_BACKSPACE 0x7f
#define SERIAL_ENTER 0x0d

const uint8_t spaKey[7] = {0x03, 0x0f, 0x18, 0x1a, 0x13, 0x1b, 0x09};
const uint8_t upKey[3] = {0x1b,0x5b,0x41};
const uint8_t downkey[3] = {0x1b,0x5b,0x42};


const char LetterTable[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', //26
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', //52
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',                                                                                 //62
    '!', '@', '#', '$', '%', '^', '&', '*', ',', '.',                                                                                 //72
    '/', '\\', '?', ';', ':', '\'', '\"', '|', '-', '+', '=', '`', '~',                                                               //85
    '(', ')', '<', '>', '[', ']', '{', '}',                                                                                           //93
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06,                                                                                               //99(ctrl + coxzs)+esc
    0xdd,0xa2,0xa3,' '};                                                                                                              //103(tab,space),up,down

uint8_t fxg[8] = {
    0b00000,
    0b10000,
    0b01000,
    0b00100,
    0b00010,
    0b00001,
    0b00000,
    0b00000,
};

uint8_t CTRL_C[8] = {
    0b11111,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b11111,
    0b00000,
    0b11111,
};
uint8_t CTRL_O[8] = {
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11111,
    0b00000,
    0b11111,
};
uint8_t CTRL_X[8] = {
    0b10001,
    0b01010,
    0b00100,
    0b01010,
    0b10001,
    0b00000,
    0b00000,
    0b11111,
};
uint8_t CTRL_Z[8] = {
    0b11111,
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b10000,
    0b11111,
    0b11111,
};
uint8_t CTRL_S[8] = {
    0b11111,
    0b10000,
    0b10000,
    0b11111,
    0b00001,
    0b00001,
    0b11111,
    0b11111,
};
uint8_t ESC[8] = {
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b00100,
    0b00010,
    0b00001,
    0b11111,
};

LiquidCrystal lcd(PA7, PB0, PB1, PA6);

char LetterWindow[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
uint8_t CurWindow[6] = {0xc6, 0x20, 0x20, 0x20, 0x20, 0x20};
int8_t windowOff = 0;

int8_t CurIndex = 0; //0->5

//fn
void drawWindwow(char *window, int8_t off);
void drawCurWindows(uint8_t *Cwindow, int8_t cu);

inline void drawBlock(uint8_t enable);

//main
void draw()
{
      lcd.setCursor(0, 0);
      lcd.write(0x7f);
      lcd.setCursor(7, 0);
      lcd.write(0x7e);

      drawCurWindows(CurWindow, CurIndex);
      lcd.setCursor(1, 1);
      for (uint8_t i = 0; i < 6; i++)
      {
            lcd.write(CurWindow[i]);
      }

      drawWindwow(LetterWindow, windowOff);
      lcd.setCursor(1, 0);
      for (uint8_t i = 0; i < 6; i++)
      {
            if (LetterWindow[i] == '\\')
                  lcd.write((uint8_t)0x00);
            else
                  lcd.write((uint8_t)LetterWindow[i]);
      }
}
void input()
{
      if (digitalRead(KEY_RIGHT) == 0)
      {
            delay(3);
            if (digitalRead(KEY_RIGHT) == 0)
            {

                  CurIndex++;
                  if (CurIndex > 5)
                  {
                        CurIndex = 5;
                        windowOff++;
                        if (windowOff > WINDOW_OFF_MAX)
                        {
                              windowOff = 0;
                              CurIndex = 0;
                        }

                  } //if
                  delay(100);

            } // /digitalRead
      }       //KEY_RIGHT

      if (digitalRead(KEY_LEFT) == 0)
      {
            delay(3);
            if (digitalRead(KEY_LEFT) == 0)
            {

                  CurIndex--;
                  if (CurIndex < 0)
                  {
                        CurIndex = 0;
                        windowOff--;
                        if (windowOff < 0)
                        {
                              windowOff = WINDOW_OFF_MAX;
                              CurIndex = 5;
                        }

                  } //if
                  delay(100);

            } // /digitalRead
      }       //KEY_LEFT

      if (digitalRead(KEY_SELECT) == 0)
      {
            delay(10);
            if (digitalRead(KEY_SELECT) == 0)
            {
                  while (digitalRead(KEY_SELECT) == 0)
                        ;
                  drawBlock(1);
                  if (LetterWindow[CurIndex] > 0x06 && LetterWindow[CurIndex] < 0x7e)
                  {
                        Serial1.write(LetterWindow[CurIndex]);
                  }
                  else if(LetterWindow[CurIndex] <= 0x06)
                  {
                        Serial1.write(spaKey[CurIndex]);
                  }
                  else if(LetterWindow[CurIndex] == 0xdd)
                  {
                        Serial1.write(spaKey[6]);
                  }//tab
                  else if(LetterWindow[CurIndex] == 0xa2)
                  {
                        for(uint8_t j = 0;j < 3;++j)
                        Serial1.write(upKey[j]);
                  }//up
                  else if(LetterWindow[CurIndex] == 0xa3)
                  {
                        for(uint8_t j = 0;j < 3;++j)
                        Serial1.write(downkey[j]);
                  }//down
                  delay(100);
                  drawBlock(0);
            } // /digitalRead
      }       //KEY_SELECT

      if (digitalRead(KEY_BACKSPACE) == 0)
      {
            delay(10);
            if (digitalRead(KEY_BACKSPACE) == 0)
            {
                  while (digitalRead(KEY_BACKSPACE) == 0)
                        ;
                  drawBlock(1);
                  Serial1.write(SERIAL_BACKSPACE);
                  delay(100);
                  drawBlock(0);
            } // /digitalRead
      }       //KEY_BACKSPACE

      if (digitalRead(KEY_ENTER) == 0)
      {
            delay(10);
            if (digitalRead(KEY_ENTER) == 0)
            {
                  while (digitalRead(KEY_ENTER) == 0)
                        ;
                  drawBlock(1);
                  Serial1.write(SERIAL_ENTER);
                  delay(100);
                  drawBlock(0);
            } // /digitalRead
      }       //KEY_RIGHT
}

void setup()
{
      pinMode(KEY_ENTER, INPUT_PULLUP);
      pinMode(KEY_LEFT, INPUT_PULLUP);
      pinMode(KEY_RIGHT, INPUT_PULLUP);
      pinMode(KEY_SELECT, INPUT_PULLUP);
      pinMode(KEY_BACKSPACE, INPUT_PULLUP);

      Serial1.begin(115200);
      Serial.begin(115200);

      lcd.begin(16, 2);
      lcd.createChar(0, fxg);
      lcd.createChar(1, CTRL_C);
      lcd.createChar(2, CTRL_O);
      lcd.createChar(3, CTRL_X);
      lcd.createChar(4, CTRL_Z);
      lcd.createChar(5, CTRL_S);
      lcd.createChar(6, ESC);
}

void loop()
{
      //draw
      draw();
      input();
}

void drawWindwow(char *window, int8_t off)
{
      for (uint8_t i = 0; i < 6; i++)
      {
            window[i] = LetterTable[i + off];
      }
}

void drawCurWindows(uint8_t *Cwindow, int8_t cu)
{
      for (uint8_t i = 0; i < 6; i++)
      {
            CurWindow[i] = 0x20;
      }
      CurWindow[cu] = 0xc6;
}

inline void drawBlock(uint8_t enable)
{
      if (enable)
      {
            lcd.setCursor(0, 1);
            lcd.write(0xc8);
            lcd.setCursor(7, 1);
            lcd.write(0xba);
      }
      else
      {
            lcd.setCursor(0, 1);
            lcd.print(' ');
            lcd.setCursor(7, 1);
            lcd.print(' ');
      }
}

