/*
*********************************************************************************************************
*
*	ģ������ : DHT11 ����ģ��(1-wire ������ʪ�ȴ�������
*	�ļ����� : bsp_dht11.h
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_DHT21_H
#define _BSP_DHT21_H

typedef struct
{
	uint8_t Buf[5];
	double  Temp;		/* Temperature �¶� ���϶� */
	double  Hum;		/* Humidity ʪ�� �ٷֱ� */
}DHT21_T;

void bsp_InitDHT21(void);
uint8_t DHT21_ReadData(DHT21_T *_pDHT);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
