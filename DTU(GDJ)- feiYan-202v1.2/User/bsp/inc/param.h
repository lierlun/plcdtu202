/*
*********************************************************************************************************
*
*	ģ������ : Ӧ�ó������ģ��
*	�ļ����� : param.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __PARAM_H
#define __PARAM_H
#include "data.h"

/* ����2�к�ֻ��ѡ����һ */
//#define PARAM_SAVE_TO_W25Q16			      /* �����洢���ⲿ��FLASH */
#define PARAM_SAVE_TO_FLASH		        /* �����洢��CPU�ڲ�Flash */
//#define PARAM_SAVE_TO_EEPROM            /* �����洢��EEPROM */

#ifdef PARAM_SAVE_TO_W25Q16
	#define PARAM_ADDR		     1024			  /* ��������ַ */
	#define BOOT_INFO_ADDR     1024*8     /* BOOT��Ϣ�洢��ַ */
	#define BOOT_DATA_ADDR     1024*12    /* BOOT���ݴ洢��ַ */
#endif

#ifdef PARAM_SAVE_TO_FLASH
	#define PARAM_ADDR		ADDR_FLASH_SECTOR_4			    //����512K��1M��flash
//	#define PARAM_ADDR	 ADDR_FLASH_SECTOR_11		    /* 0x080E0000 Flash���128K����������Ų��� */
#endif

#ifdef PARAM_SAVE_TO_EEPROM
	#define PARAM_ADDR         1
#endif 

#define SYS_PARAM_VER			  0x00000013					        /* �����汾 */
#define TOUCH_PARAM_VER			0x00000006					        /* �����汾 */

/* ȫ�ֲ��� */
typedef struct
{
	uint32_t ParamVer;			/* �������汾���ƣ������ڳ�������ʱ�������Ƿ�Բ��������������� */

	GPRS_PARA       GPRS_Para;
}
PARAM_T;

extern PARAM_T g_tParam;

extern void LoadParam(uint8_t flag);
extern void SaveParam(void);

#endif
