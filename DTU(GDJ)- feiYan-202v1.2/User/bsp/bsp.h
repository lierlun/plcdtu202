/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ���ɣ�
*			  ����Ҫ#include ÿ��ģ��� h �ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H


/* ʹ����Դ�ļ���ʹ��uCOS-III�ĺ���, �����Դ�ļ���Ҫ��ָBSP�����ļ� */
#define uCOS_EN       1

#if uCOS_EN == 1    
	#include "os.h"   

	#define  ENABLE_INT()      OS_CRITICAL_EXIT()     /* ʹ��ȫ���ж� */
	#define  DISABLE_INT()     OS_CRITICAL_ENTER()    /* ��ֹȫ���ж� */
#else
	/* ����ȫ���жϵĺ� */
	#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */
#endif

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    
typedef signed   char  INT8S;                    
typedef unsigned short INT16U;    
typedef signed   short INT16S;                   
typedef unsigned int   INT32U;                  
typedef signed   int   INT32S;                  
typedef float          FP32;                    
typedef double         FP64;   

typedef unsigned char  Uint8;
typedef unsigned short Uint16;
typedef unsigned int   Uint32; 

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//�Զ����
#include "my_stdlib.h"

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

//λ����
#define BIT(x)                                   ((uint32_t)(1<<x))
//��λ
#define BIT_SET(val, mask)                       { (val) |=  (mask); }
//��λ
#define BIT_CLR(val, mask)                       { (val) &= ~(mask); }
//�ж�λ�Ƿ���λ
#define BIT_IS_SET(val, mask)                    ((((val) & (mask)) == (mask)) ? (TRUE) : (FALSE ))
//�ж�λ�Ƿ�λ
#define BIT_IS_CLR(val, mask)                    (((val) & (mask))            ? (TRUE ) : (FALSE))
//�����Сֵ
#define MIN(a, b)                                (((a) < (b)) ? (a) : (b))
//������ֵ
#define MAX(a, b)                                (((a) > (b)) ? (a) : (b))



/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "bsp_uart.h"
#include "bsp_led.h"
#include "bsp_din.h"
#include "bsp_dout.h"
#include "bsp_timer.h"
#include "bsp_beep.h"
#include "bsp_iwdg.h"
#include "bsp_cpu_rtc.h"
#include "bsp_cpu_flash.h"
#include "bsp_sim800c_rtos.h"
#include "bsp_ds18b20.h"
#include "bsp_spi_flash.h"
#include "bsp_spi_bus.h"

/* �ṩ������C�ļ����õĺ��� */
void bsp_Init(void);
void BSP_Tick_Init (void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
