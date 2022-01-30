#include "luat_air208.h"
#include "modbus_gprs_iccid.h"
#include "NetMessage.h"
#include "GprsQueue.h"
#include "data.h"
#include "crc.h"
#include "filter.h"
#include "cJSON.h"

/*
*********************************************************************************************************
*	�� �� ��: AIR208_Init
*	����˵��: AIR208��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AIR208_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
		RCC_APB2PeriphClockCmd(RCC_ALL_AIR208, ENABLE);
	
	 //�����������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    /* ��Ϊ����� */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     	/* IO������ٶ� */
	
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_CTL;
		GPIO_Init(GPIO_PORT_CTL, &GPIO_InitStructure);	


		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_POWER;
		GPIO_Init(GPIO_PORT_POWER, &GPIO_InitStructure);
	
	  //������������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		/* ��Ϊ����� */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      /* IO������ٶ� */
		
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_STA;
		GPIO_Init(GPIO_PORT_STA, &GPIO_InitStructure);
		
		  
	  //�ڲ���������
//  	memset((char*)&GPRS_Para,0,sizeof(NETWORK_PARA));
	  //��ʼ��ѭ������
    GPRS_InitQueue(&g_GPRSQueue);
		
		//������Դ
		AIR208_POWER_ON;
		//���߿���ʹ��
		AIR208_CLT_ON;
}

