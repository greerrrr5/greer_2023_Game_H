
#include "usart.h"
#include "led/bsp_led.h"
#include "TimBase/bsp_TiMbase.h"

/************************************************************
 * @description		: 通用定时器5中断初始化 
 * @param - arr 	: 自动重装值
 * @param - psc 	: 时钟预分频数
 * @return 			: 读取的字节数，如果为负值，表示读取失败
 * 定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
 ************************************************************/
void TIM5_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);	//使能TIM5时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 		//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  		//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	//初始化TIM5
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); 			//允许定时器5更新中断
	TIM_Cmd(TIM5,ENABLE); 								//使能定时器5
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; 		//定时器5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x05; 	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
