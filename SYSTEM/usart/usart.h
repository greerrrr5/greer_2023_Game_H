
#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include  <math.h>
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "sys.h" 
#include "stdio.h"	
#include "stm32f4xx_conf.h"

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void  App_Printf(char *format, ...);
    
#ifdef __cplusplus
}
#endif

#endif


