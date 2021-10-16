
#include "math.h"
#include "dac/bsp_dac.h"

/********���Ҳ������***********/
const uint16_t function[SINE_RES] = { 2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897, 
                                      2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640, 
                                      3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002, 
                                      4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894, 
                                      3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344, 
                                      3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483, 
                                      2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517, 
                                      1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679, 
                                      610, 545, 484, 426, 373, 323, 278, 237, 201, 169, 
                                      141, 118, 100, 87, 79, 75, 76, 82, 93, 109, 
                                      129, 154, 184, 218, 257, 300, 347, 399, 455, 514, 
                                      577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287, 
                                      1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047 };   

                                   

/*******************************
* @brief  TIM6��ʼ������
* @param  ��
* @retval ��
*******************************/          
void TIM6_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM6_TimeBase;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseStructInit(&TIM6_TimeBase); 
    TIM6_TimeBase.TIM_Period        = TIM_PERIOD - 1;          
    TIM6_TimeBase.TIM_Prescaler     = 0;       
    TIM6_TimeBase.TIM_ClockDivision = 0;    
    TIM6_TimeBase.TIM_CounterMode   = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM6, &TIM6_TimeBase);
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    TIM_Cmd(TIM6, ENABLE);
}

/*******************************
* @brief  DMA��ʼ������
* @param  ��
* @retval ��
*******************************/
void DAC1_Config(void)
{
    GPIO_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);                  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    DMA_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    DMA_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    DMA_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &DMA_InitStructure);
    
    DAC_InitTypeDef DAC_INIT;
    DMA_InitTypeDef DMA_INIT;

    DAC_INIT.DAC_Trigger        = DAC_Trigger_T6_TRGO;
    DAC_INIT.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_INIT.DAC_OutputBuffer   = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_INIT);

    DMA_DeInit(DMA1_Stream5);
    DMA_INIT.DMA_Channel            = DMA_Channel_7;  
    DMA_INIT.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12R1_ADDR;
    DMA_INIT.DMA_Memory0BaseAddr    = (uint32_t)&function;
    DMA_INIT.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_INIT.DMA_BufferSize         = SINE_RES;
    DMA_INIT.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_INIT.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_INIT.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_INIT.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_INIT.DMA_Mode               = DMA_Mode_Circular;
    DMA_INIT.DMA_Priority           = DMA_Priority_High;
    DMA_INIT.DMA_FIFOMode           = DMA_FIFOMode_Disable;         
    DMA_INIT.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
    DMA_INIT.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_INIT.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_INIT);

    DMA_Cmd(DMA1_Stream5, ENABLE);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_DMACmd(DAC_Channel_1, ENABLE);
}

/*******************************
* @brief  DAC��ʼ������
* @param  ��
* @retval ��
*******************************/
void DAC_Mode_Init(void)
{
    TIM6_Config();
    DAC1_Config(); 
}

/*******************************
* @brief  ����DACƵ��
* @param  ��
* @retval ��
*******************************/
uint32_t SetDACFreq(uint32_t Freq)
{
    TIM_SetAutoreload(TIM6, ((CNT_FREQ)/(((SINE_RES)*(Freq))))-1);
    return Freq;
}

