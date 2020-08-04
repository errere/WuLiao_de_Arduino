// PT6311B.h

#ifndef _PT6311B_h
#define _PT6311B_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>

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
#define CMD_DataSetting 0x40	//0100 0000
#define CMD_AddressSetting 0xc0 //1100 0000
#define CMD_DisplaySetting 0x80 //1000 0000

#define OVERFLOW 1

/*https://github.com/dmadison/LED-Segment-ASCII*/
/*
    Project     Segmented LED Display - ASCII Library
    @author     David Madison
    @link       github.com/dmadison/Segmented-LED-Display-ASCII
    @license    MIT - Copyright (c) 2017 David Madison

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

*/
const uint16_t FourteenSegmentASCII[] = {
	/*0x00*/
	0x0, /* (ctrl) */
	/*0x19*/
	0x0, /* (space) */
	/*0x1a*/
	0x0, /* ! */
	/*0x1b*/
	0x404,	/* " */
	0x259c, /* # */
	0x25da, /* $ */
	0x7fc8, /* % */
	0x46b2, /* & */
	0x400,	/* ' */
	0x4800, /* ( */
	0x1200, /* ) */
	0x7f80, /* * */
	0x2580, /* + */
	0x1000, /* , */
	0x180,	/* - */
	/*0x2d*/
	0x00, /* . */
	/*0x2e*/
	0x1800, /* / */
	/*0x2f*/
	0x187e, /* 0 */
	0x80c,	/* 1 */
	0x1b6,	/* 2 */
	0x11e,	/* 3 */
	0x1cc,	/* 4 */
	0x40d2, /* 5 */
	0x1fa,	/* 6 */
	0xe,	/* 7 */
	0x1fe,	/* 8 */
	0x1de,	/* 9 */
	/*0x39*/
	0x2400, /* : */
	0x1400, /* ; */
	0x4880, /* < */
	0x190,	/* = */
	0x1300, /* > */
	0x00,	/* ? */
	/*0x3f*/
	0x576, /* @ */
	/*0x41*/
	0x1ee,	/* A */
	0x251e, /* B */
	0x72,	/* C */
	0x241e, /* D */
	0xf2,	/* E */
	0xe2,	/* F */
	0x17a,	/* G */
	0x1ec,	/* H */
	0x2412, /* I */
	0x3c,	/* J */
	0x48e0, /* K */
	0x70,	/* L */
	0xa6c,	/* M */
	0x426c, /* N */
	0x7e,	/* O */
	0x1e6,	/* P */
	0x407e, /* Q */
	0x41e6, /* R */
	0x1da,	/* S */
	0x2402, /* T */
	0x7c,	/* U */
	0x1860, /* V */
	0x506c, /* W */
	0x5a00, /* X */
	0x1dc,	/* Y */
	0x1812, /* Z */
	/*0x5a*/
	0x72,	/* [ */
	0x4200, /* \ */
	0x1e,	/* ] */
	0x5000, /* ^ */
	0x10,	/* _ */
	0x200,	/* ` */
	/*0x60*/
	0x20b0, /* a */
	0x40f0, /* b */
	0x1b0,	/* c */
	0x111c, /* d */
	0x10b0, /* e */
	0x2980, /* f */
	0x91c,	/* g */
	0x20e0, /* h */
	0x2000, /* i */
	0x1420, /* j */
	0x6c00, /* k */
	0x60,	/* l */
	0x21a8, /* m */
	0x20a0, /* n */
	0x1b8,	/* o */
	0x2e0,	/* p */
	0x90c,	/* q */
	0xa0,	/* r */
	0x4110, /* s */
	0xf0,	/* t */
	0x38,	/* u */
	0x1020, /* v */
	0x5028, /* w */
	0x5a00, /* x */
	0x51c,	/* y */
	0x1090, /* z */
	/*0x7a*/
	0x1292, /* { */
	0x2400, /* | */
	0x4912, /* } */
	0x1980, /* ~ */
	/*0x7e*/
	0x00 /* (backSpace) */
		 /*0x7f*/
};

class PT6311B
{
public:
	//stby,clk,in,out
	PT6311B(uint8_t, uint8_t, uint8_t, uint8_t);
	uint8_t init(uint8_t);
	uint8_t PMemSet(uint8_t, uint8_t);
	uint8_t Send20Bits(uint8_t HeadAdr, uint32_t Var);

	uint16_t AsciiToSegChar(char Ch);

private:
	uint8_t stby = 0; //low = enable
	uint8_t clk = 0;
	uint8_t dout = 0; //lcd->mcu
	uint8_t din = 0;  //mcu->lcd

	uint8_t mode;

	void nextCMD(void);
	uint8_t sendBineary(uint8_t);
};

#endif
