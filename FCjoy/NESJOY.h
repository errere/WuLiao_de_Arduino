// NESJOY.h

#ifndef _NESJOY_h
#define _NESJOY_h

#define Latch_P 12
#define Sleep_Pre_Read 6
#define Reading 6

//#define STM32

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct Joy
{
	bool A;
	bool B;
	bool SEL;
	bool START;
	bool UP;
	bool DOWN;
	bool LEFT;
	bool RIGHT;
};

class NESJOY
{
 public:

	 

	NESJOY(uint8_t D, uint8_t L, uint8_t C);
	void init();
	Joy Read();
private:
	uint8_t Data, Latch, Clock;
};


#endif

