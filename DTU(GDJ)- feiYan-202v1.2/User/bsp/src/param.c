/*
*********************************************************************************************************
*
*	ģ������ : Ӧ�ó������ģ��
*	�ļ����� : param.c
*	��    �� : V1.0
*	˵    �� : ��ȡ�ͱ���Ӧ�ó���Ĳ���
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-01-01 armfly  ��ʽ����
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"

PARAM_T g_tParam;

/* ��16KB һ�������Ŀռ�Ԥ��������Ϊ������ For MDK */
//const uint8_t para_flash_area[16*1024] __attribute__((at(ADDR_FLASH_SECTOR_3)));

/*
*********************************************************************************************************
*	�� �� ��: LoadParam
*	����˵��: ��Flash��������g_tParam
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LoadParam(uint8_t flag)
{
#ifdef PARAM_SAVE_TO_FLASH
	/* ��ȡCPU Flash�еĲ��� */
	bsp_ReadCpuFlash(PARAM_ADDR, (uint8_t *)&g_tParam, sizeof(PARAM_T));
#endif

#ifdef PARAM_SAVE_TO_EEPROM
	/* ��ȡEEPROM�еĲ��� */
	ee_ReadBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
#endif

#ifdef PARAM_SAVE_TO_W25Q16
	/* ��ȡ�ⲿFLASH�еĲ��� */
	sf_ReadBuffer((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
#endif
	
	/* ���ȱʡ���� */
	if ((g_tParam.ParamVer != SYS_PARAM_VER)||(flag))
	{
		g_tParam.ParamVer = SYS_PARAM_VER;

/***************MODBUS��ز���***********************/	
		//Ӳ���汾
		GPRS_Para.hard_ver         = 1;
		//����汾
		GPRS_Para.soft_ver         = 1;
		//��������60��
		GPRS_Para.heartbeat_period = 20;	
		
		SaveParam();							/* ���²���д��Flash */
	}
	else
	{
/***************MODBUS��ز���***********************/	
		//Ӳ���汾
		GPRS_Para.hard_ver           = g_tParam.GPRS_Para.hard_ver;
		//����汾
		GPRS_Para.soft_ver           = g_tParam.GPRS_Para.soft_ver;
		GPRS_Para.heartbeat_period   = g_tParam.GPRS_Para.heartbeat_period;
		
  }
}

/*
*********************************************************************************************************
*	�� �� ��: SaveParam
*	����˵��: ��ȫ�ֱ���g_tParam д�뵽CPU�ڲ�Flash
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SaveParam(void)
{
/***************MODBUS��ز���***********************/	
	//Ӳ���汾
	g_tParam.GPRS_Para.hard_ver 				  = GPRS_Para.hard_ver;
	//����汾
	g_tParam.GPRS_Para.soft_ver 				  = GPRS_Para.soft_ver;
	g_tParam.GPRS_Para.heartbeat_period   = GPRS_Para.heartbeat_period;
		
#ifdef PARAM_SAVE_TO_FLASH
	/* ��ȫ�ֵĲ����������浽 CPU Flash */
	bsp_WriteCpuFlash(PARAM_ADDR, (unsigned char *)&g_tParam, sizeof(PARAM_T));
#endif

#ifdef PARAM_SAVE_TO_EEPROM
	/* ��ȫ�ֵĲ����������浽EEPROM */
	ee_WriteBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
#endif

#ifdef PARAM_SAVE_TO_W25Q16
	/* ��ȫ�ֵĲ����������浽�ⲿFLASH */
	sf_WriteBuffer((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
#endif
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
