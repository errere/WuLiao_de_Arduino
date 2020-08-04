
#include "PT6311B.h"
PT6311B P(8, 4, 7, 13);
uint8_t j = 0x00;
const uint8_t FontsNum[10] = { 0b0111111, 0b0000110, 0b1011011, 0b1001111, 0b1100110, 0b1101101, 0b1111101, 0b0000111, 0b1111111, 0b1101111 };
void setup()
{
  delay(1000);
  P.init(GR8SG20);

}

// Add the main program code into the continuous loop() function
void loop()
{
  P.PMemSet(P.VFDPointAdr[1], 0xff);
  P.PMemSet(P.VFDCharAdr[0], P.VFDCharMap[0]);
  P.PMemSet(P.VFDCharAdr[1], 0x00);
  for (int i = 2; i < 8; i++)
  {
    P.PMemSet(P.VFDCharAdr[i], P.VFDCharMap[random(10)]);
  }
  delay(200);

}
