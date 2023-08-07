#include "ad9833.h"

uint8_t freq_number_1=0;//�趨AD9833_1��Ƶ������
uint8_t phase_number_1=0;//�趨AD9833_1����λ����

uint8_t freq_number_2=0;//�趨AD9833_2��Ƶ������
uint8_t phase_number_2=0;//�趨AD9833_2����λ����

void Init_AD9833(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	FSYNC1_1();	/* FSYNC1 = 1 */
	FSYNC2_1();	/* FSYNC2 = 1 */

	/* ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	/* ���ü������������IO */
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* ��Ϊ���������ģʽ�� */
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

//	GPIO_InitStructure.GPIO_Pin = PIN_SCLK;
//	GPIO_Init(PORT_SCLK, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = PIN_SDATA;
//	GPIO_Init(PORT_SDATA, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = PIN_FSYNC;
//	GPIO_Init(PORT_FSYNC, &GPIO_InitStructure);


  GPIO_InitStructure.GPIO_Pin = PIN_SCLK_1 | PIN_SDATA_1 | PIN_FSYNC_1 | PIN_SCLK_2 | PIN_SDATA_2 | PIN_FSYNC_2; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 

	GPIO_Init(PORT_SCLK_1, &GPIO_InitStructure);
	GPIO_Init(PORT_SDATA_1, &GPIO_InitStructure);
	GPIO_Init(PORT_FSYNC_1, &GPIO_InitStructure);
	GPIO_Init(PORT_SCLK_2, &GPIO_InitStructure);
	GPIO_Init(PORT_SDATA_2, &GPIO_InitStructure);
	GPIO_Init(PORT_FSYNC_2, &GPIO_InitStructure);
	
	
	ResetAD9833_1();
	ResetAD9833_2();//��λ
	AD9833_1_Write_16Bits(0x2100); 
	AD9833_2_Write_16Bits(0x2100); //ѡ������һ��д��//ѡ������һ��д��
}

/*
********************************************************
*	�� �� ��: AD9833_1_Write_16Bits      
*	����˵��: ��SPI���߷���16��bit���� ���Ϳ�����
*	��    ��: _cmd : ��
*	�� �� ֵ����
*******************************************************
*/
void AD9833_1_Write_16Bits(uint16_t _cmd)
{
	uint8_t i;
	SCLK1_1(); //��ʱ���������²���
	FSYNC1_1();
	FSYNC1_0();	//Ƭѡ��
	/* AD9833  SCLKʱ��25M,���Բ���ʱ */
	for(i = 0; i < 16; i++)
	{
		if (_cmd & 0x8000)
		{
			SDATA1_1();
		}
		else
		{
			SDATA1_0();
		}
		SCLK1_0();
		_cmd <<= 1;
		SCLK1_1();
	}	
	FSYNC1_1();//Ƭѡ�ر�
	SCLK1_0();
}

void AD9833_1_SelectWave(uint8_t freqRegister, uint8_t phaseRegister, uint8_t _Type) 
{
	  uint16_t square=0x2028;
	  uint16_t triangle=0x2002;
	  uint16_t sine=0x2000;
	  uint16_t null=0x00c0;
	
	  FSYNC1_1();
    SCLK1_1();
	  switch(_Type)
		{
			case 1:
				AD9833_1_Write_16Bits(square|(freqRegister<<12)|(phaseRegister<<11)); /*Ƶ�ʼĴ���freqRegister��λ�Ĵ���phaseRegister�������*/
			  break;
			case 2:
				AD9833_1_Write_16Bits(triangle|(freqRegister<<12)|(phaseRegister<<11)); /*Ƶ�ʼĴ���freqRegister��λ�Ĵ���phaseRegister������ǲ�*/
			  break;
			case 3:
				AD9833_1_Write_16Bits(sine|(freqRegister<<12)|(phaseRegister<<11)); /*Ƶ�ʼĴ���freqRegister��λ�Ĵ���phaseRegister������Ҳ�*/
			  break;
			case 4:
				AD9833_1_Write_16Bits(null|(freqRegister<<12)|(phaseRegister<<11)); /*Ƶ�ʼĴ���freqRegister��λ�Ĵ���phaseRegister�����*/
			  break;
		}
}

