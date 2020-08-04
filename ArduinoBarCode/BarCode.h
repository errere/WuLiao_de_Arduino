// arduinoQRcode.h

#ifndef _ARDUINOQRCODE_h
#define _ARDUINOQRCODE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define C_BLACK 1
#define C_WHITE 0

class BarCode
{
 public:
	 BarCode();

	 uint8_t EAN_NumberSystem = 0;
	 uint32_t EAN_MigCode = 0;
	 uint32_t EAN_ProductCode = 0;

	 uint8_t res_Split[12] = { 0 };

	 void DrawFromSplit(uint8_t *Dst);
	 void DrawFromLong(uint8_t *Dst);

private:
	
	uint8_t * DestPtr = 0;

	uint8_t EAN_CheakDigit = 0;

	uint8_t CodeDest[95] = { C_WHITE };

	const uint8_t prevCodex_Set_Black[6] PROGMEM = { 0,2,46,48,92,94 };

	const uint8_t codex_OD[10] PROGMEM = { 0b11111,0b10100, 0b10010, 0b10001, 0b01100, 0b00110, 0b00011, 0b01010, 0b01001, 0b00101 };//odd = 1,even = 0
	const uint8_t codex_LHA[10] PROGMEM = { 0b0001101,0b0011001, 0b0010011, 0b0111101, 0b0100011, 0b0110001, 0b0101111, 0b0111011, 0b0110111, 0b0001011 };//odd
	const uint8_t codex_LHB[10] PROGMEM = { 0b0100111,0b0110011, 0b0011011, 0b0100001, 0b0011101, 0b0111001, 0b0000101, 0b0010001, 0b0001001, 0b0010111 };//even
	const uint8_t codex_RHA[10] PROGMEM = { 0b1110010,0b1100110, 0b1101100, 0b1000010, 0b1011100, 0b1001110, 0b1010000, 0b1000100, 0b1001000, 0b1110100 };

	void init();

	void Split(uint8_t NumberSystem, uint32_t MigCode, uint32_t ProductCode, uint8_t * Dst);
	void WriteNum(uint8_t * code, const  uint8_t * CharSet, uint8_t num);
	uint8_t calcCheak(uint8_t * res);

};

extern BarCode Rcode;

#endif

