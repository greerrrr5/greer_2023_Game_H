
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
* Description      : ��ʼ��DMAͨ��
* parameters[in]   : DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
* parameters[out]  : None
* Returns          : None
* Note(s)          : 
*************************************************************************************************/
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx)
{ 
    DMA_InitTypeDef  DMA_InitStructure;

    if((u32)DMA_Streamx>(u32)DMA2)																					//�õ���ǰstream������DMA2����DMA1
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);										//DMA2ʱ��ʹ�� 
        
    }else 
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);										//DMA1ʱ��ʹ�� 
    }
    DMA_DeInit(DMA_Streamx);
	
    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}											//�ȴ�DMA������ 

    /* ���� DMA Stream */
    DMA_InitStructure.DMA_Channel            = DMA_Channel_2; 									//ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&ADC3->DR);							//DMA�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)&ADC_ConvertedValue;					//DMA�洢��0��ַ
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;	                    //���赽�ڴ�
    DMA_InitStructure.DMA_BufferSize         = ADC_DataSize;									//���ݴ����� 
 
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;		    //���������ģʽ
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;				//�洢������ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;     //�������ݳ���:16λ
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;			//�洢�����ݳ���:16λ
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;					//ʹ��ѭ��ģʽ 
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;					//���ȼ�
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;			//�洢��ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;	    //����ͻ�����δ���
    DMA_Init(DMA_Streamx, &DMA_InitStructure);									//��ʼ��DMA Stream
	                     								  
    /* DMA2_Stream0 enable */
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA_Streamx, ENABLE);     
} 

/*********************************************
 * @description		: ADC����ʼ�� 
 * @param  	        : ��
 * @return 			: ��
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
 * @description		: ADC����
 * @param  	        : ��
 * @return 			: ��
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
 * @description		: ADC��DMA�ж����� 
 * @param  	        : ��
 * @return 			: ��
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
 * @description		: ͨ�ö�ʱ��2�жϳ�ʼ�� 
 * @param  	        : ��
 * @return 			: ��
 ************************************************************/
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);          
    TIM_Cmd(TIM2, DISABLE);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
    TIM_TimeBaseStructure.TIM_Period = 14 - 1;                  //�Զ���װ��ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 6 - 1;                //��ʱ����Ƶ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ʱ�ӷ�Ƶ��
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ��� 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

    TIM_ARRPreloadConfig(TIM2, ENABLE); 
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);          

    TIM_Cmd(TIM2, ENABLE); 
}

/******************************************
 * @description		: ADC��ʼ�� 
 * @param  	        : ��
 * @return 			: ��
 ******************************************/
void ADC_Mode_Init(void)
{
    ADC_GPIO_Init();            //ADC���ų�ʼ��
	ADC_Config();               //ADC����
	DMA_Config(DMA2_Stream0);   //DMA����
	NVIC_Config();              //TIM2��DMA�ж�����
	TIM2_Init();                //��ʱ��2��ʼ��
}

/************************************************
 * @description		  : ADC�����ٶ�����
 * @param - SampleRate: �����ٶ�
 * @return 			  : ��
 ************************************************/
void SetADCSampleRate(uint32_t SampleRate)
{
	TIM_PrescalerConfig(TIM3, 6000000 / SampleRate - 1, TIM_PSCReloadMode_Immediate);
}

