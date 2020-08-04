// 
// 
// 

#include "BarCode.h"

BarCode::BarCode()
{
}

void BarCode::init()
{
	this->DestPtr = CodeDest;
	memset(this->CodeDest, 0x0, sizeof(CodeDest));
	for (uint8_t i = 0; i < 6; i++)
	{
		CodeDest[prevCodex_Set_Black[i]] = C_BLACK;
	}
	this->DestPtr += 3;
}

void BarCode::Split(uint8_t NumberSystem, uint32_t MigCode, uint32_t ProductCode, uint8_t * Dst)
{
	Dst[0] = NumberSystem / 10;
	Dst[1] = NumberSystem % 10;

	for (uint8_t i = 0; i < 5; i++)
	{
		Dst[i + 2] = (MigCode / (uint16_t)pow(10, 4 - i)) % 10;
	}

	for (uint8_t i = 0; i < 5; i++)
	{
		Dst[i + 7] = (ProductCode / (uint16_t)pow(10, 4 - i)) % 10;
	}
	
}

void BarCode::WriteNum(uint8_t * code, const uint8_t * CharSet, uint8_t num)
{
	for (uint8_t i = 0; i < 7; i++)
	{
		*this->DestPtr = ((CharSet[num] & (0b1000000 >> i)) ? C_BLACK : C_WHITE);
		this->DestPtr++;
	}
}

uint8_t BarCode::calcCheak(uint8_t * res)
{
	//1.�����ұ�һ����λ��Ϊ��������λ����������Ϊÿ���ַ�ָ������/ż��λ��
	uint8_t tmp1 = res[11] + res[9] + res[7] + res[5] + res[3] + res[1];
	tmp1 = tmp1 * 3;//2.����������λ�ϵ���ֵ��ͣ����ṹ����3��
	uint8_t tmp2 = res[10] + res[8] + res[6] + res[4] + res[2] + res[0];//3.������ż��λ�ϵ���ֵ��͡�
	tmp1 += tmp2;//4.�Ե�2���͵�3������Ľ����͡�
	//5.У��λ�����ּ����õ�4��������ܺ���Ӧ���ܹ���10������
	//6.�����4��������ܺ����ܹ���10������У��λ���ǡ�0��������10��
	tmp2 = (10 - (tmp1 % 10));
	if (tmp2 >= 10)tmp2 = 0;
	return tmp2;
}

void BarCode::DrawFromSplit(uint8_t *Dst)
{
	this->init();
	EAN_CheakDigit = this->calcCheak(res_Split);
	WriteNum(CodeDest, this->codex_LHA, res_Split[1]);

	for (uint8_t i = 0; i < 5; i++)
	{

		if (codex_OD[res_Split[0]] & (0b10000 >> i))WriteNum(CodeDest, this->codex_LHA, res_Split[i + 2]);//odd
		else WriteNum(CodeDest, this->codex_LHB, res_Split[i + 2]);//even
	}

	DestPtr += 5;

	for (uint8_t i = 0; i < 5; i++)
	{
		WriteNum(CodeDest, this->codex_RHA, res_Split[i + 7]);
	}
	WriteNum(CodeDest, this->codex_RHA, EAN_CheakDigit);

	memcpy(Dst, this->CodeDest, 95 * sizeof(uint8_t));
}

void BarCode::DrawFromLong(uint8_t *Dst)
{
	this->Split(EAN_NumberSystem, EAN_MigCode, EAN_ProductCode, res_Split);
	this->DrawFromSplit(Dst);
}




BarCode Rcode;