/*
*********************************************************************************************************
*	�� �� ��: AIR208_InitPara
*	����˵��: AIR208��ʼ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AIR208_InitPara(void)
{
	//����״̬��ѯ
	bsp_StartTimer(NET_CHECK_TMR_ID, 10*TIME_SEC);
	GPRS_Para.NetDataSendFlag = 0;
	GPRS_Para.ImeiGetOKFlag = 0;
	GPRS_Para.NtpTimeOKFlag = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: AIR208_Send
*	����˵��: ����ģʽ��������
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t AIR208_Send(uint8_t *pBuf,uint16_t len,uint8_t addr)
{
	ElemType_G e;
	
	{
		//���������
		e.trycnt = TRY_CNT;
		e.addr   = addr; 
		e.len    = len;
		if(len > BUF_LEN)
		{
			return 0;
		}
		memset(e.buf,0,BUF_LEN);
		memcpy(e.buf,pBuf,e.len);
		GPRS_EnQueue(&g_GPRSQueue, e);
		return 1;
  }
}


/*
*********************************************************************************************************
*	�� �� ��: AIR208_Switch_Check
*	����˵��: AIR208���ؼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t AIR208_Switch_Check(void)
{
	if(AIR208_STA)
	{
		return 1;
  }
	else
	{
		return 0;
  }
}

/*
*********************************************************************************************************
*	�� �� ��: AIR208_Power_Ctl
*	����˵��: AIR208��Դ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t AIR208_Power_Ctl(uint8_t flag)
{
	if(flag)
	{
		//�򿪵�Դ
		AIR208_POWER_ON;
	}
	else
	{
		//�رյ�Դ
		AIR208_POWER_OFF;
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: AIR208_Switch_Ctl
*	����˵��: AIR208���ؿ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t AIR208_Switch_Ctl(uint8_t flag)
{
	static uint8_t ctl_flag = 0;
	if(flag)
	{
		//����
		if(AIR208_Switch_Check())
		{
			//�Ѿ�����
			ctl_flag = 0;
			return 1;
    }
		else
		{
			if(ctl_flag == 0)
			{
				//����2��
				AIR208_CLT_OFF;
				bsp_StartTimer(NET_POWER_TMR_ID, 2*TIME_SEC);
				ctl_flag = 1;
			}
			else if(ctl_flag == 1)
			{
				//��ʱ�䵽
				if(bsp_CheckTimer(NET_POWER_TMR_ID))
				{
					//����
					AIR208_CLT_ON;
					//�����ж�3�����Ƿ񿪻������δ��������Ҫ�ٴγ��Թػ��ٿ���
					bsp_StartTimer(NET_POWER_TMR_ID, 3*TIME_SEC);
					ctl_flag = 2;
        }			
			}
      else if(ctl_flag == 2)
			{
				if(bsp_CheckTimer(NET_POWER_TMR_ID))//�ȴ�3���ӵ�ʱ�䣬���AIR208_Switch_Check()Ϊ�٣���ر�208���ٴ�ѭ������
				{
					ctl_flag = 0;
        }
      }
			//ִ����
			return 0xff;
    }
  }
	else
	{
		//�ػ�
		if(AIR208_Switch_Check() == 0)
		{
			//�Ѿ��ػ�
			ctl_flag = 0;
			return 0;
    }
		else
		{
			if(ctl_flag == 0)
			{
				//����4��
				AIR208_CLT_OFF;
				bsp_StartTimer(NET_POWER_TMR_ID, 4*TIME_SEC);
				ctl_flag = 1;
			}
			else if(ctl_flag == 1)
			{
				//��ʱ�䵽
				if(bsp_CheckTimer(NET_POWER_TMR_ID))
				{
					//����
					AIR208_CLT_ON;
					//�����ж�3�����Ƿ񿪻������δ��������Ҫ�ٴγ��Կ���
					bsp_StartTimer(NET_POWER_TMR_ID, 3*TIME_SEC);
					ctl_flag = 2;
        }			
			}
      else if(ctl_flag == 2)
			{
				if(bsp_CheckTimer(NET_POWER_TMR_ID))
				{
					ctl_flag = 0;
        }
      }
			//ִ����
			return 0xff;
    }
  }
}

/*
*********************************************************************************************************
*	�� �� ��: ResolveLinkID
*	����˵��: ��������ID
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t ResolveLinkID(uint8_t *pBuf)
{
	char     *p;
	uint8_t  cnt = 0;
	char     Val[5];
	uint16_t temp = 0;
	
	if(strstr((char*)pBuf,"CONNECT LINK ID:") != NULL)
	{
		//ȥ��֡ͷ
		p = strstr((char*)pBuf,"CONNECT LINK ID:");
		p+=16;
  }
	else if(strstr((char*)pBuf,"CLOSE LINK ID:") != NULL)
	{
		//ȥ��֡ͷ
		p = strstr((char*)pBuf,"CLOSE LINK ID:");
		p+=14;
  }
	
	  cnt = 0;
	  memset(Val,0,5);
	  do
		{
			Val[cnt] = *p;
			cnt++;
			p++;
    }while((*p != ',')&&(cnt <= 5));	
		temp = atoi(Val);
		return temp;
} 

/*
*********************************************************************************************************
*	�� �� ��: ResolveCSQ
*	����˵��: ��������CSQ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t ResolveCSQ(uint8_t *pBuf)
{
	uint8_t  cnt = 0;
	char     Val[5];
	uint16_t temp = 0;
	char     *p;
	
		//ȥ��֡ͷ
		p = strstr((char*)pBuf,"rrpc,getcsq,");
		p+=12;
	
	  cnt = 0;
	  memset(Val,0,5);
	  do
		{
			Val[cnt] = *p;
			cnt++;
			p++;
    }while((*p != ',')&&(cnt <= 2));	
		temp = atoi(Val);
		return temp;
}

/*
*********************************************************************************************************
*	�� �� ��: ResolveIMEI
*	����˵��: ����IMEI
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ResolveIMEI_HEX(uint8_t *pBuf,uint8_t *pAim)
{
	char     *p;
	uint8_t  i = 0;

	//ȥ��֡ͷ
	p = strstr((char*)pBuf,"rrpc,getimei,");
	p+=13;
		
	//ֻ��15λ��Ч	
	for(i=0;i<7;i++)
	{
		pAim[i] = (uint8_t)(Ascii2UINT8(p[2*i])<<4)|(uint8_t)(Ascii2UINT8(p[2*i+1])<<0);
	}
	pAim[7] = (uint8_t)(Ascii2UINT8(p[14])<<4);	
}


/*
*********************************************************************************************************
*	�� �� ��: ResolveIMEIstr
*	����˵��: ����IMEI���ַ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ResolveIMEIstr(uint8_t *pBuf,uint8_t *pAim)
{
	char     *p;
	uint8_t  i = 0;

	//ȥ��֡ͷ
	p = strstr((char*)pBuf,"rrpc,getimei,");
	p+=13;
		
	//ֻ��15λ��Ч	
	for(i=0;i<15;i++)
	{
		pAim[i] = p[i];
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ResolveICCID
*	����˵��: ����ICCID
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ResolveICCID_HEX(uint8_t *pBuf,uint8_t *pAim)
{
	char     *p;
	uint8_t  i = 0;

	//ȥ��֡ͷ
	p = strstr((char*)pBuf,"rrpc,geticcid,");
	p+=14;
		
	//20λ��Ч
	for(i=0;i<10;i++)
	{
		pAim[i] = (uint8_t)(Ascii2UINT8(p[2*i])<<4)|(uint8_t)(Ascii2UINT8(p[2*i+1])<<0);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ResolveICCIDstr
*	����˵��: ����ICCIDΪ�ַ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ResolveICCIDstr(uint8_t *pBuf,uint8_t *pAim)
{
	char     *p;
	uint8_t  i = 0;

	//ȥ��֡ͷ
	p = strstr((char*)pBuf,"rrpc,geticcid,");
	p+=14;
		
	//20λ��Ч
	for(i=0;i<20;i++)
	{
		pAim[i] = p[i];
	}
}
/*
*********************************************************************************************************
*	�� �� ��: ResolveNtpTime
*	����˵��: ����ICCID����ʱ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//Ӧ���ʽ:rrpc,nettime,2019,12,12,22,53,31
uint8_t ResolveNtpTime(uint8_t *pBuf,_RTC *pAim)
{
	char     *p;
	uint8_t  cnt = 0;
	char     Val[5];
	
	//ȥ��֡ͷ
	p = strstr((char*)pBuf,"rrpc,nettime,");
	p+=13;
	
	//������
	cnt = 0;
	memset(Val,0,5);
	do
	{
		Val[cnt] = *p;
		cnt++;
		p++;
	}while((*p != ',')&&(cnt < 4));	
	pAim->Year = atoi(Val)%100;
	if(pAim->Year > 100)
	{
		return 0;
  }
	
	//ȥ��,
	p++;
	
	//������
	cnt = 0;
	memset(Val,0,5);
	do
	{
		Val[cnt] = *p;
		cnt++;
		p++;
	}while((*p != ',')&&(cnt < 2));	
	pAim->Mon = atoi(Val);
	if(pAim->Mon > 12)
	{
		return 0;
  }

	
	//ȥ��,
	p++;
	
	//������
	cnt = 0;
	memset(Val,0,5);
	do
	{
		Val[cnt] = *p;
		cnt++;
		p++;
	}while((*p != ',')&&(cnt < 2));	
	pAim->Day = atoi(Val);
	if(pAim->Day > 31)
	{
		return 0;
  }

	
	//ȥ��,
	p++;
	
	//����ʱ
	cnt = 0;
	memset(Val,0,5);
	do
	{
		Val[cnt] = *p;
		cnt++;
		p++;
	}while((*p != ',')&&(cnt < 2));	
	pAim->Hour = atoi(Val);
	if(pAim->Hour > 24)
	{
		return 0;
  }
	
	//ȥ��,
	p++;
	
	//������
	cnt = 0;
	memset(Val,0,5);
	do
	{
		Val[cnt] = *p;
		cnt++;
		p++;
	}while((*p != ',')&&(cnt < 2));	
	pAim->Min = atoi(Val);
	if(pAim->Min > 59)
	{
		return 0;
  }
	
	//ȥ��,
	p++;
	
	//������
	cnt = 0;
	memset(Val,0,5);
	do
	{
		Val[cnt] = *p;
		cnt++;
		p++;
	}while((*p != ',')&&(cnt < 2));	
	pAim->Sec = atoi(Val);
	if(pAim->Sec > 59)
	{
		return 0;
  }
	
	return 1;
}
/*
*********************************************************************************************************
*	�� �� ��: NetCheckSend
*	����˵��: ����״̬��ѯ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void NetCheckSend(void)
{
	char  SendBuf[] = {"rrpc,netstatus\r\n"};
	AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
}

/*
*********************************************************************************************************
*	�� �� ��: GetNtpTimeSend
*	����˵��: ��ȡ������ʱʱ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GetNtpTimeSend(void)
{
	char  SendBuf[] = {"rrpc,gettime\r\n"};
	AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
}

/*
*********************************************************************************************************
*	�� �� ��: GetIMEISend
*	����˵��: ��ȡIMEI��ֵ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GetIMEISend(void)
{
	char  SendBuf[] = {"rrpc,getimei\r\n"};
	if(GPRS_Para.ImeiGetOKFlag == 0)AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
}


/*
*********************************************************************************************************
*	�� �� ��: GetCsqSend
*	����˵��: ��ȡCSQ��ֵ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GetCsqSend(void)
{
	char  SendBuf[] = {"rrpc,getcsq\r\n"};
	AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
}

/*
*********************************************************************************************************
*	�� �� ��: FyIMEI
*	����˵��: ���ɷ���ƽ̨IMEI
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void FyIMEI(uint8_t *pAim, uint8_t array[])
{
	char *pBuf;
	pBuf = (char*)mymalloc(SRAMIN, 16);
	memset(pBuf, 0 ,16);
	//strcat((char*)array,"https://g.aliplus.com/ilop/d.html?locale=all&pk=a1DNlQwHT07&dn=");//ͨ��plc
	//strcat((char*)array,"https://g.aliplus.com/ilop/d.html?locale=all&pk=a1d6JgYInkz&dn=");//��������ȷ��
	strcat((char*)array,"a1DNlQwHT07&dn=");//�¹滮plcֻ���պ���30���ַ���ǰ���plc�̶�д��
	strncpy(pBuf, (const char*)pAim, 15);
	strcat((char*)array,(char*)pBuf);
	myfree(SRAMIN,(char*)pBuf);
}

/*
*********************************************************************************************************
*	�� �� ��: FeiyanIMEI
*	����˵��: ���ɷ���ƽ̨IMEI
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
char *FeiyanIMEI(char *sIMEI, char *dst)
{
	  char *p = sIMEI;
    char *q = dst;
		*(q+30) = '\0';//��dst�ַ������һλд�ɽ������
    strcat(q, "a1DNlQwHT07&dn=");
    strcat(q, p);
    return dst;
}

/*
*********************************************************************************************************
*	�� �� ��: GetICCIDSend
*	����˵��: ��ȡICCID��ֵ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GetICCIDSend(void)
{
	char  SendBuf[] = {"rrpc,geticcid\r\n"};
	if(GPRS_Para.IccidGetOKFlag == 0) AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
	//comSendBuf(UART208,(uint8_t*)SendBuf,strlen((char*)SendBuf));
}

/*
*********************************************************************************************************
*	�� �� ��: TransparentTask
*	����˵��: ͸������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t TransparentTask(void)
{
	static ElemType_G e;
	static uint8_t FirstStart = 0;
	static uint8_t Air208RunSta_bak = 0xff;
				 uint8_t LinkID = 0;
				 uint8_t i = 0;
				 uint8_t ret = 0;
//				 uint8_t cnt = 0;
//	static uint8_t paramflag = 0;

/*****************ֻ���״��ϵ�ʱ���ŷ���*******************/		
	if(FirstStart == 0)
	{
		if((GPRS_Para.Air208RunSta == 2)&&(GPRS_Para.ImeiGetOKFlag)&&(GPRS_Para.IccidGetOKFlag))
		{
			FirstStart = 1;
			//����һ��ע���������һ��������
//			Send_HeatBeatMsg();
		}	
	}
	
	
