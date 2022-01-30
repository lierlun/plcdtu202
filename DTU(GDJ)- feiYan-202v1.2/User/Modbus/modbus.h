#ifndef _MODBUS_H_
#define _MODBUS_H_
#include <stdint.h>

#define ON			                        1
#define OFF			                        0

#define ASCII                           0
#define RTU                             1

#define D8BITNUM			                  8
#define D16BITNUM			                  16


//MODBUS
#define MB_BITCOIL_RD		    0x01		//����Ȧ
#define MB_BITINP_RD		    0x02		//����ɢ������
#define MB_REGHOLD_RD		    0x03		//�����ּĴ���
#define MB_REGINP_RD		    0x04		//������Ĵ���
#define MB_BITCOIL_WT		    0x05		//д������Ȧ
#define MB_REGHOLD_WT		    0x06		//д�������ּĴ���
#define MB_MULTICOIL_WT		  0x0F		//д�����Ȧ
#define MB_MULTIREG_WT		  0x10		//д������ּĴ���

#define MB_ERR_FUNCODE		  0x01		//�Ƿ�����
#define MB_ERR_ADDR		      0x02		//�Ƿ����ݵ�ַ
#define MB_ERR_DATE		      0x03		//�Ƿ�����ֵ
#define MB_ERR_SLAVE		    0x04		//���豸����
#define MB_ERR_HANDLE		    0x05		//ִ��ȷ��
#define MB_ERR_SLBUSY		    0x06		//���豸æ
#define MB_ERR_ODDEVEN		  0x07		//�洢��żУ�����


#define	MBMAX_BITINP_RD		2000L		  //��ɢ������������ȡ�������
#define	MBMAX_BITCOIL_RD	2000L		  //��Ȧ������ȡ���������
#define	MBMAX_BITCOIL_WT	2000L		  //��Ȧ����д����������
#define	MBMAX_REGINP_RD		512L		  //����Ĵ���������ȡ���������
#define	MBMAX_REGHOLD_RD	512L		  //���ּĴ���������ȡ���������
#define	MBMAX_REGHOLD_WT	512L		  //���ּĴ�������д����������


#define	MB_OFF				  (uint16_t)0x0000
#define	MB_ON				    (uint16_t)0xFF00

	                        
typedef union	_DAT16
{
	uint16_t d16;
	struct
	{
		uint8_t Lo;
		uint8_t Hi;
	}d8;
}uDAT16;

typedef union	_DAT32
{
	uint32_t d32;
	union _DAT16 d16[2];
}uDAT32;


#define PERIOD_CNT            0xffff
//modbus��������ṹ��
typedef struct
{
	uint16_t slave;           //�ӻ���ַ
	uint16_t func;	          //������			
	uint16_t addr;				    //�׵�ַ
	uint16_t cnt;							//����ȡ����
	uint32_t startTimeout;    //ͨѶ��ʱ��ʼʱ��            
	uint32_t timeout;					//ͨѶ��ʱʱ����
	uint32_t startPeriod;     //ͨѶ������ʼʱ��            
	uint32_t period;          //ͨѶ���ڼ��     
	uint16_t commCnt;         //���������ͨѶ������0xffff��ʾ���޴���   
	uint16_t timeoutcnt;      //�������
}_CMD;

//modbus����״̬
typedef struct 
{
	uint16_t rep;																				
	uint8_t  PLCCOMFlag;	 	            
}_MTCMD;

#endif
