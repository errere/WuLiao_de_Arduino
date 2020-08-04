// PT6311B.h

#ifndef _PT6311B_h
#define _PT6311B_h



#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define ATMEGA328P
//#define STM32

#define GR8SG20 0b0000U
#define GR9SG19 0b1000U
#define GR10SG18 0b1001U
#define GR11SG17 0b1010U
#define GR12SG16 0b1011U
#define GR13SG15 0b1100U
#define GR14SG14 0b1101U
#define GR15SG13 0b1110U
#define GR16SG12 0b1111U

#define CMD_ModeSetting 0x00
#define CMD_DataSetting 0x40//0100 0000
#define CMD_AddressSetting 0xc0//1100 0000
#define CMD_DisplaySetting 0x80//1000 0000

#define OVERFLOW 1

class PT6311B
{
public:
	//stby,clk,in,out
	PT6311B(uint8_t, uint8_t, uint8_t, uint8_t);
	uint8_t init(uint8_t);
	uint8_t PMemSet(uint8_t, uint8_t);
private:
	uint8_t stby = 0;//low = enable
	uint8_t clk = 0;
	uint8_t dout = 0;//lcd->mcu
	uint8_t din = 0;//mcu->lcd

	uint8_t mode;
 
	void nextCMD(void);
	uint8_t sendBineary(uint8_t);

};


#endif