/*****************��ʱ����*******************************/	
	if((GPRS_Para.ImeiGetOKFlag)&&(GPRS_Para.IccidGetOKFlag))
	{
		if(Air208RunSta_bak != GPRS_Para.Air208RunSta)
		{
			Air208RunSta_bak = GPRS_Para.Air208RunSta;
			if(GPRS_Para.Air208RunSta == 2)//����,�豸�Ӷ������������������ʱ��ʼ
			{
				bsp_StartTimer(NET_SEND_TMR_ID, GPRS_Para.heartbeat_period*TIME_SEC);
			}
			else
			{
				bsp_StopTimer(NET_SEND_TMR_ID);
			}
		}

		//��������
		if(GPRS_Para.Air208RunSta == 2)
		{
			//ʱ�䵽����ʱ����
			if(bsp_CheckTimer(NET_SEND_TMR_ID))
			{
				bsp_StartTimer(NET_SEND_TMR_ID, GPRS_Para.heartbeat_period*TIME_SEC);
				GetCsqSend();
				Send_JsonHeatBeatMsg();
			}				
		}
	}
  else
	{
		Air208RunSta_bak = 0xff;
		bsp_StopTimer(NET_SEND_TMR_ID);
	}
		
/*****************����״̬��ѯ****************************/	
	if(bsp_CheckTimer(NET_CHECK_TMR_ID))
	{
		//��ȡһ��IMEI
		GetIMEISend();	
		GetICCIDSend();	
		
		if((GPRS_Para.Air208RunSta == 1)||(GPRS_Para.Air208RunSta == 0))
		{
			//������20���ѯһ��
			bsp_StartTimer(NET_CHECK_TMR_ID, 5*TIME_SEC);
			NetCheckSend();
			GetCsqSend();			
			//Ӧ��ʱ��ʱ��
			bsp_StartTimer(NET_TIMEOUT_TMR_ID, 3*TIME_SEC);
		}
		else if(GPRS_Para.Air208RunSta == 2)
		{
			//������120���ѯһ��
			bsp_StartTimer(NET_CHECK_TMR_ID, 10*TIME_SEC);
		}
	}
	//Ӧ��ʱ
	if(bsp_CheckTimer(NET_TIMEOUT_TMR_ID))
	{
		bsp_StopTimer(NET_TIMEOUT_TMR_ID);
		//����
		GPRS_Para.Air208RunSta = 1;	
	}
	
