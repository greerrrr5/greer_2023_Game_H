#include "ad9833.h"

uint8_t freq_number_1=0;//设定AD9833_1的频率数量
uint8_t phase_number_1=0;//设定AD9833_1的相位数量

uint8_t freq_number_2=0;//设定AD9833_2的频率数量
uint8_t phase_number_2=0;//设定AD9833_2的相位数量

void Init_AD9833(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	FSYNC1_1();	/* FSYNC1 = 1 */
	FSYNC2_1();	/* FSYNC2 = 1 */

	/* 打开GPIO时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	/* 配置几个推推挽输出IO */
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* 设为输出、推挽模式口 */
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO口最大速度 */

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
	ResetAD9833_2();//复位
	AD9833_1_Write_16Bits(0x2100); 
	AD9833_2_Write_16Bits(0x2100); //选择数据一次写入//选择数据一次写入
}

/*
********************************************************
*	函 数 名: AD9833_1_Write_16Bits      
*	功能说明: 向SPI总线发送16个bit数据 发送控制字
*	形    参: _cmd : 无
*	返 回 值：无
*******************************************************
*/
void AD9833_1_Write_16Bits(uint16_t _cmd)
{
	uint8_t i;
	SCLK1_1(); //在时钟上升沿下操作
	FSYNC1_1();
	FSYNC1_0();	//片选打开
	/* AD9833  SCLK时钟25M,可以不延时 */
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
	FSYNC1_1();//片选关闭
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
				AD9833_1_Write_16Bits(square|(freqRegister<<12)|(phaseRegister<<11)); /*频率寄存器freqRegister相位寄存器phaseRegister输出方波*/
			  break;
			case 2:
				AD9833_1_Write_16Bits(triangle|(freqRegister<<12)|(phaseRegister<<11)); /*频率寄存器freqRegister相位寄存器phaseRegister输出三角波*/
			  break;
			case 3:
				AD9833_1_Write_16Bits(sine|(freqRegister<<12)|(phaseRegister<<11)); /*频率寄存器freqRegister相位寄存器phaseRegister输出正弦波*/
			  break;
			case 4:
				AD9833_1_Write_16Bits(null|(freqRegister<<12)|(phaseRegister<<11)); /*频率寄存器freqRegister相位寄存器phaseRegister无输出*/
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
		lsb_14bit &= ~(1U<<15);//0111 1111 1111 1111 先把第15位清0，其他位不变 结果就是
		lsb_14bit |= 1<<14;    //0100 0000 0000 0000 再把第14位置1，其他位不变 结果就是01xx xxxx xxxx xxxx
		msb_14bit &= ~(1U<<15); //同上
		msb_14bit |= 1<<14;
		freq_number_1 = 1;
	}
	else if(freq_number_1 == 1)
	{
		lsb_14bit &= ~(1U<<14); //1011 1111 1111 1111 先把第14位清0，其他位不变
		lsb_14bit |= 1U<<15;   //1000 0000 0000 0000 再把第15位置1，其他位不变 结果就是10xx xxxx xxxx xxxx
		msb_14bit &= ~(1<<14); //同上
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
		lsb_12bit &= ~(1U<<13);//1101 1111 1111 1111 先把第13位清0，其他位不变
		lsb_12bit |= 1U<<14;//0100 0000 0000 0000 把第14位置1，其他位不变
		lsb_12bit |= 1U<<15;//1000 0000 0000 0000 把第15位置1，其他位不变
		msb_12bit &= ~(1U<<13); //同上
		msb_12bit |= 1U<<14;//
		msb_12bit |= 1U<<15;//
		phase_number_1 = 1;
	}
	else if(phase_number_1 == 1)
	{
		lsb_12bit |= 1U<<13; //1011 1111 1111 1111 先把第13位置1，其他位不变
		lsb_12bit |= 1U<<14; //1000 0000 0000 0000 再把第14位置1，其他位不变
		lsb_12bit |= 1U<<15; //1000 0000 0000 0000 把第15位置1，其他位不变
		msb_12bit |= 1U<<13; //同上
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
*	函 数 名: AD9833_2_Write_16Bits      
*	功能说明: 向SPI总线发送16个bit数据 发送控制字
*	形    参: _cmd : 无
*	返 回 值：无
*******************************************************
*/
void AD9833_2_Write_16Bits(uint16_t _cmd)
{
	uint8_t i;
	SCLK2_1(); //在时钟上升沿下操作
	FSYNC2_1();
	FSYNC2_0();	//片选打开
	/* AD9833  SCLK时钟25M,可以不延时 */
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
	FSYNC2_1();//片选关闭
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
				AD9833_2_Write_16Bits(square|(freqRegister<<12)|(phaseRegister<<11)); /*频率寄存器freqRegister相位寄存器phaseRegister输出方波*/
			  break;
			case 2:
				AD9833_2_Write_16Bits(triangle|(freqRegister<<12)|(phaseRegister<<11)); /*频率寄存器freqRegister相位寄存器phaseRegister输出三角波*/
			  break;
			case 3:
				AD9833_2_Write_16Bits(sine|(freqRegister<<12)|(phaseRegister<<11)); /*频率寄存器freqRegister相位寄存器phaseRegister输出正弦波*/
			  break;
			case 4:
				AD9833_2_Write_16Bits(null|(freqRegister<<12)|(phaseRegister<<11)); /*频率寄存器freqRegister相位寄存器phaseRegister无输出*/
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
		lsb_14bit &= ~(1U<<15);//0111 1111 1111 1111 先把第15位清0，其他位不变 结果就是
		lsb_14bit |= 1<<14;    //0100 0000 0000 0000 再把第14位置1，其他位不变 结果就是01xx xxxx xxxx xxxx
		msb_14bit &= ~(1U<<15); //同上
		msb_14bit |= 1<<14;
		freq_number_2 = 0;
	}
	else if(freq_number_2 == 1)
	{
		lsb_14bit &= ~(1U<<14); //1011 1111 1111 1111 先把第14位清0，其他位不变
		lsb_14bit |= 1U<<15;   //1000 0000 0000 0000 再把第15位置1，其他位不变 结果就是10xx xxxx xxxx xxxx
		msb_14bit &= ~(1<<14); //同上
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
		lsb_12bit &= ~(1U<<13);//1101 1111 1111 1111 先把第13位清0，其他位不变
		lsb_12bit |= 1U<<14;//0100 0000 0000 0000 把第14位置1，其他位不变
		lsb_12bit |= 1U<<15;//1000 0000 0000 0000 把第15位置1，其他位不变
		msb_12bit &= ~(1U<<13); //同上
		msb_12bit |= 1U<<14;//
		msb_12bit |= 1U<<15;//
		phase_number_2 = 0;
	}
	else if(phase_number_2 == 1)
	{
		lsb_12bit |= 1U<<13; //1011 1111 1111 1111 先把第13位置1，其他位不变
		lsb_12bit |= 1U<<14; //1000 0000 0000 0000 再把第14位置1，其他位不变
		lsb_12bit |= 1U<<15; //1000 0000 0000 0000 把第15位置1，其他位不变
		msb_12bit |= 1U<<13; //同上
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
