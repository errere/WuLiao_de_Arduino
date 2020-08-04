
#include "BarCode.h"

BarCode C;
void setup()
{
	Serial.begin(9600);
	uint8_t as[95] = { 0 };
	C.EAN_NumberSystem = 23;
	C.EAN_MigCode = 58741;
	C.EAN_ProductCode = 63205;
	C.DrawFromLong(as);
	for (int i = 0; i < 95; i++)
	{
		Serial.print((int)as[i]);
	}
	
}

// Add the main program code into the continuous loop() function
void loop()
{


}
