
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
	pinMode(din, OUTPUT); //mcu->lcd
	pinMode(dout, INPUT); //lcd->mcu

	digitalWrite(this->stby, 0);
	digitalWrite(this->din, 0);
	digitalWrite(this->clk, 1);
}

uint8_t PT6311B::init(uint8_t mode = GR15SG13)
{
	this->mode = mode;
	nextCMD();
	sendBineary(CMD_ModeSetting | this->mode); //cmd1

	nextCMD();
	sendBineary(CMD_DataSetting | 0x04); ////fixed addr//cmd2

	for (uint8_t i = 0; i < 0x2f; i++)
	{
		nextCMD();
		sendBineary(CMD_AddressSetting | i); //cmd3
		delayMicroseconds(1);
		sendBineary(0x00);
	}
	nextCMD();
	sendBineary(CMD_DisplaySetting | 0x0f); //start display//cmd4
	return 0;
}
//high
uint16_t PT6311B::AsciiToSegChar(char Ch)
{
	if (Ch < 0x20)
		return FourteenSegmentASCII[0];
	if (Ch <= 0x7f)
	{
		return FourteenSegmentASCII[(Ch - 0x1f)];
	}
	return FourteenSegmentASCII[0];
}

//mid
uint8_t PT6311B::PMemSet(uint8_t ADR, uint8_t val)
{
	nextCMD();
	sendBineary(CMD_ModeSetting | this->mode); //cmd1
	nextCMD();
	sendBineary(CMD_DataSetting | 0x04); //cmd2
	nextCMD();
	sendBineary(CMD_AddressSetting | ADR); //cmd3
	delayMicroseconds(1);
	sendBineary(val);
	nextCMD();
	sendBineary(CMD_DisplaySetting | 0x0f); //start display//cmd4//14/16
	return 0;
}

uint8_t PT6311B::Send20Bits(uint8_t HeadAdr, uint32_t Var)
{
	this->PMemSet(HeadAdr, (Var & 0xffU));
	this->PMemSet(HeadAdr + 1, ((Var & 0xff00U) >> 8));
	this->PMemSet(HeadAdr + 2, ((Var & 0xf0000UL) >> 16));
	return 0;
}

//low
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
	} //for
	return 0;
}
