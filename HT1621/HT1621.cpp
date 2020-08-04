#include "HT1621.h"

HT1621::HT1621()
{
  this->__CSPIN__ = 0;
  this->__WRPIN__ = 0;
  this->__DATPIN__ = 0;
}

uint8_t HT1621::begin(uint8_t csPin, uint8_t wrPin, uint8_t datPin)
{
  this->__CSPIN__ = csPin;
  this->__WRPIN__ = wrPin;
  this->__DATPIN__ = datPin;
  __LCD_SET_GPIO_MODE__(this->__CSPIN__, __LCD_GPIO_OUTPUT);
  __LCD_SET_GPIO_MODE__(this->__WRPIN__, __LCD_GPIO_OUTPUT);
  __LCD_SET_GPIO_MODE__(this->__DATPIN__, __LCD_GPIO_OUTPUT);
  __LCD_DELAY_MS__(1);
  LCD_WRITE_CMD(BIAS);
  LCD_WRITE_CMD(RC);
  LCD_WRITE_CMD(SYSDIS);
  LCD_WRITE_CMD(WDTDIS);
  LCD_WRITE_CMD(SYSEN);
  LCD_WRITE_CMD(LCDON);
  return OK;
}

void HT1621::clear()
{

  uint8_t adr = 0;
  uint8_t addr = 0;
  uint8_t i;
  for (i = 0; i < 17; i++)
  {
    adr = addr;
    adr <<= 2;
    __LCD_SET_GPIO_WRITE__(this->__CSPIN__, 0);
    LCD_WRITE_DATA(0xa0, 3);
    LCD_WRITE_DATA(adr, 6);
    LCD_WRITE_DATA(0x00, 8);
    __LCD_SET_GPIO_WRITE__(this->__CSPIN__, 1);
    addr += 2;
  }
}

uint8_t HT1621::writeDig(uint8_t adr, uint8_t dat)
{
  if (adr >= 17)
    return ERROR;
  __LCD_SET_GPIO_WRITE__(this->__CSPIN__, 0);
  LCD_WRITE_DATA(0xa0, 3);
  LCD_WRITE_DATA((digSigMap[digMap[adr]] << 2), 6);
  LCD_WRITE_DATA(dat, 4);
  __LCD_SET_GPIO_WRITE__(this->__CSPIN__, 1);
  __LCD_DELAY_US__(4);
  __LCD_SET_GPIO_WRITE__(this->__CSPIN__, 0);
  LCD_WRITE_DATA(0xa0, 3);
  LCD_WRITE_DATA((digSigMap[digMap[adr] + 1] << 2), 6);
  LCD_WRITE_DATA((dat << 4), 4);
  __LCD_SET_GPIO_WRITE__(this->__CSPIN__, 1);
  return OK;
}

uint8_t HT1621::writeFomart(uint8_t adr, uint8_t dat) //abcdefg(h)
{
  uint8_t temp = 0;
  if (adr >= 17)
    return ERROR;
  if (adr < 13)
  {                                          //abc(h)fged
    temp |= (((dat & 0b00000001) ? 1 : 0)<<0);//a
    temp |= (((dat & 0b00000010) ? 1 : 0)<<1);//b
    temp |= (((dat & 0b00000100) ? 1 : 0)<<2);//c
    temp |= (((dat & 0b00001000) ? 1 : 0)<<7);//d
    temp |= (((dat & 0b00010000) ? 1 : 0)<<6);//e
    temp |= (((dat & 0b00100000) ? 1 : 0)<<4);//f
    temp |= (((dat & 0b01000000) ? 1 : 0)<<5);//g
    temp |= (((dat & 0b10000000) ? 1 : 0)<<3);//h
  }
  else if (adr == 13)
  { //degf(h)cba
    temp |= (((dat & 0b00000001) ? 1 : 0)<<7);//a
    temp |= (((dat & 0b00000010) ? 1 : 0)<<6);//b
    temp |= (((dat & 0b00000100) ? 1 : 0)<<5);//c
    temp |= (((dat & 0b00001000) ? 1 : 0)<<0);//d
    temp |= (((dat & 0b00010000) ? 1 : 0)<<1);//e
    temp |= (((dat & 0b00100000) ? 1 : 0)<<3);//f
    temp |= (((dat & 0b01000000) ? 1 : 0)<<2);//g
    temp |= (((dat & 0b10000000) ? 1 : 0)<<4);//h
  }
  else if (adr > 13)
  { //(h)decgfba
    temp |= (((dat & 0b00000001) ? 1 : 0)<<7);//a
    temp |= (((dat & 0b00000010) ? 1 : 0)<<6);//b
    temp |= (((dat & 0b00000100) ? 1 : 0)<<3);//c
    temp |= (((dat & 0b00001000) ? 1 : 0)<<1);//d
    temp |= (((dat & 0b00010000) ? 1 : 0)<<2);//e
    temp |= (((dat & 0b00100000) ? 1 : 0)<<5);//f
    temp |= (((dat & 0b01000000) ? 1 : 0)<<4);//g
    temp |= (((dat & 0b10000000) ? 1 : 0)<<0);//h
  }
  writeDig(adr,temp);
}

void HT1621::LCD_WRITE_DATA(uint8_t dat, uint8_t len)
{
  uint8_t i;
  for (i = 0; i < len; ++i)
  {
    __LCD_SET_GPIO_WRITE__(this->__WRPIN__, 0);
    __LCD_DELAY_US__(4);
    if (dat & 0x80)
    {
      __LCD_SET_GPIO_WRITE__(this->__DATPIN__, 1);
    }
    else
    {
      __LCD_SET_GPIO_WRITE__(this->__DATPIN__, 0);
    }
    __LCD_SET_GPIO_WRITE__(this->__WRPIN__, 1);
    __LCD_DELAY_US__(4);
    dat <<= 1;
  }
}
void HT1621::LCD_WRITE_CMD(uint8_t cmd)
{
  __LCD_SET_GPIO_WRITE__(this->__CSPIN__, 0);
  LCD_WRITE_DATA(0x80, 4);
  LCD_WRITE_DATA(cmd, 8);
  __LCD_SET_GPIO_WRITE__(this->__CSPIN__, 1);
}
