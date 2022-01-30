#ifndef _MODBUS_RF433_H_
#define _MODBUS_RF433_H_
#include <stdint.h>
#include "modbus.h"

#define RF433_SLAVE_CNT        1                      			//主机状态下，支持的从机个数

//////////////////////缓冲区大小定义///////////////////////////////////////////////////////
#define	RF433_RXBUFLEN			(512)		     										//保持寄存器连续读取的最大数量
#define	RF433_TXBUFLEN			(512)		    						  			//保持寄存器连续写入的最大数量

//////////////////////PDU//////////////////////////////////////////////////////////////////
#define RF433_BITCOIL_BADDR	0	      											  //与PLC通信时，线圈基地址
#define	RF433_REGHOLD_BADDR	0		    											  //与PLC通信时，保持寄存器基地址
#define RF433_BITINP_BADDR	0		    											  //与PLC通信时，离散量输入基地址
#define	RF433_REGINP_BADDR	0	      											  //与PLC通信时，输入寄存器基地址

#define	MAX2_BITINP_D16			6						                	  //离散输入D16存储个数
#define	MAX2_BITINP					(MAX2_BITINP_D16*D16BITNUM)			//离散输入D16存储个数
#define	MAX2_BITCOIL_D16		16						              	  //线圈D16存储数量
#define	MAX2_BITCOIL				(MAX2_BITCOIL_D16*D16BITNUM)	  //线圈数量
#define	MAX2_REGINP_D16			6						                	  //输入寄存器D16存储数量
#define	MAX2_REGINP					MAX2_REGINP_D16				        	//输入寄存器数量
#define	MAX2_REGHOLD_D16		1000					                	//保持寄存器D16存储数量
#define	MAX2_REGHOLD				MAX2_REGHOLD_D16			          //保持寄存器数量

typedef struct 
{
	uint16_t BitInp[MAX2_BITINP_D16];		         			        //离散输入量，只读
	uint16_t BitCoil[MAX2_BITCOIL_D16];            			      //线圈，读写
	uint16_t RegInp[MAX2_REGINP_D16];	            			      //输入寄存器，只读
	uint16_t RegHold[MAX2_REGHOLD_D16];            			      //保持寄存器，读写
}RF433_PDU;

//433发送
#define  UART_433           COM1
#define  RF433_SEND(a,b)    comSendBuf(UART_433, a, b);
//设置引脚
#define RCC_ALL_SET433 	    RCC_APB2Periph_GPIOA
#define GPIO_PORT_SET433    GPIOA
#define GPIO_PIN_SET433	    GPIO_Pin_8

extern uint8_t 		RF433RxDat[RF433_RXBUFLEN];	     
extern uint8_t 		RF433Sentdat[RF433_TXBUFLEN];	
extern RF433_PDU  g_PduRF433[RF433_SLAVE_CNT];

extern uint16_t RF433_Slave(void);
extern uint16_t RF433_MbMt_req(_CMD Cmd,RF433_PDU *Pdu);
extern uint16_t RF433_MbMt_rep(_CMD Cmd,RF433_PDU *Pdu);
extern uint16_t RF433_MbSl_rep(_CMD Cmd,RF433_PDU *Pdu);

extern uint8_t  RF33_Set(uint8_t cmd,uint8_t value);
extern void     RF433_SwitchAT(uint8_t sta);

#endif
