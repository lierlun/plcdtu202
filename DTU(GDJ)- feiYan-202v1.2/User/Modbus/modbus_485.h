#ifndef _MODBUS_RF485_H_
#define _MODBUS_RF485_H_
#include <stdint.h>
#include "modbus.h"

#define UART485_SLAVE_CNT        4                    			//����״̬�£�֧�ֵĴӻ�����

//////////////////////��������С����///////////////////////////////////////////////////////
#define	UART485_RXBUFLEN			(512)		     										//���ּĴ���������ȡ���������
#define	UART485_TXBUFLEN			(512)		    						  			//���ּĴ�������д����������

//////////////////////PDU//////////////////////////////////////////////////////////////////
#define UART485_BITCOIL_BADDR	0	      											  //��PLCͨ��ʱ����Ȧ����ַ
#define	UART485_REGHOLD_BADDR	0  		    											//��PLCͨ��ʱ�����ּĴ�������ַ
#define UART485_BITINP_BADDR	0		    											  //��PLCͨ��ʱ����ɢ���������ַ
#define	UART485_REGINP_BADDR	0	      										    //��PLCͨ��ʱ������Ĵ�������ַ

#define	MAX3_BITINP_D16			2						                	  	//��ɢ����D16�洢����
#define	MAX3_BITINP					(MAX3_BITINP_D16*D16BITNUM)				//��ɢ����D16�洢����
#define	MAX3_BITCOIL_D16		2				              	  	  //��ȦD16�洢����
#define	MAX3_BITCOIL				(MAX3_BITCOIL_D16*D16BITNUM)	  	//��Ȧ����
#define	MAX3_REGINP_D16			2						                	  	//����Ĵ���D16�洢����
#define	MAX3_REGINP					MAX3_REGINP_D16				        		//����Ĵ�������
#define	MAX3_REGHOLD_D16		200					                	  	//���ּĴ���D16�洢����
#define	MAX3_REGHOLD				MAX3_REGHOLD_D16			          	//���ּĴ�������

typedef struct 
{
	uint16_t BitInp[MAX3_BITINP_D16];		         			        	//��ɢ��������ֻ��
	uint16_t BitCoil[MAX3_BITCOIL_D16];            			     	  //��Ȧ����д
	uint16_t RegInp[MAX3_REGINP_D16];	            			     	  //����Ĵ�����ֻ��
	uint16_t RegHold[MAX3_REGHOLD_D16];            			     	  //���ּĴ�������д
}UART485_PDU;

//485����
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
