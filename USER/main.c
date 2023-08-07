
#include  <math.h>
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "sys.h"
#include "delay.h"
#include "usart.h"

//#include "lvgl.h"
//#include "lv_port_disp.h"
//#include "lv_port_indev.h"
//#include "lv_apps\demo\demo.h" 
//#include "lv_examples/lv_dso/lv_dso.h"
//#include "lv_tests\lv_test_theme\lv_test_theme_1.h" 
//#include "lv_tests\lv_test_theme\lv_test_theme_2.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "arm_math.h"
#include "dso/bsp_dso.h"
#include "led/bsp_led.h"
#include "dac/bsp_dac.h"
#include "adc/bsp_adc.h"
#include "log/bsp_log.h"
#include "sram/bsp_sram.h"
#include "lcd.h"
#include "ad9833.h"
#include "key.h"
//#include "tftlcd/picture.h"
//#include "touch/bsp_touch.h"
//#include "tftlcd/bsp_tftlcd.h"
#include "TimBase/bsp_TiMbase.h"

#define START_TASK_PRIO		1
#define START_STK_SIZE 		128  
#define CPU_TASK_PRIO		2	
#define CPU_STK_SIZE 		256  
//#define GUI_TASK_PRIO		2	
//#define GUI_STK_SIZE 		512  
#define LED_TASK_PRIO		3
#define LED_STK_SIZE 		256 
#define LCD_TASK_PRIO		3
#define LCD_STK_SIZE 		256 
#define KEY_TASK_PRIO		3
#define KEY_STK_SIZE 		256 
#define AD9833_TASK_PRIO		3
#define AD9833_STK_SIZE 		256 
//#define TP_TASK_PRIO		3	
//#define TP_STK_SIZE 		256  

TaskHandle_t StartTask_Handler;
TaskHandle_t LEDTask_Handler;   //LED TASK���
TaskHandle_t CPUTask_Handler;   //CPU TASK���
TaskHandle_t LCDTask_Handler;   //LCD TASK���
TaskHandle_t KEYTask_Handler;   //KEY TASK���
TaskHandle_t AD9833Task_Handler;   //KEY TASK���
//TaskHandle_t GUITask_Handler;   //GUI TASK���
//TaskHandle_t TPTask_Handler;    //TP  TASK���
TaskHandle_t vTaskMsgPro;       //����TASK���


SemaphoreHandle_t msgProMutex = NULL;   //USART�����ź���

extern int wave1Sin_Times;//wave1ʶ��Ϊ���ǲ��Ĵ���
extern int wave2Sin_Times;//wave2ʶ��Ϊ���ǲ��Ĵ���
extern int wave1Freq;//wave1Ƶ��
extern int wave2Freq;//ͬ��

//���10��fft������
int Final_wave1Type=4;//����wave1���ͣ�Ĭ��ΪNULL
int Final_wave2Type=4;//ͬ��
int Final_wave1Freq;//����wave1Ƶ��
int Final_wave2Freq;//ͬ��
int wave1Sin_Times=0;
int wave2Sin_Times=0;
int phase=0;//��������DDS��λ
int sure_sta=0;//ȷ���������״̬��Ĭ��Ϊδȷ��
int times=10;//fft�Ƿ�ʼ��0Ϊ��ʼ��־
int start_output=0;

void xMutexCreate(void);
void start_task(void *pvParameters);
void LED_Task(void *pvParameters);
void CPU_Task(void *pvParameters);
void LCD_Task(void *pvParameters);
void KEY_Task(void *pvParameters);
void AD9833_Task(void *pvParameters);

//void GUI_Task(void *pvParameters);
//void TP_Task(void *pvParameters);

