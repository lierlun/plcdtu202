#include "bsp_sim800c_rtos.h"
#include "malloc.h"
#include "GprsQueue.h"
#include "param.h"
#include "data.h"


/***************************************************************************/
/////////////////////////////////������ָ����о�///////////////////////////
//AT+CIICR                                  ����������·
//AT+CIFSR                                  ��ñ���IP��ַ

/////////////////////////////////����ָ�����///////////////////////////////
//AT+CPIN?                                  ���SIM��״̬
//AT+CSQ                                    ��������ź�ǿ��
//AT+CREG?                                  �������ע��״̬
//AT+CGATT?                                 ���GPRS����״̬
//AT+CSTT="CMNET"                           ����APN
//AT+CIPSTART="TCP","175.49.143.24","8899"  ����TCP����
//AT+CIPSEND                                �������ݵ�Զ�˷�����
//AT+CIPCLOSE                               �����ر����ӣ��ر�TCP��UDP���ӣ�
//AT+CIPSHUT                                �ر�PDP�����ģ��ر����Ӻ��ٷ��͸�ָ����������ӣ�


//+++                                       �����ݺ�PPPģʽ�л�������ģʽ��ָ��ǰ�����1s
//ATO                                       ������ģʽ�л�������ģʽ
//AT+CIPMODE=0��1                           0������ģʽ��1��͸��ģʽ
//AT+CPOWD=1                                �ػ�


////////////////////////////////////////////////�ײ����/////////////////////////////////////////////////
//����״̬��ⶨʱ��
OS_TMR 		   OS_TmrNetCheck; 
//GPRS������Ϣ����
OS_Q         Q_GPRS_Data;
//�������
NETWORK_PARA NetPara;

static void OS_TmrNetCheckCallback(void *p_tmr, void *p_arg);