/*****************������ʱִ��****************************/	
	//����������Ž���������ʱ
	if(GPRS_Para.Air208RunSta == 2)
	{
		if(GPRS_Para.NtpTimeOKFlag == 0)
		{
			//δ��ʱ,10��󣬿�ʼ��ѯʱ��
			bsp_StartTimer(NTP_TIME_TMR_ID, 10*TIME_SEC);
			GPRS_Para.NtpTimeOKFlag = 1;
    }
		else if(GPRS_Para.NtpTimeOKFlag == 1)
		{
			if(bsp_CheckTimer(NTP_TIME_TMR_ID))
			{
				//��ȡ����ʱ��
				GetNtpTimeSend();
				GPRS_Para.NtpTimeOKFlag = 2;
				bsp_StartTimer(NTP_TIME_TMR_ID, 10*TIME_SEC);
			}
    }
		else if(GPRS_Para.NtpTimeOKFlag == 2)
		{
			//���У��ȴ���ʱ�ɹ�
			if(bsp_CheckTimer(NTP_TIME_TMR_ID))
			{
				bsp_StartTimer(NTP_TIME_TMR_ID, 10*TIME_SEC);
				GPRS_Para.NtpTimeOKFlag = 1;
      }
    }
		else if(GPRS_Para.NtpTimeOKFlag == 3)
		{
// 			//�Ѿ���ʱ�ɹ�,ÿ6Сʱ��ʱһ��
// 			bsp_StartTimer(NTP_TIME_TMR_ID, 6*TIME_HOUR);
// 			GPRS_Para.NtpTimeOKFlag = 1;
    }
  }
