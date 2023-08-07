
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
TaskHandle_t LEDTask_Handler;   //LED TASK句柄
TaskHandle_t CPUTask_Handler;   //CPU TASK句柄
TaskHandle_t LCDTask_Handler;   //LCD TASK句柄
TaskHandle_t KEYTask_Handler;   //KEY TASK句柄
TaskHandle_t AD9833Task_Handler;   //KEY TASK句柄
//TaskHandle_t GUITask_Handler;   //GUI TASK句柄
//TaskHandle_t TPTask_Handler;    //TP  TASK句柄
TaskHandle_t vTaskMsgPro;       //串口TASK句柄


SemaphoreHandle_t msgProMutex = NULL;   //USART互斥信号量

extern int wave1Sin_Times;//wave1识别为三角波的次数
extern int wave2Sin_Times;//wave2识别为三角波的次数
extern int wave1Freq;//wave1频率
extern int wave2Freq;//同上

//完成10次fft后重置
int Final_wave1Type=4;//最终wave1类型，默认为NULL
int Final_wave2Type=4;//同上
int Final_wave1Freq;//最终wave1频率
int Final_wave2Freq;//同上
int wave1Sin_Times=0;
int wave2Sin_Times=0;
int phase=0;//按键设置DDS相位
int sure_sta=0;//确认输出按键状态，默认为未确认
int times=10;//fft是否开始，0为开始标志
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
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     //设置系统中断优先级分组4
	  delay_init(168);		                            //初始化延时函数
	  uart_init(115200);     	                            //初始化串口
	  LED_Init();		                                    //初始化LED端口
		KEY_Init();							                          //初始化按键
		LCD_Init();						                          	//初始化LCD
		
    DAC_Mode_Init();                                    //DAC初始化
    ADC_Mode_Init();                                    //ADC初始化
    //TFTLCD_Init();			                            //LCD初始化
    //TP_Init();                                          //触摸初始化
    FSMC_SRAM_Init();                                   //SRAM初始化
    TIM5_Init(1000-1, 84-1);                            //TIM5初始化,1ms
    arm_cfft_radix4_init_f32(&scfft, SAMPLE_DEPTH, 0, 1); //初始化FFT
   // lv_init();	                                          //lvgl系统初始化
	//lv_port_disp_init();	                              //lvgl显示接口初始化,放在lv_init()的后面
	//lv_port_indev_init();	                              //lvgl输入接口初始化,放在lv_init()的后面
    
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
 * @description		: 开始任务函数
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
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
     
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

/******************************************************
 * @description		: LED任务函数
 * @param   	    : 无
 * @return 			: 无
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
						
						printf("wave1:10次%d次正弦波",wave1Sin_Times);
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
						printf("wave2:10次%d次正弦波",wave2Sin_Times);
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
 * @description		: LCD任务函数
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
void LCD_Task(void *pvParameters)
{ 
	  while(1)
		{
			POINT_COLOR = RED;
			//void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
			LCD_ShowString(30,10,200,16,16,"Please enter the initial");	
			LCD_ShowString(55,30,200,16,16,"phase difference!");
			
			LCD_ShowxNum(100,70,phase,3,24,0);         //显示并不断更新信号量值
			
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
 * @description		: KEY任务函数
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
void KEY_Task(void *pvParameters)
{ 
	  u8 key;
	  
    while(1)//一直扫描是否确认
	  {
				if(sure_sta==0)//在未确认状态下扫描
				{
					key=KEY_Scan(0);            //按键扫描
					switch(key)
					{					 
						case KEY2_PRES:			//KEY2按键(-5)
							if(phase>=5)
							{
								phase=phase-5;
								//LCD_ShowxNum(100,70,phase,3,24,0);         //显示信号量值
							}
								break;
						case KEY0_PRES:			//KEY0按键(+5)
							if(phase<=175)
							{
								phase=phase+5;
								//LCD_ShowxNum(100,70,phase,3,24,0);         //显示信号量值
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
 * @description		: AD9833任务函数
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
void AD9833_Task(void *pvParameters)
{ 
    while(1)//不断刷新查看是否10次fft转换完毕
    {
			if(start_output==1)
			{
				Init_AD9833();                                    //初始化AD9833
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
 * @description		: TP任务函数
 * @param   	    : 无
 * @return 			: 无
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
 * @description		: GUI任务函数
 * @param   	    : 无
 * @return 			: 无
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
 * @description		: GUI任务函数
 * @param   	    : 无
 * @return 			: 无
 ******************************************************/
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
