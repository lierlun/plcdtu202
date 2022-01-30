#include "data.h"
#include "param.h"
//modbus参数
GPRS_PARA     GPRS_Para;
ICCID_SAVE		ICCID_Save;

void     RunBoot(void);
void     CpuReset(void);
uint16_t CmdAddr(uint8_t *pBuf);
uint16_t CmdValue(uint8_t *pBuf);
uint16_t CmdOnOffNum(uint8_t *pBuf);
/*
*********************************************************************************************************
*	函 数 名: Init_SoftPara
*	功能说明: 初始化软件参数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Init_SoftPara(void)
{	
	uint16_t i;
	//内部RAM初始化，必须在最前面初始化
	my_mem_init(SRAMIN); 
	
	//装载参数
	LoadParam(0);
	
	//485从机设备个数
	GPRS_Para.MaxSlaveCnt    = UART485_SLAVE_CNT;
	
	//初始化485
	UART485_Init();
	for(i=270; i<462; i++)
	{
		g_PduUART485_bak[0].RegHold[i] = g_PduUART485[0].RegHold[i];
	}
	
}

/*
*********************************************************************************************************
*	函 数 名: Para_Save_Check
*	功能说明: 参数存储
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Para_Save_Check(void)
{
	static uint32_t start_time = 0;
	       uint8_t  sign = 0;
	if((g_tParam.GPRS_Para.heartbeat_period    			!= GPRS_Para.heartbeat_period))
	{
			if(GPRS_Para.SaveFlag == 0)
			{
				//存储
				GPRS_Para.SaveFlag = 1;
			}
  }
	//每5秒检测一下是否有参数需要存储到flash中
	sign = timing(start_time,5000);
	if(sign)
	{
		start_time = bsp_GetRunTime();
		if(GPRS_Para.SaveFlag)
		{
			SaveParam();
			GPRS_Para.SaveFlag = 0;
		}		
	}
}
/*
*********************************************************************************************************
*	函 数 名: Led_Task
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Led_Task(void)
{
	static uint32_t start_time = 0;
	uint8_t  sign       = 0;
	if(GPRS_Para.Air208RunSta == 2)
	{
		sign = timing(start_time,1000);
	}
	else
	{
		sign = timing(start_time,150);
	}
	if(sign)
	{
		start_time = bsp_GetRunTime();
		bsp_LedToggle(1);
	}
	
}
/*
*********************************************************************************************************
*	函 数 名: GPRS_Read
*	功能说明: modbus读取
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void GPRS_Read(void)
{	
	
}
/*
*********************************************************************************************************
*	函 数 名: GPRS_Write
*	功能说明: modbus寄存器写
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void GPRS_Write(void)
{
}


/*
*********************************************************************************************************
*	函 数 名: UART485_Read
*	功能说明: modbus读取
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void UART485_Read(void)
{
}

/*
*********************************************************************************************************
*	函 数 名: UART485_Write
*	功能说明: modbus寄存器写
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void UART485_Write(void)
{
}

/*
*********************************************************************************************************
*	函 数 名: CpuReset
*	功能说明: 执行复位操作
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
//定义函数指针类型
typedef  void (*pFunction)(void);

void CpuReset(void)
{
#if uCOS_EN == 1 
	CPU_SR_ALLOC();
#endif
		//定义一个函数指针
		pFunction     Jump_To_Application;
		//跳转地址
		uint32_t      JumpAddress;
	
		//关闭外设	
		RCC_DeInit();		
		//关闭中断
		DISABLE_INT(); 
		//进入用户级线程模式，进入软中断后才可以回到特权级线程模式
		__set_CONTROL(0);
	
		//跳转地址
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		//用户应用地址
		Jump_To_Application = (pFunction) JumpAddress;
		//初始化MSP
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		//跳转到应用程序
		Jump_To_Application();	
		//打开中断
		ENABLE_INT();
}


/*
*********************************************************************************************************
*	函 数 名: JsonTask
*	功能说明: json串发送任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void JsonTask(void)
{
  char  *pBuf = NULL;
	cJSON *root, *item; 
	uint8_t i;
	typedef union
	{
		float f;
		uint16_t RegHold[2];
	}union_t;
	union_t Ia[4]={0},Ib[4]={0},Ic[4]={0};
	
	for(i=0; i<4; i++)
	{
			Ia[i].RegHold[1] = g_PduUART485[i].RegHold[69];
			Ia[i].RegHold[0] = g_PduUART485[i].RegHold[70];
			Ib[i].RegHold[1] = g_PduUART485[i].RegHold[71];
			Ib[i].RegHold[0] = g_PduUART485[i].RegHold[72];
			Ic[i].RegHold[1] = g_PduUART485[i].RegHold[73];
			Ic[i].RegHold[0] = g_PduUART485[i].RegHold[74];
  }

		if(GPRS_Para.ParamChangeFlag & BIT(0))
		{
			GPRS_Para.ParamChangeFlag &=~BIT(0);
			root = cJSON_CreateObject();
			item = cJSON_CreateObject();
			cJSON_AddItemToObject(item,"IA0",cJSON_CreateNumber(Ia[0].f));//下限
			cJSON_AddItemToObject(item,"IB0",cJSON_CreateNumber(Ib[0].f));//上限
			cJSON_AddItemToObject(item,"IC0",cJSON_CreateNumber(Ic[0].f));//开机状态
			cJSON_AddItemToObject(root,"params",item);					
			pBuf = cJSON_PrintUnformatted(root);					
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//释放cJSON_CreateObject分配出来的内存空间					
			
			if(pBuf != NULL)
			{
				free(pBuf);
			}
		}
		else if(GPRS_Para.ParamChangeFlag & BIT(1))
		{
			GPRS_Para.ParamChangeFlag &=~BIT(1);
			root = cJSON_CreateObject();					
			item = cJSON_CreateObject();
			cJSON_AddItemToObject(item,"IA1",cJSON_CreateNumber(Ia[1].f));//下限
			cJSON_AddItemToObject(item,"IB1",cJSON_CreateNumber(Ib[1].f));//上限
			cJSON_AddItemToObject(item,"IC1",cJSON_CreateNumber(Ic[1].f));//开机状态						
			cJSON_AddItemToObject(root,"params",item);					
			pBuf = cJSON_PrintUnformatted(root);				
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//释放cJSON_CreateObject分配出来的内存空间				

			if(pBuf != NULL)
			{
				free(pBuf);
			}
		}
		else if(GPRS_Para.ParamChangeFlag & BIT(2))
		{
			GPRS_Para.ParamChangeFlag &=~BIT(2);
			root = cJSON_CreateObject();	
			item = cJSON_CreateObject();
			cJSON_AddItemToObject(item,"IA2",cJSON_CreateNumber(Ia[2].f));//下限
			cJSON_AddItemToObject(item,"IB2",cJSON_CreateNumber(Ib[2].f));//上限
			cJSON_AddItemToObject(item,"IC2",cJSON_CreateNumber(Ic[2].f));//开机状态
			cJSON_AddItemToObject(root,"params",item);					
			pBuf = cJSON_PrintUnformatted(root);					
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//释放cJSON_CreateObject分配出来的内存空间				
			
			if(pBuf != NULL)
			{
				free(pBuf);
			}					
		}
		else if(GPRS_Para.ParamChangeFlag & BIT(3))
		{
			GPRS_Para.ParamChangeFlag &=~BIT(3);
			root = cJSON_CreateObject();					
			item = cJSON_CreateObject();
			cJSON_AddItemToObject(item,"IA3",cJSON_CreateNumber(Ia[3].f));//下限
			cJSON_AddItemToObject(item,"IB3",cJSON_CreateNumber(Ib[3].f));//上限
			cJSON_AddItemToObject(item,"IC3",cJSON_CreateNumber(Ic[3].f));//开机状态
			cJSON_AddItemToObject(root,"params",item);					
			pBuf = cJSON_PrintUnformatted(root);					
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//释放cJSON_CreateObject分配出来的内存空间				
			
			if(pBuf != NULL)
			{
				free(pBuf);
			}					
		}
		else if(GPRS_Para.ParamChangeFlag & BIT(4))
		{
			GPRS_Para.ParamChangeFlag &=~BIT(4);
			root = cJSON_CreateObject();					
			item = cJSON_CreateObject();
			cJSON_AddItemToObject(item,"CSQ",cJSON_CreateNumber(GPRS_Para.CSQ));//csq
			cJSON_AddItemToObject(item,"ICCID",cJSON_CreateString((char*)GPRS_Para.sICCID));
			cJSON_AddItemToObject(item,"IMEI",cJSON_CreateString((char*)GPRS_Para.sIMEI));
			cJSON_AddItemToObject(root,"params",item);				
			pBuf = cJSON_PrintUnformatted(root);					
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//释放cJSON_CreateObject分配出来的内存空间				
			
			if(pBuf != NULL)
			{
				free(pBuf);
			}					
		}

}
