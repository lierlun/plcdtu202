#ifndef __SIM800C__H
#define __SIM800C__H
#include  <includes.h>
#include "bsp.h"


//�����������ʾ֧��485͸������
//#define SIM800C_TRANS_485 

// #define RCC_SIM800C 	    (RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA)
// //��Դ����
// #define GPIO_PORT_KEY     GPIOB
// #define GPIO_PIN_KEY	    GPIO_Pin_6

// //״ָ̬ʾ
// #define GPIO_PORT_STATE   GPIOB
// #define GPIO_PIN_STATE 	  GPIO_Pin_5

// //��Դ����
// #define GPIO_PORT_POWER   GPIOA
// #define GPIO_PIN_POWER  	GPIO_Pin_8


//ģ�鿪��
// #define KEY_ON()          GPIO_PORT_KEY->BRR    = GPIO_PIN_KEY
// #define KEY_OFF()         GPIO_PORT_KEY->BSRR   = GPIO_PIN_KEY

// //��Դ����
// #define POWER_ON()        GPIO_PORT_POWER->BSRR  = GPIO_PIN_POWER
// #define POWER_OFF()       GPIO_PORT_POWER->BRR = GPIO_PIN_POWER

// #define STATE             (GPIO_ReadInputData(GPIO_PORT_STATE)&GPIO_PIN_STATE)

// #define SIM800C_UART       COM2

// #define AT_CR		'\r'      //0x0D
// #define AT_LF		'\n'      //0x0A

// #define TIMEOUT                0xff 

// //����·ģʽ��ֻ֧�ַ�͸��ģʽ
// #define SIM800C_TRANSPARENT    1

// typedef struct
// {
//   uint8_t  IpAddr[25];   //IP��ַ
// 	uint8_t  IpPort[8];    //�˿ں�
// 	uint8_t  APN[8];       //���뷽ʽ
// 	uint8_t  SN[16];       //���к�
// 	uint8_t  NetType[6];   //��������,��TCP������UDP��
// 	uint8_t  IMEI[20];     //���к�
// 	uint8_t  ICCID[30];    //ICCID���
// 	int16_t  Rssi;         //�ź�ǿ��
// 	uint16_t Lac;
// 	uint16_t Ci;
// 	
// 	//�м����
// 	uint8_t  Connect_Sta;   //����״̬,1:������
// 	uint8_t  Reconnect_cnt; //��������
// 	uint8_t  ReconnectTotal;//�����ܴ���
// 	uint32_t NetCheckTime;  //��������״̬���ʱ�䣬�������󣬲�ѯʱ���Ƶ��
// 	uint32_t NetCheckTime1; //������ʱ��
// }NETWORK_PARA;

//extern NETWORK_PARA NetPara;

extern void     Init_SIM800C(void);
extern void     Init_SIM800C_Soft(void);
extern uint8_t  SIM800C_HardPowerOn(void);
extern uint8_t  SIM800C_HardPowerOff(void);
extern uint8_t  SIM800C_SwitchCmdMode(void);
extern uint8_t  SIM800C_SwitchDataMode(void);
extern uint8_t  SIM800C_CMD_Init(void);
extern uint8_t  SIM800C_SendAT_WithAck(char *_Cmd, char *_pAckStr, uint16_t _usTimeOut);
extern void     SIM800C_SendAT_NoAck(char *_Cmd);
extern uint8_t  SIM800C_WaitATResponse(char *_pAckStr, uint16_t _usTimeOut);
extern void     SIM800C_SendStr(char *_Str);
extern uint8_t  SIM800C_ATMode(uint8_t sta);
extern uint8_t  Printf_AT(uint8_t _try,uint16_t _usTimeOut,char* _pAckStr, char *fmt,...);
extern uint8_t  Shut_Down_Phone(void);
extern uint8_t  SIM800C_CheckPIN(uint8_t _try,uint16_t _usTimeOut);
//extern uint8_t  SIM800C_ConnectNetwork(NETWORK_PARA *net);
extern uint8_t  SIM800C_Echo_Ctl(uint8_t flag);
extern uint8_t  SIM800C_Check_NetSta(uint8_t _try,uint16_t _usTimeOut);
extern uint8_t  SIM800C_Check_PowerSta(void);
extern uint16_t SIM800C_Recv_Len(uint8_t *pBuf);
extern uint8_t  SIM800C_ReadIMEI(uint8_t *pbuf,uint8_t _try,uint16_t _usTimeOut);
extern uint8_t  SIM800C_ReadICCID(uint8_t *pbuf,uint8_t _try,uint16_t _usTimeOut);
extern uint8_t  SIM800C_ReadRSSI(int16_t *Rssi,uint8_t _try,uint16_t _usTimeOut);
extern uint8_t  SIM800C_Send(uint8_t *pBuf,uint16_t len);
extern uint8_t  SIM800C_ReadLacCi(uint16_t *Lac,uint16_t *Ci,uint8_t _try,uint16_t _usTimeOut);

extern void     Send_RegMsg(uint8_t _try,uint8_t *pIMEI);
extern void     SIM800C_Task(void);


#endif
