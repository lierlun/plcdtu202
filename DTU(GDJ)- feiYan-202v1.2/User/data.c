#include "data.h"
#include "param.h"
//modbus����
GPRS_PARA     GPRS_Para;
ICCID_SAVE		ICCID_Save;

void     RunBoot(void);
void     CpuReset(void);
uint16_t CmdAddr(uint8_t *pBuf);
uint16_t CmdValue(uint8_t *pBuf);
uint16_t CmdOnOffNum(uint8_t *pBuf);
/*
*********************************************************************************************************
*	�� �� ��: Init_SoftPara
*	����˵��: ��ʼ���������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Init_SoftPara(void)
{	
	uint16_t i;
	//�ڲ�RAM��ʼ������������ǰ���ʼ��
	my_mem_init(SRAMIN); 
	
	//װ�ز���
	LoadParam(0);
	
	//485�ӻ��豸����
	GPRS_Para.MaxSlaveCnt    = UART485_SLAVE_CNT;
	
	//��ʼ��485
	UART485_Init();
	for(i=270; i<462; i++)
	{
		g_PduUART485_bak[0].RegHold[i] = g_PduUART485[0].RegHold[i];
	}
	
}

/*
*********************************************************************************************************
*	�� �� ��: Para_Save_Check
*	����˵��: �����洢
*	��    ��: ��
*	�� �� ֵ: ��
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
				//�洢
				GPRS_Para.SaveFlag = 1;
			}
  }
	//ÿ5����һ���Ƿ��в�����Ҫ�洢��flash��
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
*	�� �� ��: Led_Task
*	����˵��: 
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: GPRS_Read
*	����˵��: modbus��ȡ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GPRS_Read(void)
{	
	
}
/*
*********************************************************************************************************
*	�� �� ��: GPRS_Write
*	����˵��: modbus�Ĵ���д
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GPRS_Write(void)
{
}


/*
*********************************************************************************************************
*	�� �� ��: UART485_Read
*	����˵��: modbus��ȡ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UART485_Read(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: UART485_Write
*	����˵��: modbus�Ĵ���д
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UART485_Write(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: CpuReset
*	����˵��: ִ�и�λ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//���庯��ָ������
typedef  void (*pFunction)(void);

void CpuReset(void)
{
#if uCOS_EN == 1 
	CPU_SR_ALLOC();
#endif
		//����һ������ָ��
		pFunction     Jump_To_Application;
		//��ת��ַ
		uint32_t      JumpAddress;
	
		//�ر�����	
		RCC_DeInit();		
		//�ر��ж�
		DISABLE_INT(); 
		//�����û����߳�ģʽ���������жϺ�ſ��Իص���Ȩ���߳�ģʽ
		__set_CONTROL(0);
	
		//��ת��ַ
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		//�û�Ӧ�õ�ַ
		Jump_To_Application = (pFunction) JumpAddress;
		//��ʼ��MSP
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		//��ת��Ӧ�ó���
		Jump_To_Application();	
		//���ж�
		ENABLE_INT();
}


/*
*********************************************************************************************************
*	�� �� ��: JsonTask
*	����˵��: json����������
*	��    ��: ��
*	�� �� ֵ: ��
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
			cJSON_AddItemToObject(item,"IA0",cJSON_CreateNumber(Ia[0].f));//����
			cJSON_AddItemToObject(item,"IB0",cJSON_CreateNumber(Ib[0].f));//����
			cJSON_AddItemToObject(item,"IC0",cJSON_CreateNumber(Ic[0].f));//����״̬
			cJSON_AddItemToObject(root,"params",item);					
			pBuf = cJSON_PrintUnformatted(root);					
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//�ͷ�cJSON_CreateObject����������ڴ�ռ�					
			
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
			cJSON_AddItemToObject(item,"IA1",cJSON_CreateNumber(Ia[1].f));//����
			cJSON_AddItemToObject(item,"IB1",cJSON_CreateNumber(Ib[1].f));//����
			cJSON_AddItemToObject(item,"IC1",cJSON_CreateNumber(Ic[1].f));//����״̬						
			cJSON_AddItemToObject(root,"params",item);					
			pBuf = cJSON_PrintUnformatted(root);				
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//�ͷ�cJSON_CreateObject����������ڴ�ռ�				

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
			cJSON_AddItemToObject(item,"IA2",cJSON_CreateNumber(Ia[2].f));//����
			cJSON_AddItemToObject(item,"IB2",cJSON_CreateNumber(Ib[2].f));//����
			cJSON_AddItemToObject(item,"IC2",cJSON_CreateNumber(Ic[2].f));//����״̬
			cJSON_AddItemToObject(root,"params",item);					
			pBuf = cJSON_PrintUnformatted(root);					
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//�ͷ�cJSON_CreateObject����������ڴ�ռ�				
			
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
			cJSON_AddItemToObject(item,"IA3",cJSON_CreateNumber(Ia[3].f));//����
			cJSON_AddItemToObject(item,"IB3",cJSON_CreateNumber(Ib[3].f));//����
			cJSON_AddItemToObject(item,"IC3",cJSON_CreateNumber(Ic[3].f));//����״̬
			cJSON_AddItemToObject(root,"params",item);					
			pBuf = cJSON_PrintUnformatted(root);					
			AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);						
			cJSON_Delete(root);//�ͷ�cJSON_CreateObject����������ڴ�ռ�				
			
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
			cJSON_Delete(root);//�ͷ�cJSON_CreateObject����������ڴ�ռ�				
			
			if(pBuf != NULL)
			{
				free(pBuf);
			}					
		}

}
