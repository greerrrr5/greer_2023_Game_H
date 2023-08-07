#ifndef __AD9833_H
#define __AD9833_H

#include "sys.h"
#include "stdint.h"
#include "stm32f4xx_rcc.h"

#define AD9833_SYSTEM_CLOCK 25000000//AD9833内置25MHz时钟
#define pi 3.1415926535

/***********AD9833_1***********/
/* GPIO */
#define RCC_SCLK_1 	RCC_AHB1Periph_GPIOF
#define PORT_SCLK_1	GPIOF
#define PIN_SCLK_1	GPIO_Pin_0

#define RCC_SDATA_1 	RCC_AHB1Periph_GPIOF
#define PORT_SDATA_1	GPIOF
#define PIN_SDATA_1	GPIO_Pin_1

/* 片选 */
#define RCC_FSYNC_1 	RCC_AHB1Periph_GPIOF
#define PORT_FSYNC_1	GPIOF
#define PIN_FSYNC_1	GPIO_Pin_2
	
/* 定义口线置0和置1的宏 */
#define FSYNC1_0()	GPIO_ResetBits(PORT_FSYNC_1,PIN_FSYNC_1)
#define FSYNC1_1()	GPIO_SetBits(PORT_FSYNC_1,PIN_FSYNC_1)

#define SCLK1_0()	GPIO_ResetBits(PORT_SCLK_1,PIN_SCLK_1)
#define SCLK1_1()	GPIO_SetBits(PORT_SCLK_1,PIN_SCLK_1)

#define SDATA1_0()	GPIO_ResetBits(PORT_SDATA_1,PIN_SDATA_1)
#define SDATA1_1()	GPIO_SetBits(PORT_SDATA_1,PIN_SDATA_1)


/***********AD9833_2***********/
/* GPIO */
#define RCC_SCLK_2 	RCC_AHB1Periph_GPIOF
#define PORT_SCLK_2	GPIOF
#define PIN_SCLK_2	GPIO_Pin_4

#define RCC_SDATA_2 	RCC_AHB1Periph_GPIOF
#define PORT_SDATA_2	GPIOF
#define PIN_SDATA_2	GPIO_Pin_5

/* 片选 */
#define RCC_FSYNC_2 	RCC_AHB1Periph_GPIOF
#define PORT_FSYNC_2	GPIOF
#define PIN_FSYNC_2	GPIO_Pin_6

	
/* 定义口线置0和置1的宏 */
#define FSYNC2_0()	GPIO_ResetBits(PORT_FSYNC_2,PIN_FSYNC_2)
#define FSYNC2_1()	GPIO_SetBits(PORT_FSYNC_2,PIN_FSYNC_2)

#define SCLK2_0()	GPIO_ResetBits(PORT_SCLK_2,PIN_SCLK_2)
#define SCLK2_1()	GPIO_SetBits(PORT_SCLK_2,PIN_SCLK_2)

#define SDATA2_0()	GPIO_ResetBits(PORT_SDATA_2,PIN_SDATA_2)
#define SDATA2_1()	GPIO_SetBits(PORT_SDATA_2,PIN_SDATA_2)

void Init_AD9833(void);
void AD9833_1_Write_16Bits(uint16_t _cmd);
void AD9833_1_SelectWave(uint8_t freqRegister, uint8_t phaseRegister, uint8_t _Type);
void AD9833_1_SetFreq(uint32_t _freq);
void AD9833_1_SetPHASE(uint32_t _phase);
void ResetAD9833_1(void);

void AD9833_2_Write_16Bits(uint16_t _cmd);
void AD9833_2_SelectWave(uint8_t freqRegister, uint8_t phaseRegister, uint8_t _Type);
void AD9833_2_SetFreq(uint32_t _freq);
void AD9833_2_SetPHASE(uint32_t _phase);
void ResetAD9833_2(void);

#endif /* __AD9833_H */
