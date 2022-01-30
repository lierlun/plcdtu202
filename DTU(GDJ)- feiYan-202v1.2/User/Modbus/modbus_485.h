#ifndef _MODBUS_RF485_H_
#define _MODBUS_RF485_H_
#include <stdint.h>
#include "modbus.h"

#define UART485_SLAVE_CNT        4                    			//主机状态下，支持的从机个数

//////////////////////缓冲区大小定义///////////////////////////////////////////////////////
#define	UART485_RXBUFLEN			(512)		     										//保持寄存器连续读取的最大数量
#define	UART485_TXBUFLEN			(512)		    						  			//保持寄存器连续写入的最大数量

//////////////////////PDU//////////////////////////////////////////////////////////////////
#define UART485_BITCOIL_BADDR	0	      											  //与PLC通信时，线圈基地址
#define	UART485_REGHOLD_BADDR	0  		    											//与PLC通信时，保持寄存器基地址
#define UART485_BITINP_BADDR	0		    											  //与PLC通信时，离散量输入基地址
#define	UART485_REGINP_BADDR	0	      										    //与PLC通信时，输入寄存器基地址

#define	MAX3_BITINP_D16			2						                	  	//离散输入D16存储个数
#define	MAX3_BITINP					(MAX3_BITINP_D16*D16BITNUM)				//离散输入D16存储个数
#define	MAX3_BITCOIL_D16		2				              	  	  //线圈D16存储数量
#define	MAX3_BITCOIL				(MAX3_BITCOIL_D16*D16BITNUM)	  	//线圈数量
#define	MAX3_REGINP_D16			2						                	  	//输入寄存器D16存储数量
#define	MAX3_REGINP					MAX3_REGINP_D16				        		//输入寄存器数量
#define	MAX3_REGHOLD_D16		200					                	  	//保持寄存器D16存储数量
#define	MAX3_REGHOLD				MAX3_REGHOLD_D16			          	//保持寄存器数量

typedef struct 
{
	uint16_t BitInp[MAX3_BITINP_D16];		         			        	//离散输入量，只读
	uint16_t BitCoil[MAX3_BITCOIL_D16];            			     	  //线圈，读写
	uint16_t RegInp[MAX3_REGINP_D16];	            			     	  //输入寄存器，只读
	uint16_t RegHold[MAX3_REGHOLD_D16];            			     	  //保持寄存器，读写
}UART485_PDU;

//485发送
#define  UART_485             COM1
#define  UART485_RXBUF        g_RxBuf1
#define  UART485_tUART        g_tUart1
//#define  UART485_SEND(a,b)    (Sendlen = b);
#define  UART485_SEND(a,b)    comSendBuf(UART_485, a, b);

extern uint8_t 		  Uart485RxDat[UART485_RXBUFLEN];	     
extern uint8_t 		  Uart485Sentdat[UART485_TXBUFLEN];	
extern UART485_PDU  g_PduUART485[UART485_SLAVE_CNT];
extern UART485_PDU  g_PduUART485_bak[UART485_SLAVE_CNT];

extern void     Uart485_Task(void);
extern void     UART485_Init(void);
extern void     Para_Change_Check(void);
extern uint8_t  UART485_Master(uint32_t *addr);
extern uint16_t UART485_Slave(void);
extern uint16_t UART485_MbMt_req(_CMD Cmd,UART485_PDU *Pdu);
extern uint16_t UART485_MbMt_rep(_CMD Cmd,UART485_PDU *Pdu);
extern uint16_t UART485_MbSl_rep(_CMD Cmd,UART485_PDU *Pdu);
#endif
