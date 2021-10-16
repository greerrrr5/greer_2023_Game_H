
#include "usart.h"
#include "adc/bsp_adc.h"

volatile uint16_t ADC_ConvertedValue[ADC_DataSize];

void TIM2_IRQHandler(void)  
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update))  
    {         
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void DMA2_Stream0_IRQHandler(void)  
{
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))  
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
    }
}

/*************************************************************************************************
* Function         : DMA_Config
* Description      : 初始化DMA通道
* parameters[in]   : DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
* parameters[out]  : None
* Returns          : None
* Note(s)          : 
*************************************************************************************************/
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx)
{ 
    DMA_InitTypeDef  DMA_InitStructure;

    if((u32)DMA_Streamx>(u32)DMA2)																					//得到当前stream是属于DMA2还是DMA1
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);										//DMA2时钟使能 
        
    }else 
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);										//DMA1时钟使能 
    }
    DMA_DeInit(DMA_Streamx);
	
    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}											//等待DMA可配置 

    /* 配置 DMA Stream */
    DMA_InitStructure.DMA_Channel            = DMA_Channel_2; 									//通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&ADC3->DR);							//DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)&ADC_ConvertedValue;					//DMA存储器0地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;	                    //外设到内存
    DMA_InitStructure.DMA_BufferSize         = ADC_DataSize;									//数据传输量 
 
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;		    //外设非增量模式
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;				//存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;     //外设数据长度:16位
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;			//存储器数据长度:16位
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;					//使用循环模式 
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;					//优先级
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;			//存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;	    //外设突发单次传输
    DMA_Init(DMA_Streamx, &DMA_InitStructure);									//初始化DMA Stream
	                     								  
    /* DMA2_Stream0 enable */
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA_Streamx, ENABLE);     
} 

/*********************************************
 * @description		: ADC引脚始化 
 * @param  	        : 无
 * @return 			: 无
 *********************************************/
void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
}

/*****************************************
 * @description		: ADC配置
 * @param  	        : 无
 * @return 			: 无
 ****************************************/
void ADC_Config(void)
{
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);      
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);    

    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; 
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; 
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonInit(&ADC_CommonInitStructure);
    
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; 
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;  
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;    
    ADC_Init(ADC3, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC3, ADC_Channel_0, 1, ADC_SampleTime_15Cycles);  

    ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
    ADC_DMACmd(ADC3, ENABLE);  
    ADC_Cmd(ADC3, ENABLE);
}

/***************************************************
 * @description		: ADC、DMA中断配置 
 * @param  	        : 无
 * @return 			: 无
 **************************************************/
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  	
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            
    NVIC_Init(&NVIC_InitStructure);    

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           
    NVIC_Init(&NVIC_InitStructure);   
}

/************************************************************
 * @description		: 通用定时器2中断初始化 
 * @param  	        : 无
 * @return 			: 无
 ************************************************************/
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);          
    TIM_Cmd(TIM2, DISABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = 14 - 1;                  //自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 6 - 1;                //定时器分频
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频器
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

    TIM_ARRPreloadConfig(TIM2, ENABLE); 
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);          

    TIM_Cmd(TIM2, ENABLE); 
}

/******************************************
 * @description		: ADC初始化 
 * @param  	        : 无
 * @return 			: 无
 ******************************************/
void ADC_Mode_Init(void)
{
    ADC_GPIO_Init();            //ADC引脚初始化
	ADC_Config();               //ADC配置
	DMA_Config(DMA2_Stream0);   //DMA配置
	NVIC_Config();              //TIM2、DMA中断配置
	TIM2_Init();                //定时器2初始化
}

/************************************************
 * @description		  : ADC采样速度设置
 * @param - SampleRate: 采样速度
 * @return 			  : 无
 ************************************************/
void SetADCSampleRate(uint32_t SampleRate)
{
	TIM_PrescalerConfig(TIM3, 6000000 / SampleRate - 1, TIM_PSCReloadMode_Immediate);
}

