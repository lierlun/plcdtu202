#ifndef __DATA__H
#define __DATA__H
#include  <includes.h>

//modbus从机地址定义，无论选择433还是SIM800C均从此处配置，SIM800C时一般固定为1
#define SLAVE_ADDR        1
#define MAX_LINK            6
//时间定义
#define  TIME_SEC                 (1000)
#define  TIME_MIN                 (60*1000)
#define  TIME_HOUR                (60*60*1000)


//用于命令超时成帧
// #define SIM800C_TMR_ID	          (TMR_COUNT - 1)
// //用于定时判断网络状态
// #define SIM800C_TMR_ID1	          (TMR_COUNT - 2)
// #define SIM800C_TMR_CHECK_RUN     (TMR_COUNT - 3)
// #define SIM800C_TMR_CHECK_NET     (TMR_COUNT - 4)
// #define TIMER_485                 (TMR_COUNT - 5)
// #define TIMER_SIM									(TMR_COUNT - 6)

#define NET_CHECK_TMR_ID          (TMR_COUNT - 1) //网络状态查询定时器
#define NET_POWER_TMR_ID          (TMR_COUNT - 2) //网络电源检测
#define NET_TIMEOUT_TMR_ID        (TMR_COUNT - 3) //网络状态应答超时定时器  
#define NTP_TIME_TMR_ID           (TMR_COUNT - 4) //网络授时标志
#define NET_SEND_TMR_ID           (TMR_COUNT - 5) //网络消息发送定时器
#define NET_MAX_LINK_TMR_ID       (TMR_COUNT - 6) //网络最大连接时间
#define TIMER_485                 (TMR_COUNT - 7)
#define TIMER_SIM									(TMR_COUNT - 8)
#define NET_SEND_DELAY_TMR_ID     (TMR_COUNT - 9) //网络发送延时定时器,208
#define SEND_HEARTBEAT_TMR_ID     (TMR_COUNT - 10) //发送全部参数心跳间隔

//Modbus参数
typedef struct
{
	uint16_t   hard_ver;          //硬件版本
	uint16_t   soft_ver;          //软件版本
	uint16_t   heartbeat_period;  //心跳周期
	uint16_t   gprs_rssi;         //信号质量
	
	//应用变量
	uint8_t    MaxSlaveCnt;       //485从机个数
  uint8_t    DataFlag;          //数据标志	
	uint8_t    OnlineSlaveCnt;    //在线从设备个数
	uint32_t   OnlineFlag;        //在线标志，每一位代表一个设备
	uint16_t   heartbeat_sn;
	
	uint8_t		SaveFlag;				
	
	//新增加部分luat
	uint8_t    NetDataSendFlag;
	uint8_t    ImeiGetOKFlag;  //imei获取标志
	uint8_t 		IccidGetOKFlag;       //ICCID获取标志
	uint8_t    NtpTimeOKFlag;   //iccid获取标志
	//AIR208模块部分
	uint8_t 		Air208RunSta;         //运行状态，0：关机；1：开机未联网；2：联网
	uint8_t 		Air208RunStep;        //网络模块运行步
//	uint8_t 		IMEI[10];             //hex
//	uint8_t 		ICCID[41];            //hex
	uint8_t 		sIMEI[16];            //str
	uint8_t 		sICCID[21];           //str
	uint8_t 		link_flag[MAX_LINK];
	uint8_t     NetSendFlag;          //网络发送标志
	
	uint16_t    heatFrame_sn;         //心跳序号
	uint32_t    ParamChangeFlag;       //参数变化标志
	uint8_t     FeiYanLink[31];      //之前是78+1个,现在前面的由plc固定写死
	uint8_t     IMEISendCntFlag;
	uint8_t     CSQ;
	
}GPRS_PARA;
//Modbus参数
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