void AD9833_1_SetFreq(uint32_t _freq)
{
	uint32_t freq;
	uint16_t lsb_14bit;
	uint16_t msb_14bit;

	freq = (uint32_t)(268435456.0 / AD9833_SYSTEM_CLOCK * _freq);
	lsb_14bit = (uint16_t)freq;
	msb_14bit = (uint16_t)(freq >> 14);
	if(freq_number_1 != 1)
	{
		lsb_14bit &= ~(1U<<15);//0111 1111 1111 1111 �Ȱѵ�15λ��0������λ���� �������
		lsb_14bit |= 1<<14;    //0100 0000 0000 0000 �ٰѵ�14λ��1������λ���� �������01xx xxxx xxxx xxxx
		msb_14bit &= ~(1U<<15); //ͬ��
		msb_14bit |= 1<<14;
		freq_number_1 = 1;
	}
	else if(freq_number_1 == 1)
	{
		lsb_14bit &= ~(1U<<14); //1011 1111 1111 1111 �Ȱѵ�14λ��0������λ����
		lsb_14bit |= 1U<<15;   //1000 0000 0000 0000 �ٰѵ�15λ��1������λ���� �������10xx xxxx xxxx xxxx
		msb_14bit &= ~(1<<14); //ͬ��
		msb_14bit |= 1U<<15;
		freq_number_1++;
	}
	AD9833_1_Write_16Bits(lsb_14bit);
	AD9833_1_Write_16Bits(msb_14bit);
}

void AD9833_1_SetPHASE(uint32_t _phase)
{
	uint32_t phase;
	uint16_t lsb_12bit;
	uint16_t msb_12bit;
	
	phase = (uint32_t)(4096*_phase/2/pi);
	lsb_12bit = (uint16_t)phase;
	msb_12bit = (uint16_t)(phase >> 12);
	if(phase_number_1 != 1 )
	{
		lsb_12bit &= ~(1U<<13);//1101 1111 1111 1111 �Ȱѵ�13λ��0������λ����
		lsb_12bit |= 1U<<14;//0100 0000 0000 0000 �ѵ�14λ��1������λ����
		lsb_12bit |= 1U<<15;//1000 0000 0000 0000 �ѵ�15λ��1������λ����
		msb_12bit &= ~(1U<<13); //ͬ��
		msb_12bit |= 1U<<14;//
		msb_12bit |= 1U<<15;//
		phase_number_1 = 1;
	}
	else if(phase_number_1 == 1)
	{
		lsb_12bit |= 1U<<13; //1011 1111 1111 1111 �Ȱѵ�13λ��1������λ����
		lsb_12bit |= 1U<<14; //1000 0000 0000 0000 �ٰѵ�14λ��1������λ����
		lsb_12bit |= 1U<<15; //1000 0000 0000 0000 �ѵ�15λ��1������λ����
		msb_12bit |= 1U<<13; //ͬ��
		msb_12bit |= 1U<<14; //
		msb_12bit |= 1U<<15; //
		phase_number_1++;
	}
	AD9833_1_Write_16Bits(lsb_12bit);
	AD9833_1_Write_16Bits(msb_12bit);
}

void ResetAD9833_1()
{
	AD9833_1_Write_16Bits(0x0100);
	freq_number_1=0;
	phase_number_1=0;
}










/*
********************************************************
*	�� �� ��: AD9833_2_Write_16Bits      
*	����˵��: ��SPI���߷���16��bit���� ���Ϳ�����
*	��    ��: _cmd : ��
*	�� �� ֵ����
*******************************************************
*/
void AD9833_2_Write_16Bits(uint16_t _cmd)
{
	uint8_t i;
	SCLK2_1(); //��ʱ���������²���
	FSYNC2_1();
	FSYNC2_0();	//Ƭѡ��
	/* AD9833  SCLKʱ��25M,���Բ���ʱ */
	for(i = 0; i < 16; i++)
	{
		if (_cmd & 0x8000)
		{
			SDATA2_1();
		}
		else
		{
			SDATA2_0();
		}
		SCLK2_0();
		_cmd <<= 1;
		SCLK2_1();
	}	
	FSYNC2_1();//Ƭѡ�ر�
	SCLK2_0();
}

