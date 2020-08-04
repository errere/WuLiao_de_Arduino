
#include "PT6311B.h"
PT6311B P(PB12, PB13, PB14, PB15); //uint8_t stby, uint8_t clk, uint8_t din, uint8_t dout

uint32_t tmp = 0x01;

uint8_t regMap[8] = {0x00, 0x03, 0x06, 0x09, 0x0c, 0x0f, 0x12, 0x15};




void setup()
{
  delay(1000);
  P.init(GR8SG20);
}

// Add the main program code into the continuous loop() function
void loop()
{
  for (int i = 0; i < 92; ++i)
  {
    for (int j = 0; j < 7; ++j)
    {
      P.Send20Bit(regMap[j], FourteenSegmentASCII[i]);
    }

    delay(500);
  }
}
