/*
*********************************************************************************************************
*
*	ģ������ : DHT21 ����ģ��(1-wire �����¶ȴ�������
*	�ļ����� : bsp_dht21.c
*	��    �� : V1.0
*	˵    �� : DH211��CPU֮�����1��GPIO�ӿڡ�������� DHT21_ReadData()��Ƶ�ʲ�Ҫ����1Hz��
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2014-01-24  armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	DHT21 �����öŰ������ӵ������塣

     DHT21      STM32F407������
	  VCC   ------  3.3V
	  DQ    ------  PB14   (���������� 4.7K ��������. �� USER ������)
	  GND   ------  GND
*/

/* ����GPIO�˿� */
#define RCC_DQ		RCC_APB2Periph_GPIOB
#define PORT_DQ		GPIOB
#define PIN_DQ		GPIO_Pin_14

#define DQ_0()		PORT_DQ->BRR = PIN_DQ
#define DQ_1()		PORT_DQ->BSRR = PIN_DQ

/* �ж�DQ�����Ƿ�Ϊ�� */
#define DQ_IS_LOW()	((PORT_DQ->IDR & PIN_DQ) == 0)


static uint8_t DHT21_ReadByte(void);

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitDHT21
*	����˵��: ����STM32��GPIO��SPI�ӿڣ��������� DHT11
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitDHT21(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_DQ, ENABLE);

	DQ_1();

	/* ����DQΪ��©��� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	

	GPIO_InitStructure.GPIO_Pin = PIN_DQ;
	GPIO_Init(PORT_DQ, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: DHT21_ReadData
*	����˵��: ��λDHT11�� ����DQΪ�ͣ���������480us��Ȼ��ȴ�
*	��    ��: ��
*	�� �� ֵ: 0 ʧ�ܣ� 1 ��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t DHT21_ReadData(DHT21_T *_pDTH)
{
#if uCOS_EN == 1 
	CPU_SR_ALLOC();
#endif
	uint8_t   sum;
	uint16_t  temp;
	uint8_t   i;
  uint8_t   cnt = 0;
	
	DISABLE_INT();/* ��ֹȫ���ж� */
	
	/* 1. ����������ʼ�ź�, DQ����ʱ�� 600us */
	DQ_0();		/* DQ = 0 */
	bsp_DelayUS(600);
	DQ_1();		/* DQ = 1 */

	bsp_DelayUS(30);	/* �ȴ�30us */

	if(DQ_IS_LOW())
	{
			cnt = 0;
			while(DQ_IS_LOW())     //DHT����80us
			{
				bsp_DelayUS(1);
				cnt++;
				if(cnt > 150)
				{
					ENABLE_INT();	/* ʹ��ȫ���ж� */
					return 0;
				}
			}

			cnt = 0;
			while(!DQ_IS_LOW())    //DHT����80us
			{
				bsp_DelayUS(1);
				cnt++;
				if(cnt > 150)
				{
					ENABLE_INT();	/* ʹ��ȫ���ж� */
					return 0;
				}  
			}

			for(i=0;i<5;i++)
			{
				_pDTH->Buf[i] = DHT21_ReadByte();
      }

			bsp_DelayUS(100);
			/* ����У��� */
			sum = _pDTH->Buf[0] + _pDTH->Buf[1] + _pDTH->Buf[2] + _pDTH->Buf[3];
			if (sum == _pDTH->Buf[4])
			{
				//�����¶�
				temp = (((uint16_t)_pDTH->Buf[2])<<8)|_pDTH->Buf[3];
				//�ж��¶��Ƿ�Ϊ����
				if(temp&0x8000)
				{
					_pDTH->Temp = -(double)temp/10.0;	
				}
				else
				{
					_pDTH->Temp = (double)temp/10.0;		
				}
				
				//����ʪ��
				temp = (((uint16_t)_pDTH->Buf[0])<<8)|_pDTH->Buf[1];
				
				_pDTH->Hum = (double)temp/10.0;	
				ENABLE_INT();	/* ʹ��ȫ���ж� */
				return 1;
			}
  }
	
	ENABLE_INT();	/* ʹ��ȫ���ж� */
	return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: DHT21_ReadByte
*	����˵��: ��DHT21��ȡ1�ֽ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static uint8_t DHT21_ReadByte(void)
{
  uint8_t i;
  uint8_t temp = 0;
  uint8_t cnt = 0;
  for(i=0;i<8;i++)
  {
    cnt = 0;
    while(DQ_IS_LOW()) //�ȴ�DHT�͵�ƽ��ʼ,DHT����50us�����ݿ�ʼ
    {
			bsp_DelayUS(1); 
      cnt++;
      if(cnt > 150)
      {
        return 0;
      }    
    }
    bsp_DelayUS(40);                 //����ߵ�ƽʱ��26~28us��ʾ"0",70us��ʾ"1"���˴���ʱ40us���ж�
    if(!DQ_IS_LOW())                 //�����Ϊ�ߵ�ƽ����Ϊ"1"
    {
      temp |= (0x80>>i);
    }
    cnt = 0;
    while(!DQ_IS_LOW())     //�ȴ�DHT�ߵ�ƽ����
    {
			bsp_DelayUS(1); 
      cnt++;
      if(cnt > 150)
      {
        return 0;
      }     
    }
  }
  return temp;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
