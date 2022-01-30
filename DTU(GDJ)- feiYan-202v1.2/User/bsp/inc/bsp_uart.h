/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
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

/* RS485芯片发送使能GPIO, PB1 */
#define RCC_RS485_TXEN 	 RCC_APB2Periph_GPIOB
#define PORT_RS485_TXEN  GPIOB
#define PIN_RS485_TXEN	 GPIO_Pin_1

#define RS485_RX_EN()	PORT_RS485_TXEN->BRR = PIN_RS485_TXEN
#define RS485_TX_EN()	PORT_RS485_TXEN->BSRR = PIN_RS485_TXEN


/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1  PA9, PA10  */
	COM2 = 1,	/* USART2, PA2, PA3   */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11  */
	COM5 = 4,	/* UART5, PC12, PD2   */
}COM_PORT_E;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
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

/* 串口设备结构体 */
typedef struct
{
	COM_PORT_E    ucPort;         /* 串口号 */ 
	USART_TypeDef *uart;		      /* STM32内部串口设备指针 */
	uint8_t       *pTxBuf;		  	/* 发送缓冲区 */
	uint8_t       *pRxBuf;			  /* 接收缓冲区 */
	uint16_t      usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t      usRxBufSize;		/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;			/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;			  /* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;			/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;			/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;			  /* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;			/* 还未读取的新数据个数 */

	void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
}UART_T;

/* 定义每个串口结构体变量 */
#if UART1_FIFO_EN == 1
 extern UART_T  g_tUart1;
 extern uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
 extern uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART2_FIFO_EN == 1
 extern UART_T  g_tUart2;
 extern uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
 extern uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART3_FIFO_EN == 1
 extern UART_T  g_tUart3;
 extern uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* 发送缓冲区 */
 extern uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART4_FIFO_EN == 1
 extern UART_T  g_tUart4;
 extern uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* 发送缓冲区 */
 extern uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART5_FIFO_EN == 1
 extern UART_T  g_tUart5;
 extern uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* 发送缓冲区 */
 extern uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* 接收缓冲区 */
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

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
