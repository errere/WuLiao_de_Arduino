
#include "PT6311B.h"
PT6311B P(PB12, PB13, PB14, PB15);//uint8_t stby, uint8_t clk, uint8_t din, uint8_t dout
void setup()
{
	delay(1000);
	P.init(GR8SG20);

}

// Add the main program code into the continuous loop() function
void loop()
{

}



