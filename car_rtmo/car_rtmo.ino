#include <SoftwareSerial.h>

uint8_t button = 0;
char asasas[20];
uint8_t sendStop = 0;
SoftwareSerial SerialS2(A2, A3); // RX, TX
int axis_X = 0;
int axis_Y = 0;
void setup() {
  // put your setup code here, to run once:
  SerialS2.begin(57600);
  //Serial.begin(115200);
  pinMode(2, 2);
  pinMode(3, 2);
  pinMode(8, 2);
  pinMode(9, 2);

  pinMode(7, 2);
  pinMode(13,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  axis_X = map(analogRead(A1), 0, 1023, 0, 255);
  axis_Y = map(analogRead(A0), 1023, 0, 0, 255);
  button = (!digitalRead(2) * 0b01000000) + (!digitalRead(3) * 0b00100000) + (!digitalRead(8) * 0b00010000) + (!digitalRead(9) * 0b00001000) + (!digitalRead(7) * 0b00000010);
  //Serial.print("X = ");
  //Serial.println(axis_X); //X
 // Serial.print("Y = ");
  //Serial.println(axis_Y); //Y
  //sprintf(asasas, "0x%x\r\n", button);
  //Serial.print(asasas);
  //Serial.print("\r\n\r\n ");

  if (axis_X > 145 | axis_X < 125 | axis_Y > 140 | axis_Y < 119 | button != 0) {
    sendStop  = 0;
    digitalWrite(13,1);
    SerialS2.write(0xa0);
    SerialS2.write(axis_X); //145,125
    SerialS2.write(axis_Y); //140,119

    SerialS2.write(button);
    SerialS2.write(0xff);
    SerialS2.write(0x0a);
  } else {
    if (sendStop < 20) {
      sendStop ++;
       digitalWrite(13,0);
      SerialS2.write(0xa0);
      SerialS2.write(0x8a);
      SerialS2.write(0x87);

      SerialS2.write(button);
      SerialS2.write(0xff);
      SerialS2.write(0x0a);

    }
  }

  delay(20);

}
