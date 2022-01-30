/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "bsp.h"

#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	1
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0

/* RS485оƬ����ʹ��GPIO, PB1 */
#define RCC_RS485_TXEN 	 RCC_APB2Periph_GPIOB
#define PORT_RS485_TXEN  GPIOB
#define PIN_RS485_TXEN	 GPIO_Pin_1

#define RS485_RX_EN()	PORT_RS485_TXEN->BRR = PIN_RS485_TXEN
#define RS485_TX_EN()	PORT_RS485_TXEN->BSRR = PIN_RS485_TXEN


/* ����˿ں� */
typedef enum
{
	COM1 = 0,	/* USART1  PA9, PA10  */
	COM2 = 1,	/* USART2, PA2, PA3   */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11  */
	COM5 = 4,	/* UART5, PC12, PD2   */
}COM_PORT_E;

/* ���崮�ڲ����ʺ�FIFO��������С����Ϊ���ͻ������ͽ��ջ�����, ֧��ȫ˫�� */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD			9600
	#define UART1_TX_BUF_SIZE	1*512
	#define UART1_RX_BUF_SIZE	1*512
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD			115200
	#define UART2_TX_BUF_SIZE	1*512
	#define UART2_RX_BUF_SIZE	1*512
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			9600
	#define UART3_TX_BUF_SIZE	1*512
	#define UART3_RX_BUF_SIZE	1*512
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			9600
	#define UART4_TX_BUF_SIZE	1*512
	#define UART4_RX_BUF_SIZE	1*512
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			115200
	#define UART5_TX_BUF_SIZE	1*1024
	#define UART5_RX_BUF_SIZE	1*1024
#endif

/* �����豸�ṹ�� */
typedef struct
{
	COM_PORT_E    ucPort;         /* ���ں� */ 
	USART_TypeDef *uart;		      /* STM32�ڲ������豸ָ�� */
	uint8_t       *pTxBuf;		  	/* ���ͻ����� */
	uint8_t       *pRxBuf;			  /* ���ջ����� */
	uint16_t      usTxBufSize;		/* ���ͻ�������С */
	uint16_t      usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;			/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;			  /* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;			/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;			/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;			  /* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;			/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint8_t _byte);	/* �����յ����ݵĻص�����ָ�� */
}UART_T;

/* ����ÿ�����ڽṹ����� */
#if UART1_FIFO_EN == 1
 extern UART_T  g_tUart1;
 extern uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* ���ͻ����� */
 extern uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART2_FIFO_EN == 1
 extern UART_T  g_tUart2;
 extern uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
 extern uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART3_FIFO_EN == 1
 extern UART_T  g_tUart3;
 extern uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* ���ͻ����� */
 extern uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART4_FIFO_EN == 1
 extern UART_T  g_tUart4;
 extern uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* ���ͻ����� */
 extern uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART5_FIFO_EN == 1
 extern UART_T  g_tUart5;
 extern uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* ���ͻ����� */
 extern uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* ���ջ����� */
#endif

void     bsp_InitUart(void);
void     comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void     comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t  comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);

void     comClearTxFifo(COM_PORT_E _ucPort);
void     comClearRxFifo(COM_PORT_E _ucPort);
void     comClearFifo(COM_PORT_E _ucPort);
uint16_t comGetWaitSend(COM_PORT_E _ucPort);
void     RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void     RS485_SendStr(char *_pBuf);
void     bsp_SetUartBaud(COM_PORT_E _ucPort,uint32_t _baud);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
