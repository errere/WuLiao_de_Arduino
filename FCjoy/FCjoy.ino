
#include "NESJOY.h"
NESJOY nes(7, 8, 9);
Joy j;
void setup()
{
	nes.init();
	Serial.begin(9600);
}

// Add the main program code into the continuous loop() function
void loop()
{
	j = nes.Read();
	char c[100] = { ' ' };
#ifdef STM32

#else
	snprintf(c, 100, "A:%d, B:%d, SEL:%d, START:%d, UP:%d, DOWN:%d, LEFT:%d, RIGHT:%d \n", j.A, j.B, j.SEL, j.START, j.UP, j.DOWN, j.LEFT, j.RIGHT);
	Serial.print(c);
#endif // STM32

	delay(100);
}