int main(void)
{ 
    BaseType_t xReturn;
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     //����ϵͳ�ж����ȼ�����4
	  delay_init(168);		                            //��ʼ����ʱ����
	  uart_init(115200);     	                            //��ʼ������
	  LED_Init();		                                    //��ʼ��LED�˿�
		KEY_Init();							                          //��ʼ������
		LCD_Init();						                          	//��ʼ��LCD
		
    DAC_Mode_Init();                                    //DAC��ʼ��
    ADC_Mode_Init();                                    //ADC��ʼ��
    //TFTLCD_Init();			                            //LCD��ʼ��
    //TP_Init();                                          //������ʼ��
    FSMC_SRAM_Init();                                   //SRAM��ʼ��
    TIM5_Init(1000-1, 84-1);                            //TIM5��ʼ��,1ms
    arm_cfft_radix4_init_f32(&scfft, SAMPLE_DEPTH, 0, 1); //��ʼ��FFT
   // lv_init();	                                          //lvglϵͳ��ʼ��
	//lv_port_disp_init();	                              //lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
	//lv_port_indev_init();	                              //lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
    
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
 * @description		: ��ʼ������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    xTaskCreate((TaskFunction_t )LED_Task,     	
                (const char*    )"LED_Task",   	
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
	  xTaskCreate((TaskFunction_t )LCD_Task,     
                (const char*    )"LCD_Task",   
                (uint16_t       )LCD_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LCD_TASK_PRIO,
                (TaskHandle_t*  )&LCDTask_Handler);
		xTaskCreate((TaskFunction_t )KEY_Task,     
                (const char*    )"KEY_Task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);
	  xTaskCreate((TaskFunction_t )AD9833_Task,     
                (const char*    )"AD9833_Task",   
                (uint16_t       )AD9833_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )AD9833_TASK_PRIO,
                (TaskHandle_t*  )&AD9833Task_Handler);
                
//    xTaskCreate((TaskFunction_t )GUI_Task,     
//                (const char*    )"GUI_Task",   
//                (uint16_t       )GUI_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )GUI_TASK_PRIO,
//                (TaskHandle_t*  )&GUITask_Handler);  

//    xTaskCreate((TaskFunction_t )TP_Task,     
//                (const char*    )"TP_Task",   
//                (uint16_t       )TP_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )TP_TASK_PRIO,
//                (TaskHandle_t*  )&TPTask_Handler);                  
     
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

/******************************************************
 * @description		: LED������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
void LED_Task(void *pvParameters)
{ 
    while(1)
    {
			if(sure_sta==1)
			{
				times=0;
				while(times!=10)
				{
					LED0=~LED0;
          CopyDataToWaveBuff();
					FFT(_tgrade[0].SPS,&wave1Sin_Times,&wave2Sin_Times);
					times++;
				  printf("times:%d\r\n",times);
					if(times==10)
					{
						Final_wave1Freq=wave1Freq;
						Final_wave2Freq=wave2Freq;
						if(wave1Sin_Times>7)
						{
							 Final_wave1Type=3;
						}
						else
						{
							 Final_wave1Type=2;
						}
						
						printf("wave1:10��%d�����Ҳ�",wave1Sin_Times);
						if(wave2Sin_Times>7)
						{
							 Final_wave2Type=3;
						}
						else
						{
							 Final_wave2Type=2;
						}
						
						if(wave1Freq==50 & wave2Freq==100)
						{
							Final_wave1Type=3;
							Final_wave2Type=3;
						}
						printf("wave2:10��%d�����Ҳ�",wave2Sin_Times);
						printf("FINAL:waveType1 %d\r\n",Final_wave1Type);
						printf("FINAL:waveType2 %d\r\n",Final_wave2Type);
						
						wave1Sin_Times=0;
						wave2Sin_Times=0;
						
					  LCD_ShowString(60,250,200,24,24,"Finish!");
						sure_sta=0;
						start_output=1;
					}
				}
			}
			      vTaskDelay(200);
		}
} 
/******************************************************
 * @description		: LCD������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
void LCD_Task(void *pvParameters)
{ 
	  while(1)
		{
			POINT_COLOR = RED;
			//void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
			LCD_ShowString(30,10,200,16,16,"Please enter the initial");	
			LCD_ShowString(55,30,200,16,16,"phase difference!");
			
			LCD_ShowxNum(100,70,phase,3,24,0);         //��ʾ�����ϸ����ź���ֵ
			
			POINT_COLOR = BLUE;
			LCD_ShowString(30,120,200,16,16,"key2:-5  key0:+5");

			POINT_COLOR = RED;
			LCD_ShowString(30,160,200,24,24,"PRESS THE KEY1");
			LCD_ShowString(60,190,200,24,24,"TO START");
			
			if(sure_sta==1)
			{
				LCD_ShowString(80,220,200,24,24,"START!");
				if(times<10) LCD_ShowString(60,250,200,24,24,"Waiting~");
			}
			else
			{
				LCD_ShowString(80,220,200,24,24,"      ");
			}
			
			vTaskDelay(200);
		}
}
/******************************************************
 * @description		: KEY������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
void KEY_Task(void *pvParameters)
{ 
	  u8 key;
	  
    while(1)//һֱɨ���Ƿ�ȷ��
	  {
				if(sure_sta==0)//��δȷ��״̬��ɨ��
				{
					key=KEY_Scan(0);            //����ɨ��
					switch(key)
					{					 
						case KEY2_PRES:			//KEY2����(-5)
							if(phase>=5)
							{
								phase=phase-5;
								//LCD_ShowxNum(100,70,phase,3,24,0);         //��ʾ�ź���ֵ
							}
								break;
						case KEY0_PRES:			//KEY0����(+5)
							if(phase<=175)
							{
								phase=phase+5;
								//LCD_ShowxNum(100,70,phase,3,24,0);         //��ʾ�ź���ֵ
							}
								break;
						case KEY1_PRES:
							sure_sta=1;
					}
			  }
				//vTaskDelay(200);
			}			
			//vTaskDelay(200);
}
/******************************************************
 * @description		: AD9833������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
void AD9833_Task(void *pvParameters)
{ 
    while(1)//����ˢ�²鿴�Ƿ�10��fftת�����
    {
			if(start_output==1)
			{
				Init_AD9833();                                    //��ʼ��AD9833
				//printf("hello!!\r\n");
			  AD9833_1_SetFreq(Final_wave1Freq*1000);
	      AD9833_1_SetPHASE(0);
		    AD9833_1_SelectWave(0,0,Final_wave1Type);
	
		    AD9833_2_SetFreq(Final_wave2Freq*1000);
	      AD9833_2_SetPHASE(phase);
		    AD9833_2_SelectWave(0,0,Final_wave2Type);
				start_output=0;
			}
			vTaskDelay(5);
    }
}
/******************************************************
 * @description		: TP������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
//void TP_Task(void *pvParameters)
//{ 
//    while(1)
//    {
//        tp_dev.scan(0);
//        vTaskDelay(5);
//    }
//}
/******************************************************
 * @description		: GUI������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
//void GUI_Task(void *pvParameters)
//{ 
////    demo_create();
////    lv_test_theme_1(lv_theme_night_init(210, NULL));
//    lv_dso_task();
//    while(1)
//    {
//        LED1=~LED1;
//		lv_task_handler();
//        vTaskDelay(20);
//    }
//}

/******************************************************
 * @description		: GUI������
 * @param   	    : ��
 * @return 			: ��
 ******************************************************/
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