/********************************************************/


/**********************���ղ���***************************/	
	//COM1��208�ӿ�
	if(bsp_CheckTimer(UART208))
	{
		bsp_StopTimer(UART208);
		
			if(strstr((char*)UART208_RXBUF,"CONNECT LINK ID:") != NULL)
			{
				//����
				LinkID = ResolveLinkID(UART208_RXBUF);
				if((LinkID >= 1)&&(LinkID <= MAX_LINK))
				{
					GPRS_Para.link_flag[LinkID - 1] = 1;
				}
				
				//����
				GPRS_Para.Air208RunSta = 2;
								
			}
			else if(strstr((char*)UART208_RXBUF,"CLOSE LINK ID:") != NULL)
			{
				//����
				LinkID = ResolveLinkID(UART208_RXBUF);
				if((LinkID >= 1)&&(LinkID <= MAX_LINK))
				{
					GPRS_Para.link_flag[LinkID - 1] = 0;
			  }
	
					//ȫ�����Ŷ���
					for(i=0;i<MAX_LINK;i++)
					{
						if(GPRS_Para.link_flag[i])
						{
							break;
						}
					}
					if(i == MAX_LINK)
					{
						//����
						GPRS_Para.Air208RunSta = 1;
						//������20���ѯһ��
						bsp_StartTimer(NET_CHECK_TMR_ID, 20*TIME_SEC);
					}
					else
					{
						//����
						GPRS_Para.Air208RunSta = 2;
					}
      }
			else if (strstr((char*)UART208_RXBUF, "rrpc,netstatus") != NULL)
			{
				//Ӧ��
				bsp_StopTimer(NET_TIMEOUT_TMR_ID);
				if (strstr((char*)UART208_RXBUF, "rrpc,netstatus,OK") != NULL)
				{
					//����
					GPRS_Para.Air208RunSta = 2;					
				}
				else if (strstr((char*)UART208_RXBUF, "rrpc,netstatus,ERROR") != NULL)
				{
					//����
					GPRS_Para.Air208RunSta = 1;
					memset(GPRS_Para.link_flag,0,MAX_LINK);
				}
			}
			else if (strstr((char*)UART208_RXBUF, "rrpc,nettime") != NULL)
			{
				//������ʱʱ��
				if (strstr((char*)UART208_RXBUF, "rrpc,nettime,error") != NULL)
				{
					//��ʱ����
        }
				else
				{
					{
						_RTC rtc;
						ret = ResolveNtpTime(UART208_RXBUF,&rtc);
						if(ret == 1)
						{
//							rtc.Week = getweek(rtc.Year,rtc.Mon,rtc.Day);
							//ִ��ʱ��У׼
//							DS1302_SetTime(rtc);
							//������ʱ�ɹ�
							GPRS_Para.NtpTimeOKFlag = 3;
            }
          }
        }
      }
			else if (strstr((char*)UART208_RXBUF, "rrpc,getimei") != NULL)
			{
				if(UART208_tUART.usRxCount > 28)
				{
					memset(GPRS_Para.sIMEI,0,16);
					ResolveIMEIstr(UART208_RXBUF,GPRS_Para.sIMEI);
//					FyIMEI(GPRS_Para.sIMEI, GPRS_Para.FeiYanLink);
					FeiyanIMEI((char*)GPRS_Para.sIMEI, (char*)GPRS_Para.FeiYanLink);
					
					//���IMEI
					GPRS_Para.ImeiGetOKFlag = 1;
        }
      }
			else if (strstr((char*)UART208_RXBUF, "rrpc,geticcid") != NULL)
			{
				if(UART208_tUART.usRxCount > 34)
				{
					//����ICCIDΪhex
// 					memset(GPRS_Para.ICCID,0,10);
// 					ResolveICCID_HEX(UART208_RXBUF,GPRS_Para.ICCID);
					
					//����ICCIDΪstr
					memset(GPRS_Para.sICCID,0,21);
					ResolveICCIDstr(UART208_RXBUF,GPRS_Para.sICCID);
					
// 					GPRS_Para.ICCID[40] = '\0';					
// 					To_Hex((char*)GPRS_Para.sICCID, 20, (char*)GPRS_Para.ICCID);
					//���ICCID
					GPRS_Para.IccidGetOKFlag = 1;	
        }
      }
			
			else if (strstr((char*)UART208_RXBUF, "rrpc,getcsq") != NULL)
			{
				if (strstr((char*)UART208_RXBUF, "error") != NULL)
				{

        }
				else
				{
					GPRS_Para.CSQ = ResolveCSQ(UART208_RXBUF);
					
					if(GPRS_Para.IMEISendCntFlag == 0)
					{
						//��ʱ����һ��M�Ĵ����ϱ�
						GPRS_Para.ParamChangeFlag |= BIT(4);
          }
					GPRS_Para.IMEISendCntFlag++;
					if(GPRS_Para.IMEISendCntFlag >= 30)
					{
						GPRS_Para.IMEISendCntFlag = 0;
          }
        }
      }
			else if(strstr((char*)UART208_RXBUF, "service.property.set"))
			{
				  //����json
         Parse_208((const char*)UART208_RXBUF);
      }

		//��ս���
		comClearRxFifo(UART208);
  }

