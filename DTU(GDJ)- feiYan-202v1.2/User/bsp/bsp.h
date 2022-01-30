/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可，
*			  不需要#include 每个模块的 h 文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H


/* 使能在源文件中使用uCOS-III的函数, 这里的源文件主要是指BSP驱动文件 */
#define uCOS_EN       1

#if uCOS_EN == 1    
	#include "os.h"   

	#define  ENABLE_INT()      OS_CRITICAL_EXIT()     /* 使能全局中断 */
	#define  DISABLE_INT()     OS_CRITICAL_ENTER()    /* 禁止全局中断 */
#else
	/* 开关全局中断的宏 */
	#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */
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

//自定义库
#include "my_stdlib.h"

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

//位左移
#define BIT(x)                                   ((uint32_t)(1<<x))
//置位
#define BIT_SET(val, mask)                       { (val) |=  (mask); }
//复位
#define BIT_CLR(val, mask)                       { (val) &= ~(mask); }
//判断位是否置位
#define BIT_IS_SET(val, mask)                    ((((val) & (mask)) == (mask)) ? (TRUE) : (FALSE ))
//判断位是否复位
#define BIT_IS_CLR(val, mask)                    (((val) & (mask))            ? (TRUE ) : (FALSE))
//获得最小值
#define MIN(a, b)                                (((a) < (b)) ? (a) : (b))
//获得最大值
#define MAX(a, b)                                (((a) > (b)) ? (a) : (b))



/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
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

/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void BSP_Tick_Init (void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
