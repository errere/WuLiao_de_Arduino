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

	const uint8_t VFDCharAdr[8] = {0x00,0x03,0x06,0x09,0x0c,0x0f,0x12,0x15};
	const uint8_t VFDPointAdr[8] = {0x01,0x04,0x07,0x0a,0x0d,0x10,0x13,0x16};
	const uint8_t VFDCharMap[16] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

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

