/*
*********************************************************************************************************
*
*	模块名称 : DHT11 驱动模块(1-wire 数字温湿度传感器）
*	文件名称 : bsp_dht11.h
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_DHT21_H
#define _BSP_DHT21_H

typedef struct
{
	uint8_t Buf[5];
	double  Temp;		/* Temperature 温度 摄氏度 */
	double  Hum;		/* Humidity 湿度 百分比 */
}DHT21_T;

void bsp_InitDHT21(void);
uint8_t DHT21_ReadData(DHT21_T *_pDHT);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
