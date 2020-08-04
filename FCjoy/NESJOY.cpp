// 
// 
// 

#include "NESJOY.h"

NESJOY::NESJOY(uint8_t D, uint8_t L, uint8_t C)
{ 
	Data = D;
	Latch = L;
	Clock = C;
}

void NESJOY::init()
{
#ifdef STM32
	pinMode(Data, INPUT);
	pinMode(Latch, OUTPUT);
	pinMode(Clock, OUTPUT);
#else
	pinMode(Data, 0);
	pinMode(Latch, 1);
	pinMode(Clock, 1);
#endif // STM32

	
	digitalWrite(Latch, 0);
	digitalWrite(Clock, 0);
}

Joy NESJOY::Read()
{
	Joy joy;
	//首先主机发送一个LATCH锁存信号脉冲，这个脉冲的宽度为12us。告诉手柄开始检查按键状态。
	digitalWrite(Latch, 1);
	delayMicroseconds(Latch_P);
	digitalWrite(Latch, 0);
	//在LATCH的脉冲发送后间隔6us
	delayMicroseconds(Reading);
	//CLOCK（PULSE）线开始发送周期为12us，占空比50%的脉冲信号，一共发8次。
	//每次的脉冲的上升沿对DATA线采样，检查DATA线是否在该位置被拉低。
	//按键被检查的顺序是固定的（游戏机设计时候设计人员固定的），按键顺序为A, B, SEL, START, 上下左右。
	//如果按键被按下，那么对于位置的DATA是低电平。

	joy.A = !digitalRead(Data);
	
	digitalWrite(Clock, 1);
	joy.B = !digitalRead(Data);
	delayMicroseconds(Reading);
	digitalWrite(Clock, 0);
	delayMicroseconds(Reading);

	digitalWrite(Clock, 1);
	joy.SEL = !digitalRead(Data);
	delayMicroseconds(Reading);
	digitalWrite(Clock, 0);
	delayMicroseconds(Reading);

	digitalWrite(Clock, 1);
	joy.START = !digitalRead(Data);
	delayMicroseconds(Reading);
	digitalWrite(Clock, 0);
	delayMicroseconds(Reading);

	digitalWrite(Clock, 1);
	joy.UP = !digitalRead(Data);
	delayMicroseconds(Reading);
	digitalWrite(Clock, 0);
	delayMicroseconds(Reading);

	digitalWrite(Clock, 1);
	joy.DOWN = !digitalRead(Data);
	delayMicroseconds(Reading);
	digitalWrite(Clock, 0);
	delayMicroseconds(Reading);

	digitalWrite(Clock, 1);
	joy.LEFT = !digitalRead(Data);
	delayMicroseconds(Reading);
	digitalWrite(Clock, 0);
	delayMicroseconds(Reading);

	digitalWrite(Clock, 1);
	joy.RIGHT = !digitalRead(Data);
	delayMicroseconds(Reading);
	digitalWrite(Clock, 0);
	return joy;
}

