#ifndef _MODBUS_GPRS_H_
#define _MODBUS_GPRS_H_
#include <stdint.h>
#include "modbus.h"

#define GPRS_SLAVE_ADDR    0

//////////////////////缓冲区大小定义///////////////////////////////////////////////////////
#define	GPRS_RXBUFLEN			(512)		     							      //保持寄存器连续读取的最大数量
#define	GPRS_TXBUFLEN			(512)		    						        //保持寄存器连续写入的最大数量

//////////////////////PDU//////////////////////////////////////////////////////////////////
#define GPRS_BITCOIL_BADDR	0	      											//与PLC通信时，线圈基地址
#define	GPRS_REGHOLD_BADDR	0		    											//与PLC通信时，保持寄存器基地址
#define GPRS_BITINP_BADDR	  0		    											//与PLC通信时，离散量输入基地址
#define	GPRS_REGINP_BADDR	  0	      											//与PLC通信时，输入寄存器基地址

#define	MAX1_BITINP_D16		2					                    //离散输入D16存储个数
#define	MAX1_BITINP				(MAX1_BITINP_D16*D16BITNUM)		  //离散输入D16存储个数
#define	MAX1_BITCOIL_D16	50						                  //线圈D16存储数量
#define	MAX1_BITCOIL			(MAX1_BITCOIL_D16*D16BITNUM)		//线圈数量
#define	MAX1_REGINP_D16	  2						                      //输入寄存器D16存储数量
#define	MAX1_REGINP				MAX1_REGINP_D16				          //输入寄存器数量
#define	MAX1_REGHOLD_D16	800					                  //保持寄存器D16存储数量
#define	MAX1_REGHOLD			MAX1_REGHOLD_D16			          //保持寄存器数量

typedef struct 
{
	uint16_t BitInp[MAX1_BITINP_D16];		                    //离散输入量，只读
	uint16_t BitCoil[MAX1_BITCOIL_D16];                     //线圈，读写
	uint16_t RegInp[MAX1_REGINP_D16];	                      //输入寄存器，只读
	uint16_t RegHold[MAX1_REGHOLD_D16];                     //保持寄存器，读写
}GPRS_PDU;

//GPRS发送
// extern uint8_t SIM800C_Send(uint8_t *pBuf,uint16_t len);
// #define  GPRS_SEND(a,b)      SIM800C_Send(a,b);

extern uint8_t 		GprsRxDat[GPRS_RXBUFLEN];	     
extern uint8_t 		GprsSentdat[GPRS_TXBUFLEN];	
extern GPRS_PDU   g_PduGprs;

extern uint16_t GPRS_Slave(char *pBuf,uint16_t len);
extern uint16_t GPRS_MbSl_rep(_CMD Cmd,GPRS_PDU *Pdu);
#endif