/**********************���Ͳ���***************************/	
		//ѭ�����Ͷ���
	  if(GPRS_Para.NetSendFlag == 0)
		{
			{
				//�����ж϶������Ƿ������Ч����
				if(!GPRS_IsQueueEmpty(&g_GPRSQueue))
				{
					//��ö�ͷԪ��
					//GPRS_GetHead(&g_GPRSQueue,&e);
					GPRS_DeQueue(&g_GPRSQueue,&e);
					
					//���ݷ���
			    comSendBuf(UART208,(uint8_t*)e.buf,e.len);//SIM800C_UART
					GPRS_Para.NetSendFlag = 1;
				}			
			}
    }
    else if(GPRS_Para.NetSendFlag == 1)
		{
			//����һ��200ms��ʱ������С����ʱ����
			bsp_StartTimer(NET_SEND_DELAY_TMR_ID, 800);//֮ǰ��200
			GPRS_Para.NetSendFlag = 2;
		}			
		else if(GPRS_Para.NetSendFlag == 2)
		{
			//�ж��Ƿ�Ӧ��ʱ
			if(bsp_CheckTimer(NET_SEND_DELAY_TMR_ID))
			{
				bsp_StopTimer(NET_SEND_DELAY_TMR_ID);
				GPRS_Para.NetSendFlag = 0;
      }		
		}
		
	return 0;

}

