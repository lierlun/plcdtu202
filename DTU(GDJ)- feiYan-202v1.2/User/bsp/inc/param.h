/*
*********************************************************************************************************
*
*	模块名称 : 应用程序参数模块
*	文件名称 : param.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __PARAM_H
#define __PARAM_H
#include "data.h"

/* 下面2行宏只能选择其一 */
//#define PARAM_SAVE_TO_W25Q16			      /* 参数存储到外部的FLASH */
#define PARAM_SAVE_TO_FLASH		        /* 参数存储到CPU内部Flash */
//#define PARAM_SAVE_TO_EEPROM            /* 参数存储到EEPROM */

#ifdef PARAM_SAVE_TO_W25Q16
	#define PARAM_ADDR		     1024			  /* 参数区地址 */
	#define BOOT_INFO_ADDR     1024*8     /* BOOT信息存储地址 */
	#define BOOT_DATA_ADDR     1024*12    /* BOOT数据存储地址 */
#endif

#ifdef PARAM_SAVE_TO_FLASH
	#define PARAM_ADDR		ADDR_FLASH_SECTOR_4			    //兼容512K与1M的flash
//	#define PARAM_ADDR	 ADDR_FLASH_SECTOR_11		    /* 0x080E0000 Flash最后128K扇区用来存放参数 */
#endif

#ifdef PARAM_SAVE_TO_EEPROM
	#define PARAM_ADDR         1
#endif 

#define SYS_PARAM_VER			  0x00000013					        /* 参数版本 */
#define TOUCH_PARAM_VER			0x00000006					        /* 参数版本 */

/* 全局参数 */
typedef struct
{
	uint32_t ParamVer;			/* 参数区版本控制（可用于程序升级时，决定是否对参数区进行升级） */

	GPRS_PARA       GPRS_Para;
}
PARAM_T;

extern PARAM_T g_tParam;

extern void LoadParam(uint8_t flag);
extern void SaveParam(void);

#endif