void Init_SIM800C(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		//��ʼ��IO
		RCC_APB2PeriphClockCmd(RCC_SIM800C, ENABLE);

	  //�����������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    /* ��Ϊ����� */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     	/* IO������ٶ� */
	
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_KEY;
		GPIO_Init(GPIO_PORT_KEY, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_POWER;
		GPIO_Init(GPIO_PORT_POWER, &GPIO_InitStructure);
	
	  //������������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		/* ��Ϊ����� */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      /* IO������ٶ� */
		
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_STATE;
		GPIO_Init(GPIO_PORT_STATE, &GPIO_InitStructure);
	  
	  //�ڲ���������
  	memset((char*)&NetPara,0,sizeof(NETWORK_PARA));
	  //��ʼ��ѭ������
    GPRS_InitQueue(&g_GPRSQueue);
		
		//�򿪵�Դ
		POWER_OFF();
}

/*
*********************************************************************************************************
*	�� �� ��: Init_SIM800C_Soft
*	����˵��: ��ʼ�����ϵͳ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Init_SIM800C_Soft(void)
{
	uint8_t err;
	//������ʱ��
	OSTmrCreate((OS_TMR		*)&OS_TmrNetCheck,		            //��ʱ��
              (CPU_CHAR	*)"OS_TmrNetCheck",		            //��ʱ������
              (OS_TICK	 )5000,			                      //5�������
              (OS_TICK	 )1000,                           //����1��
              (OS_OPT		 )OS_OPT_TMR_PERIODIC,            //����ģʽ
              (OS_TMR_CALLBACK_PTR)OS_TmrNetCheckCallback,//��ʱ���ص�����
              (void	    *)0,					                    //����Ϊ0
              (OS_ERR	    *)&err);		                    //���صĴ�����	
	 OSTmrStart ((OS_TMR*)&OS_TmrNetCheck,(OS_ERR*)&err);		
  
  //����GPRS������Ϣ���У����г���Ϊ5
  OSQCreate((OS_Q*)&Q_GPRS_Data,(CPU_CHAR*)"Q_GPRS_Data",(OS_MSG_QTY)5,(OS_ERR*)&err);					
}
/*
*********************************************************************************************************
*	�� �� ��: SIM800C_HardPowerOn
*	����˵��: Ӳ��KEY����ģ�鿪��
*	��    ��: ��
*	�� �� ֵ: TRUE:�򿪳ɹ���FALSE����ʧ�ܣ�
*********************************************************************************************************
*/
uint8_t SIM800C_HardPowerOn(void)
{
	uint16_t cnt = 0;
	POWER_ON();
	BSP_OS_TimeDlyMs(300);
	//��ǰ�Ѿ�����
	if(STATE)
	{
		DBGU_Msg("GPRS power current ON...!\r\n");	
		return TRUE;
	}
	//��ǰ�ػ�����Ҫ��
	else
	{
		BSP_OS_TimeDlyMs(100);
		DBGU_Msg("GPRS power current OFF...!\r\n");
		KEY_ON();
		BSP_OS_TimeDlyMs(1000);
		KEY_OFF();
		//��������2������
		BSP_OS_TimeDlyMs(1000);
		while(!STATE)
		{
			BSP_OS_TimeDlyMs(100);
			cnt++;
			if(cnt > 300)
			{
				DBGU_Msg("GPRS power open Error...!\r\n");	
				KEY_ON();
				return FALSE;
			}
		}
		DBGU_Msg("GPRS power open OK...!\r\n");	
		KEY_ON();
		return TRUE;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_HardPowerOff
*	����˵��: Ӳ��KEY����ģ��ػ�
*	��    ��: ��
*	�� �� ֵ: TRUE:�رճɹ���FALSE���ر�ʧ�ܣ�
*********************************************************************************************************
*/
uint8_t SIM800C_HardPowerOff(void)
{
	uint16_t cnt = 0;
	uint8_t  ret = 0;
	POWER_OFF();
	BSP_OS_TimeDlyMs(500);
	//��ǰ�Ѿ��ػ�
	if(!STATE)
	{
		DBGU_Msg("GPRS power current OFF...!\r\n");	
		return TRUE;
	}
	//��ǰ��������Ҫ�ر�
	else
	{
		DBGU_Msg("GPRS power current ON...!\r\n");
// 		KEY_ON();
// 		BSP_OS_TimeDlyMs(1000);
// 		//����ͨ��������ػ�
// 		ret = Printf_AT(2,8000, "NORMAL POWER DOWN", "AT+CPOWD=1\r\n"); 
// 		if(ret == 1)
// 		{
// 			BSP_OS_TimeDlyMs(100);
// 			DBGU_Msg("GPRS Soft PowerDown OK!\r\n");
//     }
// 		//����رղ��ɹ�����ͨ��Ӳ���ػ�
// 		else
// 		{
// 			DBGU_Msg("GPRS Soft PowerDown Error!\r\n");
// 			KEY_OFF();
// 			//��������1����
// 			BSP_OS_TimeDlyMs(3000);
// 			KEY_ON();		
// 			while(STATE)
// 			{
// 				BSP_OS_TimeDlyMs(100);
// 				cnt++;
// 				if(cnt > 200)
// 				{
// 					DBGU_Msg("GPRS power close Error...!\r\n");
// 					return FALSE;
// 				}
// 			}
// 			DBGU_Msg("GPRS Hard PowerDown OK!\r\n");
//			BSP_OS_TimeDlyMs(100);
//    }
		
		DBGU_Msg("GPRS power close OK...!\r\n");
		
		//AIR208��Ҫ��ʱ
    BSP_OS_TimeDlyMs(3000);		
		return TRUE;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_SwitchCmdMode
*	����˵��: �л�������ģʽ
*	��    ��: ��
*	�� �� ֵ: 1:�ɹ���0:ʧ��
*********************************************************************************************************
*/
uint8_t SIM800C_SwitchCmdMode(void)
{
	uint8_t  ret = 0;
	//�л�ǰһ�����
	BSP_OS_TimeDlyMs(1500);
	SIM800C_SendStr("+++");//ģʽ�л�
	//�ȴ�Ӧ��
	ret = SIM800C_WaitATResponse("OK", 3000);
	//�л���һ�����
	BSP_OS_TimeDlyMs(800);

	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_SwitchDataMode
*	����˵��: �л�������ģʽ��͸��ģʽ
*	��    ��: ��
*	�� �� ֵ: 1:�ɹ���0:ʧ��
*********************************************************************************************************
*/
uint8_t SIM800C_SwitchDataMode(void)
{
	uint8_t  ret = 0;
	ret = Printf_AT(3,1000, "CONNECT","ATO\r\n");
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_Baud_Set
*	����˵��: ����SIM800C�����ʣ�ȷ��������Ϊ115200
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SIM800C_Baud_Set(void)
{
	uint32_t baud_table[6] = {115200,4800,9600,19200,38400,57600};
	uint8_t  i = 0;
	uint8_t  ret = 0;
	
	for(i=0;i<6;i++)
	{
		bsp_SetUartBaud(SIM800C_UART,baud_table[i]);
		BSP_OS_TimeDlyMs(100);
		ret = Printf_AT(3,1000, "OK","AT+IPR?\r\n");
		BSP_OS_TimeDlyMs(100);
		if(ret == 1)
		{
			DBGU_Msg("Current sim800c baud is : %d!\r\n",baud_table[i]);
			//�ָ�����Ĭ������
     	Printf_AT(3,1000, "OK", "AT&F\r\n"); 
			//����SIM800C������Ϊ115200
			Printf_AT(3,1000, "OK","AT+IPR=115200\r\n");
			BSP_OS_TimeDlyMs(100);
			bsp_SetUartBaud(SIM800C_UART,115200);
			DBGU_Msg("Set sim800c baud is : 115200");			
			break;
		}
	}
	//��ѯ�����󣬻�û��ѯ��������
	if(ret == 0)
	{
		DBGU_Msg("Current sim800c baud is error!\r\n");
	}
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_CMD_Init
*	����˵��: ���ͳ�ʼ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_CMD_Init(void)
{
	uint8_t cnt = 0;
	uint8_t ret = 0;
	uint8_t i = 0;
	
	NetPara.ReconnectTotal++;
	if(NetPara.ReconnectTotal > 5)
	{
		NetPara.ReconnectTotal = 0;
//		CpuReset();
		return 0;
  }
	
	//������������
	NetPara.Reconnect_cnt = 0;
	//����ȷ�ϲ�����
	SIM800C_Baud_Set();
	//�رջ���
	ret = SIM800C_Echo_Ctl(FALSE);
	if(ret != 1)
	{
		return 0;
  }
	
  //ȫ����ģʽ
	ret = Printf_AT(3,1000, "OK", "AT+CFUN=1\r\n"); 
	if(ret != 1)
	{
		return 0;
  }	
	{	
// 	  sprintf((char*)NetPara.NetType,"TCP");
// 		memcpy((char*)NetPara.IpAddr,"103.55.30.84",12);
//  	sprintf((char*)NetPara.IpPort,"%d",9990);		

	sprintf((char*)NetPara.NetType,"TCP");
	memcpy((char*)NetPara.IpAddr,"guolu.tslanpu.cn",16);	
	sprintf((char*)NetPara.IpPort,"%d",51019);	
//	sprintf((char*)NetPara.IpPort,"%d",60009);
	}
	//APN����
	if(GPRS_Para.apn_type == 0)
	{
		//�ƶ�
		sprintf((char*)NetPara.APN,"CMMTM");
		//��ͨ
//		sprintf((char*)NetPara.APN,"UNIM2M.NJM2MAPN");
	}

	//��ȡICCID��ͬʱҲ�Ǽ����SIM���Ƿ����
	ret = SIM800C_ReadICCID((uint8_t*)NetPara.ICCID,3,2000);
	if(ret == 1)
	{
// 		for(i=0;i<20;i++)
// 		{
// 			//���SIM��ICCID�����˱仯����λ�洢����
// 			if(GPRS_Para.ICCID[i] != Ascii2UINT8(NetPara.ICCID[i]))
// 			{
// 				LoadParam(1);
// 				break;
// 			}
// 		}
		//�洢��ǰICCID
		for(i=0;i<10;i++)
		{
			GPRS_Para.ICCID[i] = (uint8_t)(Ascii2UINT8(NetPara.ICCID[2*i])<<4)|(uint8_t)(Ascii2UINT8(NetPara.ICCID[2*i+1])<<0);
		}
  }
	else
	{
		return 0;		
  }

	//���SIM��
	ret = SIM800C_CheckPIN(5,5000);
	if(ret != 1)
	{
		return 0;
  }
	
	//��ģ���GPRS�������ʱ�����ϱ�"+CGREG: 1,0"
	Printf_AT(2,2000, "OK","AT+CGREG=1\r\n");		

	cnt = 0;
	do
	{
		//�������ע�����
		ret = Printf_AT(1,5000, "+CGREG: 1,1","AT+CGREG?\r\n");	
		if(ret != 1)
		{
			BSP_OS_TimeDlyMs(1000);
    }
		cnt++;
		if(cnt >= 30)
		{
			return 0;
    }
  }while(ret != 1);
	
 	//���ڶ�ȡ��վ��Ϣ
	Printf_AT(2,2000, "OK","AT+CREG=2\r\n");	
	SIM800C_ReadLacCi(&NetPara.Lac,&NetPara.Ci,2,2000);
#if SIM800C_TRANSPARENT
	/*****************͸��ģʽ***************/
	//��������ģʽΪ͸��ģʽ
	Printf_AT(2,2000, "OK", "AT+CIPMODE=1\r\n"); 
#else
	/*****************����ģʽ***************/	
	//��������ģʽΪ����ģʽ
	Printf_AT(2,2000, "OK", "AT+CIPMODE=0\r\n"); 
#endif
	//���������ƣ�����
  Printf_AT(2,2000, "OK", "AT+IFC=0,0\r\n");
#if SIM800C_TRANSPARENT

#else
	//���ý���������ʾIPͷ(�����ж�������Դ)	
	Printf_AT(2,2000, "OK","AT+CIPHEAD=1\r\n");
#endif	
	//�������
	Printf_AT(3,1000, "OK","AT&W\r\n");
  return 1;
}
/*
*********************************************************************************************************
*	�� �� ��: SIM800C_Check_PowerSta
*	����˵��: ����Դ״̬
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_Check_PowerSta(void)
{
	if(STATE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/*
*********************************************************************************************************
*	�� �� ��: SIM800C_WaitATResponse
*	����˵��: �ȴ�SIM800C����ָ����Ӧ���ַ���. ����ȴ� OK
*	��    ��: _pAckStr : Ӧ����ַ����� ���Ȳ��ó���255
*			 _usTimeOut : ����ִ�г�ʱ��0��ʾһֱ�ȴ�. >����ʾ��ʱʱ�䣬��λ1ms
*	�� �� ֵ: 1 ��ʾ�ɹ�  0 ��ʾʧ��
*********************************************************************************************************
*/
uint8_t SIM800C_WaitATResponse(char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t  ucData;
	uint16_t pos = 0;
	uint8_t  ret;
	uint8_t  *p;

	//����һ���ڴ�
	p = mymalloc(SRAMIN,256);
	//�����ʱ����������
	memset((char*)p,0,256);
	
	if (strlen(_pAckStr) > 255)
	{
		myfree(SRAMIN,p);
		return 0;
	}
	/* _usTimeOut == 0 ��ʾ���޵ȴ� */
	if (_usTimeOut > 0)
	{
		bsp_StartTimer(SIM800C_TMR_ID, _usTimeOut);	
	}
	
	while (1)
	{
    BSP_OS_TimeDlyMs(1);
		if (_usTimeOut > 0)
		{
			if (bsp_CheckTimer(SIM800C_TMR_ID))
			{
				ret = 0xff;	    /* ��ʱ */
#ifdef SIM800C_DBGU
			DBGU_Msg("AT CMD Timeout\r\n");	
#endif		
				break;
			}
		}
		
		//�ȴ���ʱ��֡
		if(bsp_CheckTimer(SIM800C_UART))
		{
			bsp_StopTimer(SIM800C_UART);
			
			//���ջ�����
			while(comGetChar(SIM800C_UART, &ucData))
			{
				p[pos++] = ucData;
				if(pos > 255)
				{
					ret = 0;	    /* ���� */
					break;					
				}
			}
			
			//��ת��Ϊ�ַ�������������
			sprintf((char*)p,"%s",(char*)p);
#ifdef SIM800C_DBGU
			//��ӡӦ����Ϣ
			DBGU_Msg("%s\r\n",p);	
#endif			
			
			if (strstr((char*)_pAckStr, "CONNECT OK") != NULL)
			{
				if (strstr((char*)p, "\r\nCONNECT\r\n") != NULL)
				{
					ret = 1;	/* �յ�ָ����Ӧ�����ݣ����سɹ� */
					break;
        }
      }
			
			//����״̬
			if (strstr((char*)_pAckStr, "+CREG: 0,1") != NULL)
			{
				if (strstr((char*)p, "+CREG: 0,5") != NULL)
				{
					ret = 1;	/* �յ�ָ����Ӧ�����ݣ����سɹ� */
					break;
        }
      }
			
			//ȷ���Ƿ������Ӧ���ַ���
			if (strstr((char*)p, _pAckStr) != NULL)
			{
				ret = 1;	/* �յ�ָ����Ӧ�����ݣ����سɹ� */
				break;
			}
			else
			{
				ret = 0;	    /* ���� */
				break;					
			}
		}
	}
	myfree(SRAMIN,p);
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_SendAT_WithAck
*	����˵��: ��GSMģ�鷢��AT��� �������Զ���AT�ַ���������<CR>�ַ�
*	��    ��: _Cmd:���������/r/n
*           _pAckStr��Ӧ���ַ�����������/r/n
*           _usTimeOut:�ȴ�Ӧ��ʱʱ�䣬��λms
*	�� �� ֵ: TRUE���ɹ���FALSE:ʧ�ܣ�
*********************************************************************************************************
*/
uint8_t SIM800C_SendAT_WithAck(char *_Cmd, char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t ret;
	//���FIFO
	comClearFifo(SIM800C_UART);
	//��������
	comSendBuf(SIM800C_UART, (uint8_t *)_Cmd, strlen(_Cmd));
	comSendBuf(SIM800C_UART, (uint8_t *)"\r\n", 2);
	//�ȴ�Ӧ��
	ret = SIM800C_WaitATResponse(_pAckStr, _usTimeOut);
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_SendAT_NoAck
*	����˵��: ��GSMģ�鷢��AT��� �������Զ���AT�ַ���������<CR>�ַ�
*	��    ��: _Cmd:���������/r/n
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SIM800C_SendAT_NoAck(char *_Cmd)
{
	//���FIFO
	comClearFifo(SIM800C_UART);
	//��������
	comSendBuf(SIM800C_UART, (uint8_t *)_Cmd, strlen(_Cmd));
	comSendBuf(SIM800C_UART, (uint8_t *)"\r\n", 2);
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_SendStr
*	����˵��: �����ַ���
*	��    ��: _Str:��Ҫ���͵��ַ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SIM800C_SendStr(char *_Str)
{
	//���FIFO
	comClearFifo(SIM800C_UART);
	//�����ַ���
	comSendBuf(SIM800C_UART, (uint8_t *)_Str, strlen(_Str));
}

/*
*********************************************************************************************************
*	�� �� ��: Printf_AT
*	����˵��: ��ʽ�������AT��������ȴ�Ӧ��
*	��    ��: _try:����ʱ�ط�����
*           _usTimeOut:��ʱʱ��
*           _pAckStr:Ӧ���ַ���
*           fmt:��ʽ�����ַ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t Printf_AT(uint8_t _try,uint16_t _usTimeOut,char* _pAckStr, char *fmt,...)
{
	__va_list v_list;
	uint8_t   ret;
	uint8_t   cnt = 0;
	uint8_t   *p;
	
	do
	{
		//����һ���ڴ�
		p = mymalloc(SRAMIN,128);
		memset((char*)p,0,128);
		
		//���FIFO
		comClearFifo(SIM800C_UART);	
		//�������ʽ�����ַ�������128,���������Ⱦ���
		if(strlen(fmt) > 128)
		{
			myfree(SRAMIN,p);
			return FALSE;
		}
		va_start(v_list, fmt);     
		vsprintf((char*)p, fmt, v_list); 
		va_end(v_list);	
#ifdef SIM800C_DBGU
		//��ӡ�����͵���Ϣ
		DBGU_Msg("%s\r\n",p);	
#endif		
		//��������
		comSendBuf(SIM800C_UART, p, strlen((char*)p));
		
		if (strstr((char*)_pAckStr, "CONNECT OK") != NULL)
		{
			BSP_OS_TimeDlyMs(2000);
    }
		//�ȴ�Ӧ��
		ret = SIM800C_WaitATResponse(_pAckStr, _usTimeOut);
		//�ͷ�
		myfree(SRAMIN,p);
    cnt++;		
	}
	while(((ret == 0)&&(cnt < _try))||((ret == 0xff)&&(cnt < _try)));

	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: Shut_Down_Phone
*	����˵��: �Ҷϵ绰
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t Shut_Down_Phone(void)
{
	//ATH�������Ӧ��ʱ����3��
	return Printf_AT(3,3000,"OK","ATH\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: Call_To_Phone
*	����˵��: ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t Call_To_Phone(uint8_t *number)
{
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_ConnectNetwork
*	����˵��: ��������
*	��    ��: net���������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_ConnectNetwork(NETWORK_PARA *net)
{
	uint8_t *p;
	uint8_t ret;
	uint8_t cnt = 0;

again:
#if SIM800C_TRANSPARENT
	if(net->Reconnect_cnt != 0)
	{
		//����ȷ��Ϊ����ģʽ
		SIM800C_SwitchCmdMode();
  }
#else
	
#endif

		//ÿ���������ӻ���ôκ���
	net->Reconnect_cnt++;
	if(net->Reconnect_cnt > 2)
	{
		return TIMEOUT;
  }
	
	Printf_AT(1,5000,"OK", "AT+CIPCLOSE=1\r\n");
	BSP_OS_TimeDlyMs(200);
	Printf_AT(1,5000,"SHUT OK", "AT+CIPSHUT\r\n");	
	
/***********************��ȡ�ź�ǿ��**************************/
	cnt = 0;
	do
	{
		
		SIM800C_ReadRSSI(&net->Rssi,2,2000);
		if(net->Rssi == 0)
		{
			BSP_OS_TimeDlyMs(1000);
    }
		cnt++;
		if(cnt >= 10)
		{
			goto again;
    }
  }while(net->Rssi == 0);

	//����PDP������,��������Э��,��������Ϣ
	ret = Printf_AT(3,2000, "OK","AT+CGDCONT=1,\"IP\",\"%s\"\r\n",net->APN);
	//����GPRSҵ��
	ret = Printf_AT(3,5000, "OK","AT+CGATT=1\r\n");	
	
	cnt = 0;
	do
	{
		//�������ע�����
		ret = Printf_AT(1,5000, "+CGATT: 1\r\n","AT+CGATT?\r\n");
		if(ret != 1)
		{
			BSP_OS_TimeDlyMs(1000);
    }
		cnt++;
		if(cnt >= 20)
		{
			goto again;
    }
  }while(ret != 1);
	
  //��ȡ����IP��ַ
	ret = Printf_AT(2,3000, "OK","AT+CIFSR\r\n");
	
	//����һ���ڴ�
	p = mymalloc(SRAMIN,256);
	//�ж��ڴ��Ƿ����ɹ�
	if(p == NULL)
	{
		DBGU_Msg("Connect Network Malloc Error...!\r\n");	
		myfree(SRAMIN,p);
		return 0;
	}
	memset((char*)p,0,256);
	sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"\r\n",net->NetType,net->IpAddr,net->IpPort);

  cnt = 0;
	do
	{		
		ret = Printf_AT(2,2000,"CONNECT OK",(char*)p);	
		cnt++;
		if(cnt >= 10)
		{
			//�ͷ��ڴ�
	    myfree(SRAMIN,p);
			goto again;
    }
  }while(((ret == 0)||(ret == 0xff)));	
	if(ret == TRUE)
	{
		//�����ܴ�������
		NetPara.ReconnectTotal = 0;
		//������������
		net->Reconnect_cnt = 0;
#if SIM800C_TRANSPARENT
			//��������ģʽ
	    SIM800C_SwitchCmdMode();
#endif		
		//��ȡ�ź�ǿ��
		SIM800C_ReadRSSI(&net->Rssi,2,2000);
#if SIM800C_TRANSPARENT
		 //��������ģʽ
	   SIM800C_SwitchDataMode();
#endif			
#if SIM800C_TRANSPARENT
		//͸������
		DBGU_Msg("Data Connect Network OK...TYPE=\"%s\",IP=\"%s\",PORT=\"%s\"!\r\n",net->NetType,net->IpAddr,net->IpPort);
#else
		//��������
		DBGU_Msg("Cmd Connect Network OK...TYPE=\"%s\",IP=\"%s\",PORT=\"%s\"!\r\n",net->NetType,net->IpAddr,net->IpPort);
#endif		
	}
	else
	{
		DBGU_Msg("Connect Network Error...!\r\n");	
	}
	//�ͷ��ڴ�
	myfree(SRAMIN,p);
	
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_CheckPIN
*	����˵��: ���PIN״̬
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_CheckPIN(uint8_t _try,uint16_t _usTimeOut)
{
	uint8_t ret = 0;
	uint8_t pos = 0;
	uint8_t cnt = 0;
	uint8_t *p;
	uint8_t  ucData;
	
	do
	{
		  BSP_OS_TimeDlyMs(1500);
			//����һ���ڴ�
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CPIN?");
			
			/* _usTimeOut == 0 ��ʾ���޵ȴ� */
			if (_usTimeOut > 0)
			{
				bsp_StartTimer(SIM800C_TMR_ID, _usTimeOut);	
			}
			
			while (1)
			{
				BSP_OS_TimeDlyMs(1);
				if (_usTimeOut > 0)
				{
					if (bsp_CheckTimer(SIM800C_TMR_ID))
					{
						DBGU_Msg("Check SIM card Timeout...!\r\n");	
						ret = 0;	    /* ��ʱ */
						break;
					}
				}
				
				//�ȴ���ʱ��֡
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//��ռ���
					pos = 0;
					//���ջ�����
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Check SIM recieve error...!\r\n");
							ret = 0;	    /* ���� */
							break;					
						}
					}
					
				  //��ת��Ϊ�ַ�������������
			    sprintf((char*)p,"%s",(char*)p);
					//��ӡӦ����Ϣ
					DBGU_Msg("%s\r\n",p);	
					
					//ȷ���Ƿ������Ӧ���ַ���
					if (strstr((char*)p, "+CPIN:") != NULL)
					{
						//���յ���Ӧ��
						//���ж�Ӧ������
						if (strstr((char*)p, "READY") != NULL)
						{
							DBGU_Msg("SIM card ready...!\r\n");	
							ret = 1;
							break;
						}
						else if (strstr((char*)p, "CME ERROR") != NULL)
						{
							DBGU_Msg("SIM card CME error...!\r\n");		
							ret = 0;
							break;
						}
						else if (strstr((char*)p, "SIM PIN2") != NULL)
						{
							DBGU_Msg("SIM card SIM PIN2...!\r\n");
							ret = 0;
							break;
						}
						else if (strstr((char*)p, "SIM PIN") != NULL)
						{
							DBGU_Msg("SIM card SIM PIN...!\r\n");	
							ret = 0;
							break;
						}
						else if (strstr((char*)p, "SIM PUK2") != NULL)
						{
							DBGU_Msg("SIM card SIM PUK2...!\r\n");	
							ret = 0;
							break;
						}
						else if (strstr((char*)p, "SIM PUK") != NULL)
						{
							DBGU_Msg("SIM card SIM PUK...!\r\n");	
							ret = 0;
							break;
						}
					}
					else
					{
						DBGU_Msg("No SIM card...!\r\n");	
						ret = 0;	    /* ���� */
						break;					
					}
				}
			}
			
			//�ͷŶ�̬�ڴ�
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_ReadIMEI
*	����˵��: ��ȡ���к�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_ReadIMEI(uint8_t *pbuf,uint8_t _try,uint16_t _usTimeOut)
{
	uint8_t ret = 0;
	uint8_t pos = 0;
	uint8_t cnt = 0;
	uint8_t *p;
	uint8_t  ucData;
	
	do
	{
			//����һ���ڴ�
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+GSN");
			
			/* _usTimeOut == 0 ��ʾ���޵ȴ� */
			if (_usTimeOut > 0)
			{
				bsp_StartTimer(SIM800C_TMR_ID, _usTimeOut);	
			}
			
			while (1)
			{
				BSP_OS_TimeDlyMs(1);
				if (_usTimeOut > 0)
				{
					if (bsp_CheckTimer(SIM800C_TMR_ID))
					{
						DBGU_Msg("Read IMEI timeout...!\r\n");	
						ret = 0;	    /* ��ʱ */
						break;
					}
				}
				
				//�ȴ���ʱ��֡
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//��ռ���
					pos = 0;
					//���ջ�����
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Read IMEI Error...!\r\n");
							ret = 0;	    /* ���� */
							break;					
						}
					}
					
				  //��ת��Ϊ�ַ�������������
			    sprintf((char*)p,"%s",(char*)p);
					//��ӡӦ����Ϣ
					DBGU_Msg("%s\r\n",p);	
					
					//ȷ���Ƿ������Ӧ���ַ���
					if (strstr((char*)p, "OK") != NULL)
					{
						//��ȡ�������к�
						while(*p++)
						{
							if((*p>= '0')&&(*p<= '9'))
							{
								*pbuf++ = *p;
							}
						}
						ret = 1;
						break;
					}
					else
					{
						DBGU_Msg("Read IMEI Error...!\r\n");
						ret = 0;	    /* ���� */
						break;					
					}
				}
			}
			
			//�ͷŶ�̬�ڴ�
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}


/*
*********************************************************************************************************
*	�� �� ��: SIM800C_ReadICCID
*	����˵��: ��ȡ���к�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_ReadICCID(uint8_t *pbuf,uint8_t _try,uint16_t _usTimeOut)
{
	uint8_t ret = 0;
	uint8_t pos = 0;
	uint8_t cnt = 0;
	uint8_t *p;
	uint8_t  ucData;
	
	do
	{
		  BSP_OS_TimeDlyMs(200);
			//����һ���ڴ�
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CCID");
			
			/* _usTimeOut == 0 ��ʾ���޵ȴ� */
			if (_usTimeOut > 0)
			{
				bsp_StartTimer(SIM800C_TMR_ID, _usTimeOut);	
			}
			
			while (1)
			{
				BSP_OS_TimeDlyMs(1);
				if (_usTimeOut > 0)
				{
					if (bsp_CheckTimer(SIM800C_TMR_ID))
					{
						DBGU_Msg("Read ICCID timeout...!\r\n");	
						ret = 0;	    /* ��ʱ */
						break;
					}
				}
				
				//�ȴ���ʱ��֡
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//��ռ���
					pos = 0;
					//���ջ�����
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Read ICCID Error...!\r\n");
							ret = 0;	    /* ���� */
							break;					
						}
					}
					
				  //��ת��Ϊ�ַ�������������
			    sprintf((char*)p,"%s",(char*)p);
					//��ӡӦ����Ϣ
					DBGU_Msg("%s\r\n",p);	
					
					//ȷ���Ƿ������Ӧ���ַ���
					if (strstr((char*)p, "OK") != NULL)
					{
						//��ȡ�������к�
						while(*p++)
						{
							if((*p>= '0')&&(*p<= '9'))
							{
								*pbuf++ = *p;
							}
							else if((*p>= 'a')&&(*p<= 'z'))
							{
								*pbuf++ = *p;
              }
						}
						ret = 1;
						break;
					}
					else
					{
						DBGU_Msg("Read ICCID Error...!\r\n");
						ret = 0;	    /* ���� */
						break;					
					}
				}
			}
			
			//�ͷŶ�̬�ڴ�
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_ReadRSSI
*	����˵��: ��ȡ�ź�ǿ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_ReadRSSI(int16_t *Rssi,uint8_t _try,uint16_t _usTimeOut)
{
	uint8_t ret = 0;
	uint8_t pos = 0;
	uint8_t cnt = 0;
	uint8_t i = 0;
	uint8_t *p;
	uint8_t  ucData;
	
	do
	{
			//����һ���ڴ�
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CSQ");
			
			/* _usTimeOut == 0 ��ʾ���޵ȴ� */
			if (_usTimeOut > 0)
			{
				bsp_StartTimer(SIM800C_TMR_ID, _usTimeOut);	
			}
			
			while (1)
			{
				BSP_OS_TimeDlyMs(1);
				if (_usTimeOut > 0)
				{
					if (bsp_CheckTimer(SIM800C_TMR_ID))
					{
						DBGU_Msg("Read RSSI timeout...!\r\n");	
						ret = 0;	    /* ��ʱ */
						break;
					}
				}
				
				//�ȴ���ʱ��֡
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//��ռ���
					pos = 0;
					//���ջ�����
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Read RSSI Error...!\r\n");
							ret = 0;	    /* ���� */
							break;					
						}
					}
					
				  //��ת��Ϊ�ַ�������������
			    sprintf((char*)p,"%s",(char*)p);
					//��ӡӦ����Ϣ
					DBGU_Msg("%s\r\n",p);	
					
					//ȷ���Ƿ������Ӧ���ַ���
					if (strstr((char*)p, "OK") != NULL)
					{
						p = (uint8_t*)strstr((char*)p,(char*)":");
						//ȥ���ֺ�
						p++;
						//ȥ���ո�
						p++;
						i = 0;
						ucData = 0;
						do
						{
							ucData = ucData*10;
							ucData += (p[i]-0x30);
							i++;
							if(i>15)
							{
								ret = 0;
								break;
              }
            }while((p[i] != ',')||(i>3));
						if(ucData <= 31)
						{
							//*Rssi = -115+4*ucData;
							*Rssi = ucData;
            }
						else
						{
							*Rssi = 0;
            }

						ret = 1;
						break;
					}
					else
					{
						DBGU_Msg("Read RSSI Error...!\r\n");
						ret = 0;	    /* ���� */
						break;					
					}
				}
			}
			
			//�ͷŶ�̬�ڴ�
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_ReadLacCi
*	����˵��: ��վ��λ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_ReadLacCi(uint16_t *Lac,uint16_t *Ci,uint8_t _try,uint16_t _usTimeOut)
{
	uint8_t ret = 0;
	uint8_t pos = 0;
	uint8_t cnt = 0;
	uint8_t *p;
	uint8_t  ucData;
	
	do
	{
			//����һ���ڴ�
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CREG?");
			
			/* _usTimeOut == 0 ��ʾ���޵ȴ� */
			if (_usTimeOut > 0)
			{
				bsp_StartTimer(SIM800C_TMR_ID, _usTimeOut);	
			}
			
			while (1)
			{
				BSP_OS_TimeDlyMs(1);
				if (_usTimeOut > 0)
				{
					if (bsp_CheckTimer(SIM800C_TMR_ID))
					{
						DBGU_Msg("Read timeout...!\r\n");	
						ret = 0;	    /* ��ʱ */
						break;
					}
				}
				
				//�ȴ���ʱ��֡
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//��ռ���
					pos = 0;
					//���ջ�����
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Read Error...!\r\n");
							ret = 0;	    /* ���� */
							break;					
						}
					}
					
				  //��ת��Ϊ�ַ�������������
			    sprintf((char*)p,"%s",(char*)p);
					//��ӡӦ����Ϣ
					DBGU_Msg("%s\r\n",p);	
					
					//ȷ���Ƿ������Ӧ���ַ���
					if (strstr((char*)p, "+CREG") != NULL)
					{
						p = (uint8_t*)strstr((char*)p,(char*)"\"");
						//���˫����
						p++;
						*Lac = CharToHex(*p++);
						*Lac <<= 4;
						*Lac |= CharToHex(*p++);
						*Lac <<= 4;
						*Lac |= CharToHex(*p++);
						*Lac <<= 4;
						*Lac |= CharToHex(*p++);
						//�Ҳ�˫����
						p++;
						p++;
						p++;
						*Ci = CharToHex(*p++);
						*Ci <<= 4;
						*Ci |= CharToHex(*p++);
						*Ci <<= 4;
						*Ci |= CharToHex(*p++);
						*Ci <<= 4;
						*Ci |= CharToHex(*p++);					
						ret = 1;
						break;
					}
					else
					{
						DBGU_Msg("Read Error...!\r\n");
						ret = 0;	    /* ���� */
						break;					
					}
				}
			}
			
			//�ͷŶ�̬�ڴ�
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_ATMode
*	����˵��: ����AT����ģʽ
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_ATMode(uint8_t sta)
{
	if(sta)
	{
		SIM800C_SendStr("+++");
  }
	else
	{
		Printf_AT(1,1000,"CONNECT", "ATO\r\n");
  }
	return TRUE;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_Echo_Ctl
*	����˵��: ���Կ���
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_Echo_Ctl(uint8_t flag)
{
	uint8_t ret = 0;
	if(flag)
	{
		ret = Printf_AT(3,1000,"OK","ATE1\r\n");
		if(ret != 1)
		{
			return 0;
    }
	}
	else
	{
		ret = Printf_AT(3,1000,"OK","ATE0\r\n");
		if(ret != 1)
		{
			return 0;
    }
	}
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_Check_NetSta
*	����˵��: �������״̬
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_Check_NetSta(uint8_t _try,uint16_t _usTimeOut)
{
	uint8_t ret = 0;
	uint8_t pos = 0;
	uint8_t cnt = 0;
	uint8_t *p;
	uint8_t  ucData;
	
	do
	{
			//����һ���ڴ�
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CIPSTATUS");
			
			/* _usTimeOut == 0 ��ʾ���޵ȴ� */
			if (_usTimeOut > 0)
			{
				bsp_StartTimer(SIM800C_TMR_ID, _usTimeOut);	
			}

			while (1)
			{
				BSP_OS_TimeDlyMs(1);
				if (_usTimeOut > 0)//�������_usTimeTimer ==0, �������޵ȴ�����ô��ʼbsp_checkTimer
				{
					if (bsp_CheckTimer(SIM800C_TMR_ID))
					{
						ret = 0;	    /* ��ʱ */
						break;
					}
				}
				
				
				if(bsp_CheckTimer(SIM800C_UART))//�ȴ���ʱ��֡
				{
					bsp_StopTimer(SIM800C_UART);
					//��ռ���
					pos = 0;
					//���ջ�����
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							ret = 0;	    /* ���� */
							break;					
						}
					}
					
				  //��ת��Ϊ�ַ�������������
			    sprintf((char*)p,"%s",(char*)p);
					
					//ȷ���Ƿ������Ӧ���ַ���
					if (strstr((char*)p, "IP INITIAL") != NULL)
					{
						ret = 0;
						break;
				  }
					else if (strstr((char*)p, "IP START") != NULL)
					{
						ret = 1;
						break;
          }
					else if (strstr((char*)p, "IP CONFIG") != NULL)
					{
						ret = 2;
						break;
          }
					else if (strstr((char*)p, "IP GPRSACT") != NULL)
					{	
						ret = 3;
						break;
          }
					else if (strstr((char*)p, "IP STATUS") != NULL)
					{
						ret = 4;
						break;
          }
					else if ((strstr((char*)p, "CONNECTING") != NULL)||
						       (strstr((char*)p, "LISTENING") != NULL))
					{
						ret = 5;
						break;
          }
					else if (strstr((char*)p, "CONNECT OK") != NULL)//�����󷵻�6
					{
						ret = 6;
						break;
          }
					else if (strstr((char*)p, "CLOSING") != NULL)
					{
						ret = 7;
						break;
          }
					else if (strstr((char*)p, "CLOSED") != NULL)
					{
						ret = 8;
						break;
          }
					else if (strstr((char*)p, "PDP DEACT") != NULL)
					{
						ret = 9;
						break;
          }
				}
			}
		//�ͷŶ�̬�ڴ�
		myfree(SRAMIN,p);
		cnt++;
	}while((ret == 0)&&(cnt < _try));		
			
	return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: SIM800C_Send
*	����˵��: ����ģʽ��������
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t SIM800C_Send(uint8_t *pBuf,uint16_t len)
{
	ElemType_G e;
	uint8_t    ret = 0;
#if SIM800C_TRANSPARENT
// 	//���FIFO
// 	comClearTxFifo(SIM800C_UART);	
// 		//�����ַ���
// 	comSendBuf(SIM800C_UART, pBuf, len);
// 	BSP_OS_TimeDlyMs(200);
	//���������
	e.len = len;
	memcpy(e.buf,pBuf,e.len);
	GPRS_EnQueue(&g_GPRSQueue, e);
#else
	ret = Printf_AT(2,500,">","AT+CIPSEND=%d\r\n",len);
	if(ret)
	{
			//���FIFO
		  comClearTxFifo(SIM800C_UART);	
			//�����ַ���
			comSendBuf(SIM800C_UART, pBuf, len);
	}
	else
	{
			//���FIFO
		  comClearTxFifo(SIM800C_UART);	
		  //ȡ������
		  comSendChar(SIM800C_UART, 0x1b);//ESC
	}
#endif
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_Recv_Len
*	����˵��: ������յ�����Ч���ݳ���
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//+IPD,16:http://en.usr.cn
uint16_t SIM800C_Recv_Len(uint8_t *pBuf)
{
	uint8_t  cnt = 0;
	uint8_t  *p;
	//�洢�����ַ�
	uint8_t  *pLen;
	uint16_t len = 0;
	
	//������̬�ڴ�
	pLen = mymalloc(SRAMIN,256);
	memset((char*)pLen,0,256);
	
	//������һ�����֡�,"��λ��
	p = (uint8_t*)strstr((char*)pBuf, ",");
	//��Ҫ','������ַ�
	p++;
	//������":"
	do
	{
		pLen[cnt++] = *p++;
	}while(*p != ':');
	//ת��Ϊ�ַ���
	sprintf((char*)pLen,"%s",(char*)pLen);
  
	len = atoi((char*)pLen);
	
	//�ͷŶ�̬�ڴ�
	myfree(SRAMIN,(char*)pLen);
	return len;
}


////////////////////////////////////////////////Ӧ�����/////////////////////////////////////////////////

/*
*********************************************************************************************************
*	�� �� ��: Send_HeatBeatMsg
*	����˵��: ����������Ϣ
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Send_HeatBeatMsg(uint32_t interval)
{
	static uint32_t start_time = 0;
	       uint8_t  sign = 0;
	       char     *pBuf;
	       char     *p;
	       uint16_t len = 0;
	sign = timing(start_time,interval);
	if(sign)
	{
		start_time = bsp_GetRunTime();
		
		//����һ�鶯̬�ڴ�
		pBuf = (char*)mymalloc(SRAMIN,256);
		p    = (char*)mymalloc(SRAMIN,256);
		//�ж��ڴ������Ƿ���ȷ
		if(pBuf == NULL)
		{
			myfree(SRAMIN,pBuf);
			return;
		}
		//��̬�ڴ�����
		memset((char*)pBuf,0,256);	
		memset((char*)p,0,256);
		//�������������
		GPRS_Para.heartbeat_sn++;
		
		//ת��Ϊ�ַ�����ICCID
		sprintf((char*)p,"%s",(char*)NetPara.ICCID);    
		//֡ͷ
		strcat((char*)pBuf,"glxxx,ICCID,");
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//���̱��
		sprintf((char*)p,"%d",77701);//plc��������
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//���������
		sprintf((char*)p,"%d",GPRS_Para.heartbeat_sn);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
// 		//��������
// 		sprintf((char*)p,"%d",GPRS_Para.heartbeat_period);
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,",");
		//�豸���߱�־
// 		sprintf((char*)p,"%d",GPRS_Para.OnlineFlag);
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,",");	
    //��ˮ�¶�
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[313]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
    //��ˮ�¶�
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[310]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//��Դ�¶�
		//sprintf((char*)p,"%d",g_PduUART485[0].RegHold[333]);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		//����״̬
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[338]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//����ģʽ
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[339]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//ˮλ����
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[345]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//���±���
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[346]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//�¶�̽ͷ�쳣״̬
		//sprintf((char*)p,"%d",GPRS_Para.TempProbeSta);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		//©�籨��״̬
		//sprintf((char*)p,"%d",GPRS_Para.PowerSta);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");	
		//ˮ��1����
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[347]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//ˮ��2����
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[348]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
    //ˮ��1����״̬
		//sprintf((char*)p,"%d",GPRS_Para.PumpRunSta[0]);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");	
		//ˮ��2����״̬
		//sprintf((char*)p,"%d",GPRS_Para.PumpRunSta[1]);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
			//ˮλ״̬
	//sprintf((char*)p,"%d",GPRS_Para.WaterLevelSta);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
	 //�ɼ���ѹֵ
		//sprintf((char*)p,"%.1f",(GPRS_Para.VoltVal/10.0));
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		//�ɼ�����ֵ
		//sprintf((char*)p,"%.1f",(GPRS_Para.CurVal/10.0));
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		
		//�����¶�
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[320]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
			//Ԥ��λ
		
		sprintf((char*)p,"%d",((g_PduUART485[0].BitCoil[0]>>10)&0x3f)|(g_PduUART485[0].BitCoil[1]<<6&0x03c0));
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		
		
// 		if(g_PduUART485[0].BitCoil[0]&BIT(11))			//M11
// 		{
// 			sprintf((char*)p,"%d",1);
//     }
// 		else
// 		{
// 			sprintf((char*)p,"%d",0);
//     }
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,"_");
// 		
// 		if(g_PduUART485[0].BitCoil[1]&BIT(0))			//M16
// 		{
// 			sprintf((char*)p,"%d",1);
//     }
// 		else
// 		{
// 			sprintf((char*)p,"%d",0);
//     }
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,"_");
// 		
// 		if(g_PduUART485[0].BitCoil[1]&BIT(1))			//M17
// 		{
// 			sprintf((char*)p,"%d",1);
//     }
// 		else
// 		{
// 			sprintf((char*)p,"%d",0);
//     }
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,"_");
// 		
// 		if(g_PduUART485[0].BitCoil[1]&BIT(2))			//M18
// 		{
// 			sprintf((char*)p,"%d",1);
//     }
// 		else
// 		{
// 			sprintf((char*)p,"%d",0);
//     }
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,"_");
// 		
// 		if(g_PduUART485[0].BitCoil[1]&BIT(3))			//M19
// 		{
// 			sprintf((char*)p,"%d",1);
//     }
// 		else
// 		{
// 			sprintf((char*)p,"%d",0);
//     }
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,",");
// 		
// 		sprintf((char*)p,"%d",0);//g_PduUART485[0].BitCoil[11]);
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,"_");
// 		
// 		sprintf((char*)p,"%d",0);//g_PduUART485[0].BitCoil[12]);
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,",");

    //�����¶�
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[318]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
			//����
		//sprintf((char*)p,"%.1f",(GPRS_Para.CurVal/10.0));
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		//Ŀ���¶�����
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[271]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
    //Ŀ���¶�����
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[270]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//��ǰ��λ
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[315]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
    //ת���¶�
		//sprintf((char*)p,"%d",g_PduUART485[0].RegHold[310]);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		
 //�����֡����
    len = strlen((char*)pBuf);
		//���֡����
		pBuf[2] = (len%1000/100)+0x30;
		pBuf[3] = (len%100/10)+0x30;
		pBuf[4] = (len%10/1)+0x30;
	
	
		//��������
		SIM800C_Send((uint8_t*)pBuf,strlen(pBuf));
		
		myfree(SRAMIN,pBuf);
		myfree(SRAMIN,p);
  }
}

/*
*********************************************************************************************************
*	�� �� ��: SIM800C_Task
*	����˵��: 
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SIM800C_Task(void)
{
	char          *pNew;	
	char          *pOld;
	uint16_t      len;
	uint8_t       ucData;
	uint8_t       ret;
	uint8_t       cnt = 0;
	
	Init_SIM800C();
reset:  
	//����ȵ�SIM��������
	do
	{
		SIM800C_HardPowerOff();
		BSP_OS_TimeDlyMs(200);
		SIM800C_HardPowerOn();
		BSP_OS_TimeDlyMs(1000);
		ret = SIM800C_CMD_Init();
	}while(ret == 0);
	//��������
	ret = SIM800C_ConnectNetwork(&NetPara);
	if(ret == 1)
	{
		//�¼ӿͻ�Ӧ��
		{
			GPRS_Para.gprs_rssi = NetPara.Rssi;
    }
		//��������
		NetPara.Connect_Sta = TRUE;
		//���̷�������
	  Send_HeatBeatMsg(1000);	
  }
	else if(ret == TIMEOUT)
	{
		//����ʧ��
		NetPara.Connect_Sta = FALSE;
		goto reset;
  }
	else
	{
		//����ʧ��
		NetPara.Connect_Sta = FALSE;
  }
	
	//��ʼĬ��ֵ
	NetPara.NetCheckTime = 2000;
	
	bsp_StartTimer(SIM800C_TMR_ID1, NetPara.NetCheckTime);//2�����ж�һ��sim800��Դ״̬
	
	NetPara.NetCheckTime1 = 30*1000;
	bsp_StartTimer(SIM800C_TMR_CHECK_NET, NetPara.NetCheckTime1);//30����һ������״̬
	
	
	while(1)
	{
		
		bsp_StartTimer(SIM800C_TMR_CHECK_RUN,5*60*1000);//�����ж�5���Ӷ�δִ�и�ѭ������ֱ�������豸

/*********************************��������������״̬**********************************/
		//����״̬���,�����м��
		if(bsp_CheckTimer(SIM800C_TMR_CHECK_NET))//3���Ӽ��һ������״̬
		{			
#if SIM800C_TRANSPARENT//��ͷ�ļ��к궨��Ϊ1
			//��������ģʽ
	    SIM800C_SwitchCmdMode();//������+++ģʽ�л�
#endif	
      	
			if(SIM800C_Check_NetSta(1,1000) == 6)//�ж������Ƿ�����	��ret == 6��Ϊconnect ok	
			{
				NetPara.NetCheckTime1 = 2*60*1000;
				NetPara.Connect_Sta   = TRUE;
#if SIM800C_TRANSPARENT
			 
			 SIM800C_SwitchDataMode();//��������ģʽ��͸��ģʽ
#endif		
      }
			else
			{
				NetPara.Connect_Sta = FALSE;
				//5������£���������
				bsp_StartTimer(SIM800C_TMR_ID1, 2000);
				NetPara.NetCheckTime1 = 30*1000;
      }
			
			bsp_StartTimer(SIM800C_TMR_CHECK_NET, NetPara.NetCheckTime1);//30��
		}
		
/*********************************��Դ��⣬ͬʱ�������״̬**********************************/    
    //��Դ״̬���		
		if(bsp_CheckTimer(SIM800C_TMR_ID1))//����Ϊ2��
		{
			bsp_StopTimer(SIM800C_TMR_ID1);
			//����Դ״̬
			ret = SIM800C_Check_PowerSta();//stm32��PB5�ţ����ӵ�sim800c��42�ţ�PowerState
			if(ret == FALSE)
			{
				//��������ģ��
				goto reset;
			}
			
			//�����������
			if(NetPara.Connect_Sta)//ȷ����������AT+CIPSTART���ͺ󣬷���connect ok
			{
				NetPara.NetCheckTime = 10000;//��ʼֵΪ2�룬�����ɹ���ĳ�10��
			}
			//�������ʧ��
			else
			{
				ret = SIM800C_ConnectNetwork(&NetPara);//��������
				if(ret == TRUE)
				{
					//��������
					NetPara.Connect_Sta = TRUE;
					//���̷�������
					Send_HeatBeatMsg(1000);	
					//�޸ļ��ʱ��
					NetPara.NetCheckTime = 10000;//������ʱ��ĳ�10��
				}
				else if(ret == TIMEOUT)
				{
					//����ʧ��
					NetPara.Connect_Sta = FALSE;
					goto reset;
        }
				else
				{
					//����ʧ��
					NetPara.Connect_Sta = FALSE;
					NetPara.NetCheckTime = 5000;//������ʱ��ĳ�5��
				}
      }
 			//�������������ⶨʱ��
 			bsp_StartTimer(SIM800C_TMR_ID1, NetPara.NetCheckTime);
 		}

		//�¼ӿͻ�Ӧ��
		{
			GPRS_Para.gprs_rssi = NetPara.Rssi;//�ź�ǿ�ȣ�SIM800C_ReadRSSI����
		}
		
		//��������������������Ϣ
		if(NetPara.Connect_Sta)
		{
				//������Ϣ���ͣ�ʱ�������
				Send_HeatBeatMsg(GPRS_Para.heartbeat_period*1000);
    }
		
		//��ʱ��֡���
		if(bsp_CheckTimer(SIM800C_UART))
		{
			bsp_StopTimer(SIM800C_UART);
			
			//����һ�鶯̬�ڴ�
      pOld = (char*)mymalloc(SRAMIN,256);
			//�ж��ڴ������Ƿ���ȷ
			if(pOld == NULL)
			{
				DBGU_Msg("Malloc Error!\r\n");	
				myfree(SRAMIN,pOld);
				break;
			}
			//��̬�ڴ�����
			memset((char*)pOld,0,256);

			//���ȼ�������
			len = 0;
			//���ջ�����
			while(comGetChar(SIM800C_UART, &ucData))
			{
				pOld[len++] = ucData;
				if(len >= 1024)
				{
					//������������ݳ��ȣ����ڷ�����ڴ滺����
					DBGU_Msg("Recieve Len Overflow!\r\n");
					myfree(SRAMIN,pOld);
					break;
				}
			}
			//ת��Ϊ�ַ���
			sprintf((char*)pOld,"%s",(char*)pOld);	
			
#if SIM800C_TRANSPARENT
			(void)pNew;
			
      if (strstr((char*)pOld, "\r\nCLOSED\r\n") != NULL)
			{
				//����Ͽ����ȴ���������
				//����ʧ��
				NetPara.Connect_Sta = FALSE;
			}	
			else  if (strstr((char*)pOld, "+CGREG: 1,0") != NULL)
			{
				//���總��ʧ�ܣ���������
				NetPara.Connect_Sta = FALSE;
      }
			else if ((strstr((char*)pOld, "\r\nCONNECT\r\n") != NULL)||
				       (strstr((char*)pOld, "CONNECT OK")      != NULL))
			{
				//��������
				NetPara.Connect_Sta = TRUE;
			}	
			else
			{
				  cnt = 0;
					//ȷ�ϵ�ַ
					if((GPRS_Para.ICCID[0] == pOld[cnt])  &&
						 (GPRS_Para.ICCID[1] == pOld[cnt+1])&&
						 (GPRS_Para.ICCID[2] == pOld[cnt+2])&&
						 (GPRS_Para.ICCID[3] == pOld[cnt+3])&&
						 (GPRS_Para.ICCID[4] == pOld[cnt+4])&&
						 (GPRS_Para.ICCID[5] == pOld[cnt+5])&&
						 (GPRS_Para.ICCID[6] == pOld[cnt+6])&&
						 (GPRS_Para.ICCID[7] == pOld[cnt+7])&&
						 (GPRS_Para.ICCID[8] == pOld[cnt+8])&&
						 (GPRS_Para.ICCID[9] == pOld[cnt+9]))
					{
						//ȥ��֡ͷ
						pOld += 10;
						len  -= 10;
						ret = GPRS_Slave((char*)pOld,len);
						if(ret == 0)
						{
							//ȷ�ϵ�ַ������
							if((pOld[0] >= 1)&&(pOld[0] <= 16))
							{
								//��ŵ������У��ȴ��·���485
                {
									T_ElemType e;
									e.len = len;
									memcpy(e.buf,pOld,e.len);
									T_EnQueue(&Trans_Queue, e);
                }							
              }
            }
					}
					else
					{
						
					}	
      }
#else	
			//�жϽ��յ����Ƿ�Ϊ����֡
			if (strstr((char*)pOld, "+IPD,") != NULL)
			{
					//������һ�����֡�:"��λ��
					pNew = strstr((char*)pOld, ":");
					//��Ҫð��
					pNew++;
				  
				  //modbus�ӻ�����
				  ret = GPRS_Slave((char*)pNew,SIM800C_Recv_Len((uint8_t*)pOld));				
			}
			//�ж��Ƿ�������
			else if (strstr((char*)pOld, "+CLIP:") != NULL)
			{
				//�Ҷϵ绰
				Shut_Down_Phone();
			}
			//�ж��Ƿ��ж���Ϣ
			else if (strstr((char*)pOld, "+CMTI:") != NULL)
			{
				//���ղ��������
			}	
			//�յ�����Ϣ��ʾ����Ͽ�
		  else if (strstr((char*)pOld, "CLOSED") != NULL)
			{
				//����Ͽ����ȴ���������
				//����ʧ��
				NetPara.Connect_Sta = FALSE;
			}	
#endif			
			//��ջ�����
			comClearRxFifo(SIM800C_UART);	
			myfree(SRAMIN,pOld);
		}			

#if SIM800C_TRANSPARENT		
		//ѭ�����Ͷ���
		{
			static ElemType_G e;
			//�����ж϶������Ƿ������Ч����
			if(!GPRS_IsQueueEmpty(&g_GPRSQueue))
			{
				//��ö�ͷԪ��
				GPRS_GetHead(&g_GPRSQueue,&e);
				//�����ַ���
				comSendBuf(SIM800C_UART, (uint8_t*)e.buf, e.len);
				BSP_OS_TimeDlyMs(200);	
				GPRS_DeQueue(&g_GPRSQueue,&e);						
			}
    }
#endif		
		BSP_OS_TimeDlyMs(1);
	}	 
}

/*
*********************************************************************************************************
*	�� �� ��: OS_TmrNetCheckCallback
*	����˵��: ����ϵͳ��ʱ���ص�����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OS_TmrNetCheckCallback(void *p_tmr, void *p_arg)			
{

}

