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
	//������������һ��LATCH�����ź����壬�������Ŀ��Ϊ12us�������ֱ���ʼ��鰴��״̬��
	digitalWrite(Latch, 1);
	delayMicroseconds(Latch_P);
	digitalWrite(Latch, 0);
	//��LATCH�����巢�ͺ���6us
	delayMicroseconds(Reading);
	//CLOCK��PULSE���߿�ʼ��������Ϊ12us��ռ�ձ�50%�������źţ�һ����8�Ρ�
	//ÿ�ε�����������ض�DATA�߲��������DATA���Ƿ��ڸ�λ�ñ����͡�
	//����������˳���ǹ̶��ģ���Ϸ�����ʱ�������Ա�̶��ģ�������˳��ΪA, B, SEL, START, �������ҡ�
	//������������£���ô����λ�õ�DATA�ǵ͵�ƽ��

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

