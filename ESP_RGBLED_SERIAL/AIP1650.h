#ifndef AIP1650_h
#define AIP1650_h

#include <Arduino.h>
#include <inttypes.h>

#define ACK_ACKOK 0
#define ACK_NOACK 1

#define LED_SG7 7
#define LED_SG8 8

#define SYSOFF 0x4800
#define SYS_SLEEEP 0x4804

#define SYSON_SG8_L1 0x4811
#define SYSON_SG8_L2 0x4821
#define SYSON_SG8_L3 0x4831
#define SYSON_SG8_L4 0x4841
#define SYSON_SG8_L5 0x4851
#define SYSON_SG8_L6 0x4861
#define SYSON_SG8_L7 0x4871
#define SYSON_SG8_L8 0x4801

#define SYSON_SG7_L1 0x4819
#define SYSON_SG7_L2 0x4829
#define SYSON_SG7_L3 0x4839
#define SYSON_SG7_L4 0x4849
#define SYSON_SG7_L5 0x4859
#define SYSON_SG7_L6 0x4869
#define SYSON_SG7_L7 0x4879
#define SYSON_SG7_L8 0x4809

#define CMD_DIG0 0x68
#define CMD_DIG1 0x6a
#define CMD_DIG2 0x6c
#define CMD_DIG3 0x6e

class AIP1650
{
private:
    uint8_t PIN_DIO;
    uint8_t PIN_CLK;
    uint8_t SEG;   //7,8
    uint8_t Light; //1->8

    //mid
    uint8_t send16Bits(uint8_t MSB8, uint8_t LSB8);
    uint8_t send16Bits(uint16_t Dat16);

    //low
    void CLKSend();
    void sendStart();
    void sendStop();

    uint8_t Send8bits(uint8_t dat);
    uint8_t getACK();

public:
    AIP1650();
    void init(uint8_t Dat, uint8_t Clk, uint8_t Seg);

    //high
    uint8_t Sleep();
    uint8_t WakeUp();
    uint8_t setLight(uint8_t L);
    uint8_t Write(uint8_t Dig, uint8_t Dat);
};

#endif
