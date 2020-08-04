#include "AIP1650.h"

AIP1650::AIP1650()
{
}

void AIP1650::init(uint8_t Dat, uint8_t Clk, uint8_t Seg)
{
    this->PIN_DIO = Dat;
    this->PIN_CLK = Clk;
    this->SEG = Seg;

    pinMode(PIN_DIO, OUTPUT_OPEN_DRAIN); //1 = float,0 = pullDown
    pinMode(PIN_CLK, OUTPUT);

    digitalWrite(PIN_DIO, 0);
    digitalWrite(PIN_CLK, 0);

    this->Light = 8;

    if (SEG == LED_SG7)
        send16Bits(SYSON_SG7_L8);
    else if (SEG == LED_SG8)
        send16Bits(SYSON_SG8_L8);
}

//high
uint8_t AIP1650::Sleep()
{
    return send16Bits(SYS_SLEEEP);
}

uint8_t AIP1650::WakeUp()
{
    uint8_t Res;
    if (SEG == LED_SG7)
    {
        switch (this->Light)
        {
        case 1:
            Res = send16Bits(SYSON_SG7_L1);
            break;
        case 2:
            Res = send16Bits(SYSON_SG7_L2);
            break;
        case 3:
            Res = send16Bits(SYSON_SG7_L3);
            break;
        case 4:
            Res = send16Bits(SYSON_SG7_L4);
            break;
        case 5:
            Res = send16Bits(SYSON_SG7_L5);
            break;
        case 6:
            Res = send16Bits(SYSON_SG7_L6);
            break;
        case 7:
            Res = send16Bits(SYSON_SG7_L7);
            break;
        case 8:
            Res = send16Bits(SYSON_SG7_L8);
            break;

        default:
            Res = 255;
            break;
        } //switch

    } //if
    else if (SEG == LED_SG8)
    {
        switch (this->Light)
        {
        case 1:
            Res = send16Bits(SYSON_SG8_L1);
            break;
        case 2:
            Res = send16Bits(SYSON_SG8_L2);
            break;
        case 3:
            Res = send16Bits(SYSON_SG8_L3);
            break;
        case 4:
            Res = send16Bits(SYSON_SG8_L4);
            break;
        case 5:
            Res = send16Bits(SYSON_SG8_L5);
            break;
        case 6:
            Res = send16Bits(SYSON_SG8_L6);
            break;
        case 7:
            Res = send16Bits(SYSON_SG8_L7);
            break;
        case 8:
            Res = send16Bits(SYSON_SG8_L8);
            break;

        default:
            Res = 255;
            break;
        } //switch
    }     //else
    return Res;
}

uint8_t AIP1650::setLight(uint8_t L)
{
    uint8_t Res;
    this->Light = L;
    if (SEG == LED_SG7)
    {
        switch (L)
        {
        case 1:
            Res = send16Bits(SYSON_SG7_L1);
            break;
        case 2:
            Res = send16Bits(SYSON_SG7_L2);
            break;
        case 3:
            Res = send16Bits(SYSON_SG7_L3);
            break;
        case 4:
            Res = send16Bits(SYSON_SG7_L4);
            break;
        case 5:
            Res = send16Bits(SYSON_SG7_L5);
            break;
        case 6:
            Res = send16Bits(SYSON_SG7_L6);
            break;
        case 7:
            Res = send16Bits(SYSON_SG7_L7);
            break;
        case 8:
            Res = send16Bits(SYSON_SG7_L8);
            break;

        default:
            Res = 255;
            break;
        } //switch

    } //if
    else if (SEG == LED_SG8)
    {
        switch (L)
        {
        case 1:
            Res = send16Bits(SYSON_SG8_L1);
            break;
        case 2:
            Res = send16Bits(SYSON_SG8_L2);
            break;
        case 3:
            Res = send16Bits(SYSON_SG8_L3);
            break;
        case 4:
            Res = send16Bits(SYSON_SG8_L4);
            break;
        case 5:
            Res = send16Bits(SYSON_SG8_L5);
            break;
        case 6:
            Res = send16Bits(SYSON_SG8_L6);
            break;
        case 7:
            Res = send16Bits(SYSON_SG8_L7);
            break;
        case 8:
            Res = send16Bits(SYSON_SG8_L8);
            break;

        default:
            Res = 255;
            break;
        } //switch
    }     //else
    return Res;
}

uint8_t AIP1650::Write(uint8_t Dig, uint8_t Dat)
{
    uint8_t Res = 0;
    switch (Dig)
    {
    case 0:
        Res = send16Bits(CMD_DIG0, Dat);
        break;
    case 1:
        Res = send16Bits(CMD_DIG1, Dat);
        break;
    case 2:
        Res = send16Bits(CMD_DIG2, Dat);
        break;
    case 3:
        Res = send16Bits(CMD_DIG3, Dat);
        break;
    default:
        Res = 255;
        break;
    }
    return Res;
}

//mid
uint8_t AIP1650::send16Bits(uint8_t MSB8, uint8_t LSB8)
{
    uint8_t ACK = 0;
    sendStart();
    ACK += Send8bits(MSB8);
    ACK += Send8bits(LSB8);
    sendStop();
    return (ACK > 0) ? ACK_NOACK : ACK_ACKOK;
}

uint8_t AIP1650::send16Bits(uint16_t Dat16)
{
    uint8_t ACK = 0;
    sendStart();
    ACK += Send8bits((Dat16 >> 8) & 0xff);
    ACK += Send8bits(Dat16 & 0xff);
    sendStop();
    return (ACK > 0) ? ACK_NOACK : ACK_ACKOK;
}
//low
void AIP1650::CLKSend()
{
    digitalWrite(PIN_CLK, 1);
    delayMicroseconds(2);
    digitalWrite(PIN_CLK, 0);
}

void AIP1650::sendStart()
{
    digitalWrite(PIN_DIO, 1);
    delayMicroseconds(2);
    digitalWrite(PIN_CLK, 1);
    delayMicroseconds(2);
    digitalWrite(PIN_DIO, 0);
    delayMicroseconds(2);
    digitalWrite(PIN_CLK, 0);
}

void AIP1650::sendStop()
{
    digitalWrite(PIN_DIO, 0);
    delayMicroseconds(2);
    digitalWrite(PIN_CLK, 1);
    delayMicroseconds(2);
    digitalWrite(PIN_DIO, 1);
    delayMicroseconds(2);
    digitalWrite(PIN_CLK, 0);
    delayMicroseconds(2);
    digitalWrite(PIN_DIO, 0);
}

uint8_t AIP1650::getACK()
{
    return digitalRead(PIN_DIO);
}

uint8_t AIP1650::Send8bits(uint8_t dat)
{
    uint8_t tmp = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        tmp = ((dat << i) & 0x80) ? 1 : 0; //MSB Frist
        digitalWrite(PIN_DIO, tmp);
        delayMicroseconds(2);
        CLKSend();
    }
    digitalWrite(PIN_DIO, 1);
    delayMicroseconds(2);
    digitalWrite(PIN_CLK, 1);
    delayMicroseconds(2);
    uint8_t ack = getACK();
    digitalWrite(PIN_CLK, 0);
    return ack;
}
