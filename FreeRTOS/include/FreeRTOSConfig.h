/*
 * FreeRTOS V202107.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "sys.h"
#include "usart.h"

/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif

#define configUSE_PREEMPTION			        1                               //1ʹ����ռʽ�ںˣ�0ʹ��Э��
#define configUSE_TIME_SLICING					1						        //1ʹ��ʱ��Ƭ����(Ĭ��ʽʹ�ܵ�)
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1                               //1�������ⷽ����ѡ����һ��Ҫ���е�����
                                                                                //һ����Ӳ������ǰ����ָ������ʹ�õ�
                                                                                //MCUû����ЩӲ��ָ��Ļ��˺�Ӧ������Ϊ0��
#define configUSE_TICKLESS_IDLE					0                               //1���õ͹���ticklessģʽ   
#define configUSE_QUEUE_SETS					1                               //Ϊ1ʱ���ö���
#define configCPU_CLOCK_HZ				        ( SystemCoreClock )             //CPUƵ��
#define configTICK_RATE_HZ				        ( ( TickType_t ) 1000 )         //ʱ�ӽ���Ƶ�ʣ���������Ϊ1000�����ھ���1ms
#define configMAX_PRIORITIES			        ( 32 )                          //��ʹ�õ�������ȼ�
#define configMINIMAL_STACK_SIZE		        ( ( unsigned short ) 130 )      //��������ʹ�õĶ�ջ��С
#define configMAX_TASK_NAME_LEN			        ( 16 )                          //���������ַ�������
#define configUSE_16_BIT_TICKS			        0                               //ϵͳ���ļ����������������ͣ�
                                                                                //1��ʾΪ16λ�޷������Σ�0��ʾΪ32λ�޷�������
#define configIDLE_SHOULD_YIELD			        1                               //Ϊ1ʱ�����������CPUʹ��Ȩ������ͬ���ȼ����û�����
#define configUSE_TASK_NOTIFICATIONS            1                               //Ϊ1ʱ��������֪ͨ���ܣ�Ĭ�Ͽ���
#define configUSE_MUTEXES				        1                               //Ϊ1ʱʹ�û����ź���
#define configQUEUE_REGISTRY_SIZE		        8                               //��Ϊ0ʱ��ʾ���ö��м�¼�������ֵ�ǿ���
                                                                                //��¼�Ķ��к��ź��������Ŀ��
#define configCHECK_FOR_STACK_OVERFLOW	        0                               //����0ʱ���ö�ջ�����⹦�ܣ����ʹ�ô˹���
                                                                                //�û������ṩһ��ջ������Ӻ��������ʹ�õĻ�
                                                                                //��ֵ����Ϊ1����2����Ϊ������ջ�����ⷽ����
#define configUSE_RECURSIVE_MUTEXES		        1                               //Ϊ1ʱʹ�õݹ黥���ź���
#define configUSE_MALLOC_FAILED_HOOK	        0                               //1ʹ���ڴ�����ʧ�ܹ��Ӻ���
#define configUSE_APPLICATION_TASK_TAG	        0
#define configUSE_COUNTING_SEMAPHORES	        1                               //Ϊ1ʱʹ�ü����ź���


#define configSUPPORT_DYNAMIC_ALLOCATION        1                               //֧�ֶ�̬�ڴ�����
#define configTOTAL_HEAP_SIZE			        ( ( size_t ) ( 30 * 1024 ) )    //ϵͳ�����ܵĶѴ�С

#define configUSE_IDLE_HOOK				        0                               //1��ʹ�ÿ��й��ӣ�0����ʹ��
#define configUSE_TICK_HOOK				        0                               //1��ʹ��ʱ��Ƭ���ӣ�0����ʹ��
        
#define configGENERATE_RUN_TIME_STATS	        1                               //Ϊ1ʱ��������ʱ��ͳ�ƹ���
#define configUSE_TRACE_FACILITY		        1                               //Ϊ1���ÿ��ӻ����ٵ���
#define configUSE_STATS_FORMATTING_FUNCTIONS	1                               //���configUSE_TRACE_FACILITYͬʱΪ1ʱ���������3������
                                                                                //prvWriteNameToBuffer(),vTaskList(),
                                                                                //vTaskGetRunTimeStats()
extern volatile uint32_t CPU_RunTime;

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()     (CPU_RunTime = 0ul)
#define portGET_RUN_TIME_COUNTER_VALUE()             CPU_RunTime 
                                                                     
                                                                                
/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0               //Ϊ1ʱ����Э�̣�����Э���Ժ��������ļ�croutine.c
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )       //Э�̵���Ч���ȼ���Ŀ

/* Software timer definitions. */
#define configUSE_TIMERS				1                               //Ϊ1ʱ���������ʱ��
#define configTIMER_TASK_PRIORITY		(configMAX_PRIORITIES - 1)      //�����ʱ�����ȼ�
#define configTIMER_QUEUE_LENGTH		5                               //�����ʱ�����г���
#define configTIMER_TASK_STACK_DEPTH	(configMINIMAL_STACK_SIZE * 2)  //�����ʱ�������ջ��С

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
	
/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
	
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* ǰ��ȫ��ʡ����... */

/* TraceԴ��ͷ�ļ���ǰ��˵�������µ�3.1.0�汾��TraceԴ�뱻ͳһ���򻯣�ʹ����ֻ��Ҫ����trcRecorder.h ��һ���ļ����ɣ�*/
/** @addtogroup Integrates the Tracealyzer recorder with FreeRTOS.
  * @{
  */
/* ����Ϊʹ��Percepio Tracealyzer��Ҫ�Ķ���������Ҫʱ�� configUSE_TRACE_FACILITY ����Ϊ 0 ,ʹ��ʱ���� configUSE_TRACE_FACILITY  ����Ϊ 1 ���� */
#if ( configUSE_TRACE_FACILITY == 1 )
#include "trcRecorder.h"
#define INCLUDE_xTaskGetCurrentTaskHandle				1	// ����һ����ѡ�������ú����� TraceԴ��ʹ�ã�Ĭ�ϸ�ֵΪ0 ��ʾ���ã������µ� 4.3.8��FreeRTOS 10.3.1 ������Ҫ��ֱ��ɾ������
#endif
/**
  * @}
  */


#endif /* FREERTOS_CONFIG_H */

