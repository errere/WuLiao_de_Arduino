#ifndef HT1621_H_
#define HT1621_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//#define _ON_VSCODE
#define DEBUG

#define ERROR 1
#define OK 0

//#define BIAS 0x52   //0b1000 0101 0010  1/3duty 4com   // 1000 010a bxcx
#define BIAS 0b01010000     // 1000 010a bxcx
#define SYSDIS 0X00 //0b1000 0000 0000  关振荡器和LCD偏压发生器
#define SYSEN 0X02  //0b1000 0000 0010  开振荡器
#define LCDOFF 0X04 //0b1000 0000 0100  关LCD偏压
#define LCDON 0X06  //0b1000 0000 0110  开LCD偏压
#define RC 0X30     //0b1000 0011 0000  内部时钟
#define WDTDIS 0X0A //0b1000 0000 1010  禁止看门狗

#define __LCD_GPIO_OUTPUT OUTPUT
#define __LCD_GPIO_INPUT INPUT

#define __LCD_SET_GPIO_MODE__(x, y) pinMode(x, y)
#define __LCD_SET_GPIO_WRITE__(x, y) digitalWrite(x, y)
#define __LCD_DELAY_US__(x) delayMicroseconds(x)
#define __LCD_DELAY_MS__(x) delay(x)

/********************************************************************************/
#ifdef _ON_VSCODE
typedef unsigned char uint8_t;
typedef char int8_t;

typedef unsigned int uint16_t;
typedef int int16_t;

typedef unsigned long uint32_t;
typedef long int32_t;
#endif
/********************************************************************************/

class HT1621
{
public:
  HT1621();
  uint8_t begin(uint8_t, uint8_t, uint8_t);
  void clear();
  uint8_t writeDig(uint8_t adr, uint8_t dat);
  uint8_t writeFomart(uint8_t adr, uint8_t dat);

private:
  const uint8_t digMap[17] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32}; //13
  const uint8_t digSigMap[34] = {26, 24, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 28, 29, 30, 31, 32, 33};

  void LCD_WRITE_DATA(uint8_t dat, uint8_t len);
  void LCD_WRITE_CMD(uint8_t cmd);

  uint8_t __CSPIN__;
  uint8_t __WRPIN__;
  uint8_t __DATPIN__;
};

#endif
