#include <SoftwareSerial.h>
#include <EEPROM.h>
#define KEY_A 2
#define KEY_B 3
#define JAK_TOP 9
#define JAK_MID 8
#define LCD_RXD 10
#define TX_LED 13

#define BAUD_ADR 10

#define BAUD_TABLE_MAX 9

SoftwareSerial TRMB(JAK_TOP, JAK_MID); // RX, TX
SoftwareSerial TRMA(JAK_MID, JAK_TOP); // RX, TX
SoftwareSerial LCD(11, LCD_RXD);       // RX, TX

uint8_t SerSelect = 0; //0 = A,1 = B
signed char c = 0;
const int8_t userName[3] = {0x70, 0x69, 0x0D};
const int8_t userPwd[10] = {0x72, 0x61, 0x73, 0x70, 0x62, 0x65, 0x72, 0x72, 0x79, 0x0D};

const uint32_t BAUD_TABLE[9] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880, 115200};
int8_t baudMode = 0;

void setup()
{

      pinMode(KEY_A, 2);

      pinMode(KEY_B, 2);
      pinMode(TX_LED, OUTPUT);

      digitalWrite(TX_LED, 0);

      LCD.begin(115200);
      Serial.begin(115200);

      baudMode = EEPROM.read(BAUD_ADR);

      delay(10);

      if (digitalRead(KEY_A) == 0)
      {
            if (digitalRead(KEY_B) == 0)
            {
                  LCD.print("DS12(2,0,'B = ");
                  LCD.print(BAUD_TABLE[baudMode]);
                  LCD.print("    ',5,0);\r\n");
                  while (1)
                  {
                        if (digitalRead(KEY_A) == 0) //keyA
                        {
                              delay(5);
                              if (digitalRead(KEY_A) == 0)
                              {
                                    while (digitalRead(KEY_A) == 0)
                                          ;
                                    ++baudMode;
                                    if (baudMode >= BAUD_TABLE_MAX)
                                          baudMode = 0;
                                    LCD.print("DS12(2,0,'B = ");
                                    LCD.print(BAUD_TABLE[baudMode]);
                                    LCD.print("    ',5,0);\r\n");
                                    EEPROM.write(BAUD_ADR, baudMode);
                              }
                        } //KEY_A

                        if (digitalRead(KEY_B) == 0) //KEYB
                        {
                              delay(5);
                              if (digitalRead(KEY_B) == 0)
                              {
                                    while (digitalRead(KEY_B) == 0)
                                          ;
                                    --baudMode;
                                    if (baudMode < 0)
                                          baudMode = BAUD_TABLE_MAX - 1;
                                    LCD.print("DS12(2,0,'B = ");
                                    LCD.print(BAUD_TABLE[baudMode]);
                                    LCD.print("    ',5,0);\r\n");
                                    EEPROM.write(BAUD_ADR, baudMode);
                              }
                        } //KEY_B
                  }
            }
      }

      LCD.print("SPG(1);\r\n");

      while (1)
      {
            if (digitalRead(KEY_A) == 0) //keyA
            {
                  delay(5);
                  if (digitalRead(KEY_A) == 0)
                  {
                        while (digitalRead(KEY_A) == 0)
                              ;

                        TRMB.end();
                        pinMode(JAK_TOP, OUTPUT);
                        pinMode(JAK_MID, INPUT);
                        TRMA.begin(BAUD_TABLE[baudMode]);
                        SerSelect = 0;
                        LCD.print("TERM;\r\n");
                        delay(10);
                        break;
                  }
            } //KEY_A

            if (digitalRead(KEY_B) == 0) //KEYB
            {
                  delay(5);
                  if (digitalRead(KEY_B) == 0)
                  {
                        while (digitalRead(KEY_B) == 0)
                              ;

                        TRMA.end();
                        pinMode(JAK_TOP, INPUT);
                        pinMode(JAK_MID, OUTPUT);
                        TRMB.begin(BAUD_TABLE[baudMode]);
                        SerSelect = 1;
                        LCD.print("TERM;\r\n");
                        delay(10);
                        break;
                  }
            } //KEY_B

      } //while
      digitalWrite(TX_LED, 1);
      if (SerSelect == 1)
      {
            while (1)
            {
                  loopB();
            }
      }
} //setup

void loop()
{
      if (TRMA.available())
      {
            c = TRMA.read();
            delay(1);
            if (c != 0x1a && c <= 0x7f)
            {
                  LCD.write(c);
                  delay(1);
            }
      }

      if (Serial.available())
      {
            TRMA.write(Serial.read());
      }
      if (digitalRead(KEY_A) == 0) //keyA
      {
            TrmSendA(userName, 3);
            delay(100);
      }
      if (digitalRead(KEY_B) == 0) //keyA
      {
            TrmSendA(userPwd, 10);
            delay(100);
      }
}
void loopB()
{
      if (TRMB.available())
      {
            c = TRMB.read();
            delay(1);
            if (c != 0x1a && c <= 0x7f)
            {
                  LCD.write(c);
                  delay(1);
            }
      }

      if (Serial.available())
      {
            TRMB.write(Serial.read());
      }
      if (digitalRead(KEY_A) == 0) //keyA
      {
            TrmSendB(userName, 3);
            delay(100);
      }
      if (digitalRead(KEY_B) == 0) //keyA
      {
            TrmSendB(userPwd, 10);
            delay(100);
      }
}
inline void TrmSendA(uint8_t *buf, uint8_t len)
{
      for (uint8_t i = 0; i < len; ++i)
      {
            TRMA.write(buf[i]);
      }
}
inline void TrmSendB(uint8_t *buf, uint8_t len)
{
      for (uint8_t i = 0; i < len; ++i)
      {
            TRMB.write(buf[i]);
      }
}
