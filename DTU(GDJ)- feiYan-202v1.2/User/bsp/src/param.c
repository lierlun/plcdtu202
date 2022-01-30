/*
*********************************************************************************************************
*
*	模块名称 : 应用程序参数模块
*	文件名称 : param.c
*	版    本 : V1.0
*	说    明 : 读取和保存应用程序的参数
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-01-01 armfly  正式发布
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"

PARAM_T g_tParam;

/* 将16KB 一个扇区的空间预留出来做为参数区 For MDK */
//const uint8_t para_flash_area[16*1024] __attribute__((at(ADDR_FLASH_SECTOR_3)));

/*
*********************************************************************************************************
*	函 数 名: LoadParam
*	功能说明: 从Flash读参数到g_tParam
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void LoadParam(uint8_t flag)
{
#ifdef PARAM_SAVE_TO_FLASH
	/* 读取CPU Flash中的参数 */
	bsp_ReadCpuFlash(PARAM_ADDR, (uint8_t *)&g_tParam, sizeof(PARAM_T));
#endif

#ifdef PARAM_SAVE_TO_EEPROM
	/* 读取EEPROM中的参数 */
	ee_ReadBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
#endif

#ifdef PARAM_SAVE_TO_W25Q16
	/* 读取外部FLASH中的参数 */
	sf_ReadBuffer((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
#endif
	
	/* 填充缺省参数 */
	if ((g_tParam.ParamVer != SYS_PARAM_VER)||(flag))
	{
		g_tParam.ParamVer = SYS_PARAM_VER;

/***************MODBUS相关参数***********************/	
		//硬件版本
		GPRS_Para.hard_ver         = 1;
		//软件版本
		GPRS_Para.soft_ver         = 1;
		//心跳周期60秒
		GPRS_Para.heartbeat_period = 20;	
		
		SaveParam();							/* 将新参数写入Flash */
	}
	else
	{
/***************MODBUS相关参数***********************/	
		//硬件版本
		GPRS_Para.hard_ver           = g_tParam.GPRS_Para.hard_ver;
		//软件版本
		GPRS_Para.soft_ver           = g_tParam.GPRS_Para.soft_ver;
		GPRS_Para.heartbeat_period   = g_tParam.GPRS_Para.heartbeat_period;
		
  }
}

/*
*********************************************************************************************************
*	函 数 名: SaveParam
*	功能说明: 将全局变量g_tParam 写入到CPU内部Flash
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void SaveParam(void)
{
/***************MODBUS相关参数***********************/	
	//硬件版本
	g_tParam.GPRS_Para.hard_ver 				  = GPRS_Para.hard_ver;
	//软件版本
	g_tParam.GPRS_Para.soft_ver 				  = GPRS_Para.soft_ver;
	g_tParam.GPRS_Para.heartbeat_period   = GPRS_Para.heartbeat_period;
		
#ifdef PARAM_SAVE_TO_FLASH
	/* 将全局的参数变量保存到 CPU Flash */
	bsp_WriteCpuFlash(PARAM_ADDR, (unsigned char *)&g_tParam, sizeof(PARAM_T));
#endif

#ifdef PARAM_SAVE_TO_EEPROM
	/* 将全局的参数变量保存到EEPROM */
	ee_WriteBytes((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
#endif

#ifdef PARAM_SAVE_TO_W25Q16
	/* 将全局的参数变量保存到外部FLASH */
	sf_WriteBuffer((uint8_t *)&g_tParam, PARAM_ADDR, sizeof(PARAM_T));
#endif
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