/*
*********************************************************************************************************
*	�� �� ��: AIR208_Task
*	����˵��: 208����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AIR208_Task(void)
{
	uint8_t ret = 0;

	//�ȴ򿪵�Դ
	if(GPRS_Para.Air208RunStep == 0)
	{
		AIR208_Power_Ctl(1);
		GPRS_Para.Air208RunStep = 1;
		bsp_StartTimer(NET_POWER_TMR_ID, 2*TIME_SEC);
	}
	//��ʱ2�����ж�
	else if(GPRS_Para.Air208RunStep == 1)
	{
		//��ʱ�䵽
		if(bsp_CheckTimer(NET_POWER_TMR_ID))
		{
			bsp_StopTimer(NET_POWER_TMR_ID);
			GPRS_Para.Air208RunStep = 2;
		}	
	}
	//����
	else if(GPRS_Para.Air208RunStep == 2)
	{
		ret = AIR208_Switch_Ctl(1);
		if(ret == 1)
		{
			GPRS_Para.Air208RunStep = 3;
			if(GPRS_Para.Air208RunSta == 0)
			{
				GPRS_Para.Air208RunSta = 1;
				//��ʼ���������
				AIR208_InitPara();
			}
		}	
	}
	//�����շ�����
	else if(GPRS_Para.Air208RunStep == 3)
	{
		//����Ѿ�����
		if((GPRS_Para.Air208RunSta)&&(AIR208_Switch_Check()))
		{
			TransparentTask();
		}
		//û����
		else
		{
			GPRS_Para.Air208RunStep = 4;
		}
	}
	//ִ�йرյ�Դ
	else if(GPRS_Para.Air208RunStep == 4)
	{
		AIR208_Power_Ctl(0);
		bsp_StartTimer(NET_POWER_TMR_ID, 2*TIME_SEC);
		GPRS_Para.Air208RunStep = 5;	
	}
	//��ʱ�����ڴ򿪵�Դ����
	else
	{
		//��ʱ�䵽
		if(bsp_CheckTimer(NET_POWER_TMR_ID))
		{
			bsp_StopTimer(NET_POWER_TMR_ID);
			GPRS_Para.Air208RunStep = 0;
		}	
	}
}


/*
*********************************************************************************************************
*	�� �� ��: AliEnQueue
*	����˵��: ��ģ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AliEnQueue(uint16_t Func, uint16_t Address)
{
	  ElemType e;
		e.slave        = 1;
		e.func         = Func;
		e.addr         = Address;	 	                    //���ּĴ������׵�ַ����ʾ������
		e.cnt          = 1;                            //���ݴӻ�����ȷ��д��ļĴ�������
		e.startTimeout = bsp_GetRunTime();
		e.timeout      = 1000;	                        //���� ��ʱʱ��
		e.startPeriod  = bsp_GetRunTime();
		e.period       = 5*1000;	                      //���������
		e.commCnt      = PERIOD_CNT;                    //����ɾ������
		e.timeoutcnt   = 0;		                          //�����������
		
		T_EnQueue(&Trans_Queue, e);

}

/*
*********************************************************************************************************
*	�� �� ��: Parse_208
*	����˵��: ����208��������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Parse_208(const char *JSON)
{
	uint16_t i;
	char  pBuf[6] = {0};
	char  str[4]  = {0};
	cJSON *json = NULL, *cjson_params = NULL;
	cJSON *cjson_params_d460 = NULL;
	cJSON *cjson_params_d461 = NULL;
	cJSON *cjson_params_d506 = NULL;
	cJSON *cjson_params_d507 = NULL;
	cJSON *cjson_params_d546 = NULL;
	cJSON *cjson_params_d547 = NULL;
	cJSON *cjson_params_d    = NULL;
	
	if(UART208_tUART.usRxCount > 68)
	{
		json = cJSON_Parse(JSON);//�������ڵ�json
		if (json)
		{
			cjson_params = cJSON_GetObjectItem(json,"params");
				
			if(cjson_params!=NULL)
			{	
					//M�Ĵ���
				for(i=500; i<528; i++)
				{
					strcat(pBuf, "m");
					sprintf(str, "%d", i);
					strcat(pBuf, str);
					cjson_params_d = cJSON_GetObjectItem(cjson_params, pBuf);
					memset(pBuf,NULL,sizeof(pBuf));
					if(cjson_params_d != NULL)
					{
						if(cjson_params_d->valueint)
						{
							g_PduGprs.BitCoil[i/16] |= BIT(i%16);
						}
						else
						{
							g_PduGprs.BitCoil[i/16] &= ~BIT(i%16);
						}						
						AliEnQueue(MB_BITCOIL_WT, i);
						break;
					}		
				}
				//D�Ĵ���
				for(i=323; i<454; i++)
				{
					strcat(pBuf, "d");
					sprintf(str, "%d", i);
					strcat(pBuf, str);
					cjson_params_d = cJSON_GetObjectItem(cjson_params,pBuf);
					memset(pBuf,NULL,sizeof(pBuf));
					if(cjson_params_d != NULL)
					{
						g_PduGprs.RegHold[i] = cjson_params_d->valueint;	
						AliEnQueue(MB_REGHOLD_WT, i);
						break;
					}		
				}
				cjson_params_d460 = cJSON_GetObjectItem(cjson_params,"d460");
				if(cjson_params_d460 != NULL)
				{
					g_PduGprs.RegHold[460] = cjson_params_d460->valueint;
					AliEnQueue(MB_REGHOLD_WT, 460);					
				}	
				cjson_params_d461 = cJSON_GetObjectItem(cjson_params,"d461");
				if(cjson_params_d461 != NULL)
				{
					g_PduGprs.RegHold[461] = cjson_params_d461->valueint;
					AliEnQueue(MB_REGHOLD_WT, 461);					
				}
				cjson_params_d506 = cJSON_GetObjectItem(cjson_params,"d506");
				if(cjson_params_d506 != NULL)
				{
					g_PduGprs.RegHold[506] = cjson_params_d506->valueint * 10;
					AliEnQueue(MB_REGHOLD_WT, 506);					
				}
				cjson_params_d507 = cJSON_GetObjectItem(cjson_params,"d507");
				if(cjson_params_d507 != NULL)
				{
					g_PduGprs.RegHold[507] = cjson_params_d507->valueint * 10;
					AliEnQueue(MB_REGHOLD_WT, 507);					
				}
				cjson_params_d546 = cJSON_GetObjectItem(cjson_params,"d546");
				if(cjson_params_d546 != NULL)
				{
					g_PduGprs.RegHold[546] = cjson_params_d546->valueint * 10;
					AliEnQueue(MB_REGHOLD_WT, 546);					
				}
				cjson_params_d547 = cJSON_GetObjectItem(cjson_params,"d547");
				if(cjson_params_d547 != NULL)
				{
					g_PduGprs.RegHold[547] = cjson_params_d547->valueint * 10;
					AliEnQueue(MB_REGHOLD_WT, 547);					
				}			
				
			}
			cJSON_Delete(json); // �ͷ��ڴ�
	}
	}
}
