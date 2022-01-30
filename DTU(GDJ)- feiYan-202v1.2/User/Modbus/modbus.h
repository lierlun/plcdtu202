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
#define MB_BITCOIL_RD		    0x01		//读线圈
#define MB_BITINP_RD		    0x02		//读离散量输入
#define MB_REGHOLD_RD		    0x03		//读保持寄存器
#define MB_REGINP_RD		    0x04		//读输入寄存器
#define MB_BITCOIL_WT		    0x05		//写单个线圈
#define MB_REGHOLD_WT		    0x06		//写单个保持寄存器
#define MB_MULTICOIL_WT		  0x0F		//写多个线圈
#define MB_MULTIREG_WT		  0x10		//写多个保持寄存器

#define MB_ERR_FUNCODE		  0x01		//非法功能
#define MB_ERR_ADDR		      0x02		//非法数据地址
#define MB_ERR_DATE		      0x03		//非法数据值
#define MB_ERR_SLAVE		    0x04		//从设备故障
#define MB_ERR_HANDLE		    0x05		//执行确认
#define MB_ERR_SLBUSY		    0x06		//从设备忙
#define MB_ERR_ODDEVEN		  0x07		//存储奇偶校验错误


#define	MBMAX_BITINP_RD		2000L		  //离散输入量连续读取最大数量
#define	MBMAX_BITCOIL_RD	2000L		  //线圈连续读取的最大数量
#define	MBMAX_BITCOIL_WT	2000L		  //线圈连续写入的最大数量
#define	MBMAX_REGINP_RD		512L		  //输入寄存器连续读取的最大数量
#define	MBMAX_REGHOLD_RD	512L		  //保持寄存器连续读取的最大数量
#define	MBMAX_REGHOLD_WT	512L		  //保持寄存器连续写入的最大数量


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
//modbus主机命令结构体
typedef struct
{
	uint16_t slave;           //从机地址
	uint16_t func;	          //功能码			
	uint16_t addr;				    //首地址
	uint16_t cnt;							//待读取个数
	uint32_t startTimeout;    //通讯超时起始时间            
	uint32_t timeout;					//通讯超时时间间隔
	uint32_t startPeriod;     //通讯周期起始时间            
	uint32_t period;          //通讯周期间隔     
	uint16_t commCnt;         //该条命令的通讯次数，0xffff表示无限次数   
	uint16_t timeoutcnt;      //错误计数
}_CMD;

//modbus主机状态
typedef struct 
{
	uint16_t rep;																				
	uint8_t  PLCCOMFlag;	 	            
}_MTCMD;

#endif