void AD9833_2_SelectWave(uint8_t freqRegister, uint8_t phaseRegister, uint8_t _Type) 
{
	  uint16_t square=0x2028;
	  uint16_t triangle=0x2002;
	  uint16_t sine=0x2000;
	  uint16_t null=0x00c0;
	
	  FSYNC2_1();
    SCLK2_1();
	  switch(_Type)
		{
			case 1:
				AD9833_2_Write_16Bits(square|(freqRegister<<12)|(phaseRegister<<11)); /*Ƶ�ʼĴ���freqRegister��λ�Ĵ���phaseRegister�������*/
			  break;
			case 2:
				AD9833_2_Write_16Bits(triangle|(freqRegister<<12)|(phaseRegister<<11)); /*Ƶ�ʼĴ���freqRegister��λ�Ĵ���phaseRegister������ǲ�*/
			  break;
			case 3:
				AD9833_2_Write_16Bits(sine|(freqRegister<<12)|(phaseRegister<<11)); /*Ƶ�ʼĴ���freqRegister��λ�Ĵ���phaseRegister������Ҳ�*/
			  break;
			case 4:
				AD9833_2_Write_16Bits(null|(freqRegister<<12)|(phaseRegister<<11)); /*Ƶ�ʼĴ���freqRegister��λ�Ĵ���phaseRegister�����*/
			  break;
		}
}

void AD9833_2_SetFreq(uint32_t _freq)
{
	uint32_t freq;
	uint16_t lsb_14bit;
	uint16_t msb_14bit;

	freq = (uint32_t)(268435456.0 / AD9833_SYSTEM_CLOCK * _freq);
	lsb_14bit = (uint16_t)freq;
	msb_14bit = (uint16_t)(freq >> 14);
	if(freq_number_2 != 1)
	{
		lsb_14bit &= ~(1U<<15);//0111 1111 1111 1111 �Ȱѵ�15λ��0������λ���� �������
		lsb_14bit |= 1<<14;    //0100 0000 0000 0000 �ٰѵ�14λ��1������λ���� �������01xx xxxx xxxx xxxx
		msb_14bit &= ~(1U<<15); //ͬ��
		msb_14bit |= 1<<14;
		freq_number_2 = 0;
	}
	else if(freq_number_2 == 1)
	{
		lsb_14bit &= ~(1U<<14); //1011 1111 1111 1111 �Ȱѵ�14λ��0������λ����
		lsb_14bit |= 1U<<15;   //1000 0000 0000 0000 �ٰѵ�15λ��1������λ���� �������10xx xxxx xxxx xxxx
		msb_14bit &= ~(1<<14); //ͬ��
		msb_14bit |= 1U<<15;
		freq_number_2++;
	}
	AD9833_2_Write_16Bits(lsb_14bit);
	AD9833_2_Write_16Bits(msb_14bit);
}

void AD9833_2_SetPHASE(uint32_t _phase)
{
	uint32_t phase;
	uint16_t lsb_12bit;
	uint16_t msb_12bit;
	
	phase = (uint32_t)(4096*_phase/2/pi);
	lsb_12bit = (uint16_t)phase;
	msb_12bit = (uint16_t)(phase >> 12);
	if(phase_number_2 != 1 )
	{
		lsb_12bit &= ~(1U<<13);//1101 1111 1111 1111 �Ȱѵ�13λ��0������λ����
		lsb_12bit |= 1U<<14;//0100 0000 0000 0000 �ѵ�14λ��1������λ����
		lsb_12bit |= 1U<<15;//1000 0000 0000 0000 �ѵ�15λ��1������λ����
		msb_12bit &= ~(1U<<13); //ͬ��
		msb_12bit |= 1U<<14;//
		msb_12bit |= 1U<<15;//
		phase_number_2 = 0;
	}
	else if(phase_number_2 == 1)
	{
		lsb_12bit |= 1U<<13; //1011 1111 1111 1111 �Ȱѵ�13λ��1������λ����
		lsb_12bit |= 1U<<14; //1000 0000 0000 0000 �ٰѵ�14λ��1������λ����
		lsb_12bit |= 1U<<15; //1000 0000 0000 0000 �ѵ�15λ��1������λ����
		msb_12bit |= 1U<<13; //ͬ��
		msb_12bit |= 1U<<14; //
		msb_12bit |= 1U<<15; //
		phase_number_2++;
	}
	AD9833_2_Write_16Bits(lsb_12bit);
	AD9833_2_Write_16Bits(msb_12bit);
}

void ResetAD9833_2()
{
	AD9833_2_Write_16Bits(0x0100);
	freq_number_2=0;
	phase_number_2=0;
}
