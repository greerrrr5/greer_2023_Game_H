
#include  <math.h>
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "sys.h"
#include "dso.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "arm_math.h"
#include "led/bsp_led.h"
#include "dac/bsp_dac.h"
#include "adc/bsp_adc.h"
#include "log/bsp_log.h"
#include "TimBase/bsp_TiMbase.h"

#define START_TASK_PRIO		1
#define START_STK_SIZE 		128  
#define LED_TASK_PRIO		2
#define LED_STK_SIZE 		256 
#define CPU_TASK_PRIO		3	
#define CPU_STK_SIZE 		256  

TaskHandle_t StartTask_Handler;
TaskHandle_t LEDTask_Handler;   //LED TASK句柄
TaskHandle_t CPUTask_Handler;   //CPUT ASK句柄
TaskHandle_t vTaskMsgPro;       //串口TASK句柄

SemaphoreHandle_t msgProMutex = NULL;   //USART互斥信号量

void xMutexCreate(void);
void start_task(void *pvParameters);
void led_task(void *pvParameters);
void CPU_Task(void *pvParameters);

int main(void)
{ 
    BaseType_t xReturn;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     //设置系统中断优先级分组4
	delay_init(168);		                            //初始化延时函数
	uart_init(115200);     	                            //初始化串口
	LED_Init();		                                    //初始化LED端口
    DAC_Mode_Init();                                    //DAC初始化
    SetDACFreq(56000);
    ADC_Mode_Init();                                    //ADC初始化
    SetADCSampleRate(_tgrade[0].SPS);
    TIM5_Init(1000-1, 84-1);                            //TIM5初始化
    arm_cfft_radix4_init_f32(&scfft, SAMPLE_DEPTH, 0, 1); //初始化FFT
    
    xReturn = xTaskCreate((TaskFunction_t )start_task,  //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄                  

    xMutexCreate();         //初始化互斥信号量   
    vTraceEnable(TRC_INIT);                
    if(pdPASS == xReturn)
        vTaskStartScheduler(); 
    else
        return -1;

    while(1);           
}
 
/******************************************************
 * @description		: 开始任务任务函数
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    xTaskCreate((TaskFunction_t )led_task,     	
                (const char*    )"led_task",   	
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED_TASK_PRIO,	
                (TaskHandle_t*  )&LEDTask_Handler);   

    xTaskCreate((TaskFunction_t )CPU_Task,     
                (const char*    )"CPU_Task",   
                (uint16_t       )CPU_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CPU_TASK_PRIO,
                (TaskHandle_t*  )&CPUTask_Handler);        
     
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

/******************************************************
 * @description		: LED任务任务函数
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
void led_task(void *pvParameters)
{ 
    while(1)
    {
        LED0=~LED0;
        CopyDataToWaveBuff();
        FFT_GetFreq(_tgrade[0].SPS);
        vTaskDelay(250);
    }
}   


void CPU_Task(void *parameter)
{
    uint8_t CPU_RunInfo[400]; 
    
    while (1)
    {
        memset(CPU_RunInfo, 0, 400); 

        vTaskList((char *)&CPU_RunInfo); 

        LOG_INFO("---------------------------------------------\r\n");
        LOG_INFO("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
        LOG_INFO("%s", CPU_RunInfo);
        LOG_INFO("---------------------------------------------\r\n");

        memset(CPU_RunInfo, 0, 400); 
        vTaskGetRunTimeStats((char *)&CPU_RunInfo);

        LOG_INFO("任务名       运行计数         利用率\r\n");
        LOG_INFO("%s", CPU_RunInfo);
        LOG_INFO("---------------------------------------------\r\n\n");
        vTaskDelay(20000); 
    }
}

/******************************************************
 * @description		: 创建任务通信机制
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
void xMutexCreate(void)
{
    msgProMutex = xSemaphoreCreateMutex();
    
    if(msgProMutex == NULL)
    {
    }
}
