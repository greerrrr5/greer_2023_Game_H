
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
TaskHandle_t LEDTask_Handler;   //LED TASK���
TaskHandle_t CPUTask_Handler;   //CPUT ASK���
TaskHandle_t vTaskMsgPro;       //����TASK���

SemaphoreHandle_t msgProMutex = NULL;   //USART�����ź���

void xMutexCreate(void);
void start_task(void *pvParameters);
void led_task(void *pvParameters);
void CPU_Task(void *pvParameters);

int main(void)
{ 
    BaseType_t xReturn;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     //����ϵͳ�ж����ȼ�����4
	delay_init(168);		                            //��ʼ����ʱ����
	uart_init(115200);     	                            //��ʼ������
	LED_Init();		                                    //��ʼ��LED�˿�
    DAC_Mode_Init();                                    //DAC��ʼ��
    SetDACFreq(56000);
    ADC_Mode_Init();                                    //ADC��ʼ��
    SetADCSampleRate(_tgrade[0].SPS);
    TIM5_Init(1000-1, 84-1);                            //TIM5��ʼ��
    arm_cfft_radix4_init_f32(&scfft, SAMPLE_DEPTH, 0, 1); //��ʼ��FFT
    
    xReturn = xTaskCreate((TaskFunction_t )start_task,  //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������                  

    xMutexCreate();         //��ʼ�������ź���   
    vTraceEnable(TRC_INIT);                
    if(pdPASS == xReturn)
        vTaskStartScheduler(); 
    else
        return -1;

    while(1);           
}
 
/******************************************************
 * @description		: ��ʼ����������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
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
     
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

/******************************************************
 * @description		: LED����������
 * @param   	    : ��
 * @return 			: ��
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
        LOG_INFO("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
        LOG_INFO("%s", CPU_RunInfo);
        LOG_INFO("---------------------------------------------\r\n");

        memset(CPU_RunInfo, 0, 400); 
        vTaskGetRunTimeStats((char *)&CPU_RunInfo);

        LOG_INFO("������       ���м���         ������\r\n");
        LOG_INFO("%s", CPU_RunInfo);
        LOG_INFO("---------------------------------------------\r\n\n");
        vTaskDelay(20000); 
    }
}

/******************************************************
 * @description		: ��������ͨ�Ż���
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
void xMutexCreate(void)
{
    msgProMutex = xSemaphoreCreateMutex();
    
    if(msgProMutex == NULL)
    {
    }
}
