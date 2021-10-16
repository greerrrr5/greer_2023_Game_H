
#include "usart.h"
#include "led/bsp_led.h"
#include "TimBase/bsp_TiMbase.h"

/************************************************************
 * @description		: ͨ�ö�ʱ��5�жϳ�ʼ�� 
 * @param - arr 	: �Զ���װֵ
 * @param - psc 	: ʱ��Ԥ��Ƶ��
 * @return 			: ��ȡ���ֽ��������Ϊ��ֵ����ʾ��ȡʧ��
 * ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
 ************************************************************/
void TIM5_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);	//ʹ��TIM5ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 		//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  		//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	//��ʼ��TIM5
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); 			//����ʱ��5�����ж�
	TIM_Cmd(TIM5,ENABLE); 								//ʹ�ܶ�ʱ��5
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; 		//��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x05; 	//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
