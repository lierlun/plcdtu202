#ifndef _MODBUS_RF433_H_
#define _MODBUS_RF433_H_
#include <stdint.h>
#include "modbus.h"

#define RF433_SLAVE_CNT        1                      			//����״̬�£�֧�ֵĴӻ�����

//////////////////////��������С����///////////////////////////////////////////////////////
#define	RF433_RXBUFLEN			(512)		     										//���ּĴ���������ȡ���������
#define	RF433_TXBUFLEN			(512)		    						  			//���ּĴ�������д����������

//////////////////////PDU//////////////////////////////////////////////////////////////////
#define RF433_BITCOIL_BADDR	0	      											  //��PLCͨ��ʱ����Ȧ����ַ
#define	RF433_REGHOLD_BADDR	0		    											  //��PLCͨ��ʱ�����ּĴ�������ַ
#define RF433_BITINP_BADDR	0		    											  //��PLCͨ��ʱ����ɢ���������ַ
#define	RF433_REGINP_BADDR	0	      											  //��PLCͨ��ʱ������Ĵ�������ַ

#define	MAX2_BITINP_D16			6						                	  //��ɢ����D16�洢����
#define	MAX2_BITINP					(MAX2_BITINP_D16*D16BITNUM)			//��ɢ����D16�洢����
#define	MAX2_BITCOIL_D16		16						              	  //��ȦD16�洢����
#define	MAX2_BITCOIL				(MAX2_BITCOIL_D16*D16BITNUM)	  //��Ȧ����
#define	MAX2_REGINP_D16			6						                	  //����Ĵ���D16�洢����
#define	MAX2_REGINP					MAX2_REGINP_D16				        	//����Ĵ�������
#define	MAX2_REGHOLD_D16		1000					                	//���ּĴ���D16�洢����
#define	MAX2_REGHOLD				MAX2_REGHOLD_D16			          //���ּĴ�������

typedef struct 
{
	uint16_t BitInp[MAX2_BITINP_D16];		         			        //��ɢ��������ֻ��
	uint16_t BitCoil[MAX2_BITCOIL_D16];            			      //��Ȧ����д
	uint16_t RegInp[MAX2_REGINP_D16];	            			      //����Ĵ�����ֻ��
	uint16_t RegHold[MAX2_REGHOLD_D16];            			      //���ּĴ�������д
}RF433_PDU;

//433����
#define  UART_433           COM1
#define  RF433_SEND(a,b)    comSendBuf(UART_433, a, b);
//��������
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
