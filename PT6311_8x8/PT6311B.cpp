// 
// 
// 

#include "PT6311B.h"

//stby,clk,in,out
PT6311B::PT6311B(uint8_t stby, uint8_t clk, uint8_t din, uint8_t dout)
{
	this->stby = stby;
	this->clk = clk;
	this->din = din;
	this->dout = dout;
	pinMode(stby, OUTPUT);
	pinMode(clk, OUTPUT);
	pinMode(din, OUTPUT);//mcu->lcd
	pinMode(dout, INPUT);//lcd->mcu

	digitalWrite(this->stby, 0);
	digitalWrite(this->din, 0);
	digitalWrite(this->clk, 1);
}

/*When Power is turned ��ON��, the 16-digit, 12-segment modes is selected. */
uint8_t PT6311B::init(uint8_t mode = GR15SG13)
{
	this->mode = mode;
	nextCMD();
	sendBineary(CMD_ModeSetting | this->mode);//cmd1

	nextCMD();
	sendBineary(CMD_DataSetting | 0x04);////fixed addr//cmd2

	for (uint8_t i = 0; i < 30; i++)
	{
		nextCMD();
		sendBineary(CMD_AddressSetting | i);//cmd3
		delayMicroseconds(1);
		sendBineary(0x00);
	}
	nextCMD();
	sendBineary(CMD_DisplaySetting | 0x0f);//start display//cmd4
	return 0;
}

uint8_t PT6311B::PMemSet(uint8_t ADR, uint8_t val)
{
	nextCMD();
	sendBineary(CMD_ModeSetting | this->mode);//cmd1
	nextCMD();
	sendBineary(CMD_DataSetting | 0x04);//cmd2
	nextCMD();
	sendBineary(CMD_AddressSetting | ADR);//cmd3
	delayMicroseconds(1);
	sendBineary(val);
	nextCMD();
	sendBineary(CMD_DisplaySetting | 0x0f);//start display//cmd4//14/16
	return 0;
}


void PT6311B::nextCMD(void)
{
	digitalWrite(this->stby, 1);
	delayMicroseconds(1);
	digitalWrite(this->stby, 0);
}

/*lsb->msb*/
/*b0 = lsb,b7 = msb*/
uint8_t PT6311B::sendBineary(uint8_t cmd)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		digitalWrite(this->clk, 0);
		//delayMicroseconds(1);
		digitalWrite(this->din, cmd & 0x1);
		cmd >>= 1;
		delayMicroseconds(1);
		digitalWrite(this->clk, 1);
	}//for	
	return 0;
}

