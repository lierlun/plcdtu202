#ifndef _MODBUS_GPRS_H_
#define _MODBUS_GPRS_H_
#include <stdint.h>
#include "modbus.h"

#define GPRS_SLAVE_ADDR    0

//////////////////////��������С����///////////////////////////////////////////////////////
#define	GPRS_RXBUFLEN			(512)		     							      //���ּĴ���������ȡ���������
#define	GPRS_TXBUFLEN			(512)		    						        //���ּĴ�������д����������

//////////////////////PDU//////////////////////////////////////////////////////////////////
#define GPRS_BITCOIL_BADDR	0	      											//��PLCͨ��ʱ����Ȧ����ַ
#define	GPRS_REGHOLD_BADDR	0		    											//��PLCͨ��ʱ�����ּĴ�������ַ
#define GPRS_BITINP_BADDR	  0		    											//��PLCͨ��ʱ����ɢ���������ַ
#define	GPRS_REGINP_BADDR	  0	      											//��PLCͨ��ʱ������Ĵ�������ַ

#define	MAX1_BITINP_D16		2					                    //��ɢ����D16�洢����
#define	MAX1_BITINP				(MAX1_BITINP_D16*D16BITNUM)		  //��ɢ����D16�洢����
#define	MAX1_BITCOIL_D16	50						                  //��ȦD16�洢����
#define	MAX1_BITCOIL			(MAX1_BITCOIL_D16*D16BITNUM)		//��Ȧ����
#define	MAX1_REGINP_D16	  2						                      //����Ĵ���D16�洢����
#define	MAX1_REGINP				MAX1_REGINP_D16				          //����Ĵ�������
#define	MAX1_REGHOLD_D16	800					                  //���ּĴ���D16�洢����
#define	MAX1_REGHOLD			MAX1_REGHOLD_D16			          //���ּĴ�������

typedef struct 
{
	uint16_t BitInp[MAX1_BITINP_D16];		                    //��ɢ��������ֻ��
	uint16_t BitCoil[MAX1_BITCOIL_D16];                     //��Ȧ����д
	uint16_t RegInp[MAX1_REGINP_D16];	                      //����Ĵ�����ֻ��
	uint16_t RegHold[MAX1_REGHOLD_D16];                     //���ּĴ�������д
}GPRS_PDU;

//GPRS����
// extern uint8_t SIM800C_Send(uint8_t *pBuf,uint16_t len);
// #define  GPRS_SEND(a,b)      SIM800C_Send(a,b);

extern uint8_t 		GprsRxDat[GPRS_RXBUFLEN];	     
extern uint8_t 		GprsSentdat[GPRS_TXBUFLEN];	
extern GPRS_PDU   g_PduGprs;

extern uint16_t GPRS_Slave(char *pBuf,uint16_t len);
extern uint16_t GPRS_MbSl_rep(_CMD Cmd,GPRS_PDU *Pdu);
#endif
