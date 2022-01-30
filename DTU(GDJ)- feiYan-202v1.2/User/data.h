#ifndef __DATA__H
#define __DATA__H
#include  <includes.h>

//modbus�ӻ���ַ���壬����ѡ��433����SIM800C���Ӵ˴����ã�SIM800Cʱһ��̶�Ϊ1
#define SLAVE_ADDR        1
#define MAX_LINK            6
//ʱ�䶨��
#define  TIME_SEC                 (1000)
#define  TIME_MIN                 (60*1000)
#define  TIME_HOUR                (60*60*1000)


//�������ʱ��֡
// #define SIM800C_TMR_ID	          (TMR_COUNT - 1)
// //���ڶ�ʱ�ж�����״̬
// #define SIM800C_TMR_ID1	          (TMR_COUNT - 2)
// #define SIM800C_TMR_CHECK_RUN     (TMR_COUNT - 3)
// #define SIM800C_TMR_CHECK_NET     (TMR_COUNT - 4)
// #define TIMER_485                 (TMR_COUNT - 5)
// #define TIMER_SIM									(TMR_COUNT - 6)

#define NET_CHECK_TMR_ID          (TMR_COUNT - 1) //����״̬��ѯ��ʱ��
#define NET_POWER_TMR_ID          (TMR_COUNT - 2) //�����Դ���
#define NET_TIMEOUT_TMR_ID        (TMR_COUNT - 3) //����״̬Ӧ��ʱ��ʱ��  
#define NTP_TIME_TMR_ID           (TMR_COUNT - 4) //������ʱ��־
#define NET_SEND_TMR_ID           (TMR_COUNT - 5) //������Ϣ���Ͷ�ʱ��
#define NET_MAX_LINK_TMR_ID       (TMR_COUNT - 6) //�����������ʱ��
#define TIMER_485                 (TMR_COUNT - 7)
#define TIMER_SIM									(TMR_COUNT - 8)
#define NET_SEND_DELAY_TMR_ID     (TMR_COUNT - 9) //���緢����ʱ��ʱ��,208
#define SEND_HEARTBEAT_TMR_ID     (TMR_COUNT - 10) //����ȫ�������������

//Modbus����
typedef struct
{
	uint16_t   hard_ver;          //Ӳ���汾
	uint16_t   soft_ver;          //����汾
	uint16_t   heartbeat_period;  //��������
	uint16_t   gprs_rssi;         //�ź�����
	
	//Ӧ�ñ���
	uint8_t    MaxSlaveCnt;       //485�ӻ�����
  uint8_t    DataFlag;          //���ݱ�־	
	uint8_t    OnlineSlaveCnt;    //���ߴ��豸����
	uint32_t   OnlineFlag;        //���߱�־��ÿһλ����һ���豸
	uint16_t   heartbeat_sn;
	
	uint8_t		SaveFlag;				
	
	//�����Ӳ���luat
	uint8_t    NetDataSendFlag;
	uint8_t    ImeiGetOKFlag;  //imei��ȡ��־
	uint8_t 		IccidGetOKFlag;       //ICCID��ȡ��־
	uint8_t    NtpTimeOKFlag;   //iccid��ȡ��־
	//AIR208ģ�鲿��
	uint8_t 		Air208RunSta;         //����״̬��0���ػ���1������δ������2������
	uint8_t 		Air208RunStep;        //����ģ�����в�
//	uint8_t 		IMEI[10];             //hex
//	uint8_t 		ICCID[41];            //hex
	uint8_t 		sIMEI[16];            //str
	uint8_t 		sICCID[21];           //str
	uint8_t 		link_flag[MAX_LINK];
	uint8_t     NetSendFlag;          //���緢�ͱ�־
	
	uint16_t    heatFrame_sn;         //�������
	uint32_t    ParamChangeFlag;       //�����仯��־
	uint8_t     FeiYanLink[31];      //֮ǰ��78+1��,����ǰ�����plc�̶�д��
	uint8_t     IMEISendCntFlag;
	uint8_t     CSQ;
	
}GPRS_PARA;
//Modbus����
typedef struct
{
	uint8_t    ICCID[20];         //ICCID
}ICCID_SAVE;	

extern GPRS_PARA         GPRS_Para;
extern ICCID_SAVE				ICCID_Save;

extern void    Init_SoftPara(void);
extern void    Para_Save_Check(void);
extern void    GPRS_Read(void);
extern void    GPRS_Write(void);
extern void    UART485_Read(void);
extern void    UART485_Write(void);
extern void    CpuReset(void);
extern void    JsonTask(void);
extern void    Led_Task(void);
#endif
