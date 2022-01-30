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
*	函 数 名: AIR208_Init
*	功能说明: AIR208初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AIR208_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
		RCC_APB2PeriphClockCmd(RCC_ALL_AIR208, ENABLE);
	
	 //配置输出引脚
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    /* 设为输出口 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     	/* IO口最大速度 */
	
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_CTL;
		GPIO_Init(GPIO_PORT_CTL, &GPIO_InitStructure);	


		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_POWER;
		GPIO_Init(GPIO_PORT_POWER, &GPIO_InitStructure);
	
	  //配置输入引脚
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		/* 设为输入口 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      /* IO口最大速度 */
		
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_STA;
		GPIO_Init(GPIO_PORT_STA, &GPIO_InitStructure);
		
		  
	  //内部变量清零
//  	memset((char*)&GPRS_Para,0,sizeof(NETWORK_PARA));
	  //初始化循环队列
    GPRS_InitQueue(&g_GPRSQueue);
		
		//给定电源
		AIR208_POWER_ON;
		//拉高开关使能
		AIR208_CLT_ON;
}

/*
*********************************************************************************************************
*	函 数 名: AIR208_InitPara
*	功能说明: AIR208初始化参数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AIR208_InitPara(void)
{
	//网络状态查询
	bsp_StartTimer(NET_CHECK_TMR_ID, 10*TIME_SEC);
	GPRS_Para.NetDataSendFlag = 0;
	GPRS_Para.ImeiGetOKFlag = 0;
	GPRS_Para.NtpTimeOKFlag = 0;
}

/*
*********************************************************************************************************
*	函 数 名: AIR208_Send
*	功能说明: 命令模式发送数据
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t AIR208_Send(uint8_t *pBuf,uint16_t len,uint8_t addr)
{
	ElemType_G e;
	
	{
		//加入队列中
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
*	函 数 名: AIR208_Switch_Check
*	功能说明: AIR208开关检查
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: AIR208_Power_Ctl
*	功能说明: AIR208电源控制
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t AIR208_Power_Ctl(uint8_t flag)
{
	if(flag)
	{
		//打开电源
		AIR208_POWER_ON;
	}
	else
	{
		//关闭电源
		AIR208_POWER_OFF;
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: AIR208_Switch_Ctl
*	功能说明: AIR208开关控制
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t AIR208_Switch_Ctl(uint8_t flag)
{
	static uint8_t ctl_flag = 0;
	if(flag)
	{
		//开机
		if(AIR208_Switch_Check())
		{
			//已经开机
			ctl_flag = 0;
			return 1;
    }
		else
		{
			if(ctl_flag == 0)
			{
				//拉低2秒
				AIR208_CLT_OFF;
				bsp_StartTimer(NET_POWER_TMR_ID, 2*TIME_SEC);
				ctl_flag = 1;
			}
			else if(ctl_flag == 1)
			{
				//等时间到
				if(bsp_CheckTimer(NET_POWER_TMR_ID))
				{
					//拉高
					AIR208_CLT_ON;
					//用于判断3秒内是否开机，如果未开机，需要再次尝试关机再开机
					bsp_StartTimer(NET_POWER_TMR_ID, 3*TIME_SEC);
					ctl_flag = 2;
        }			
			}
      else if(ctl_flag == 2)
			{
				if(bsp_CheckTimer(NET_POWER_TMR_ID))//等待3秒钟的时间，如果AIR208_Switch_Check()为假，则关闭208，再次循环开机
				{
					ctl_flag = 0;
        }
      }
			//执行中
			return 0xff;
    }
  }
	else
	{
		//关机
		if(AIR208_Switch_Check() == 0)
		{
			//已经关机
			ctl_flag = 0;
			return 0;
    }
		else
		{
			if(ctl_flag == 0)
			{
				//拉低4秒
				AIR208_CLT_OFF;
				bsp_StartTimer(NET_POWER_TMR_ID, 4*TIME_SEC);
				ctl_flag = 1;
			}
			else if(ctl_flag == 1)
			{
				//等时间到
				if(bsp_CheckTimer(NET_POWER_TMR_ID))
				{
					//拉高
					AIR208_CLT_ON;
					//用于判断3秒内是否开机，如果未开机，需要再次尝试开机
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
			//执行中
			return 0xff;
    }
  }
}

/*
*********************************************************************************************************
*	函 数 名: ResolveLinkID
*	功能说明: 解析链接ID
*	形    参：无
*	返 回 值: 无
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
		//去掉帧头
		p = strstr((char*)pBuf,"CONNECT LINK ID:");
		p+=16;
  }
	else if(strstr((char*)pBuf,"CLOSE LINK ID:") != NULL)
	{
		//去掉帧头
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
*	函 数 名: ResolveCSQ
*	功能说明: 解析链接CSQ
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t ResolveCSQ(uint8_t *pBuf)
{
	uint8_t  cnt = 0;
	char     Val[5];
	uint16_t temp = 0;
	char     *p;
	
		//去掉帧头
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
*	函 数 名: ResolveIMEI
*	功能说明: 解析IMEI
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ResolveIMEI_HEX(uint8_t *pBuf,uint8_t *pAim)
{
	char     *p;
	uint8_t  i = 0;

	//去掉帧头
	p = strstr((char*)pBuf,"rrpc,getimei,");
	p+=13;
		
	//只有15位有效	
	for(i=0;i<7;i++)
	{
		pAim[i] = (uint8_t)(Ascii2UINT8(p[2*i])<<4)|(uint8_t)(Ascii2UINT8(p[2*i+1])<<0);
	}
	pAim[7] = (uint8_t)(Ascii2UINT8(p[14])<<4);	
}


/*
*********************************************************************************************************
*	函 数 名: ResolveIMEIstr
*	功能说明: 解析IMEI到字符串
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ResolveIMEIstr(uint8_t *pBuf,uint8_t *pAim)
{
	char     *p;
	uint8_t  i = 0;

	//去掉帧头
	p = strstr((char*)pBuf,"rrpc,getimei,");
	p+=13;
		
	//只有15位有效	
	for(i=0;i<15;i++)
	{
		pAim[i] = p[i];
	}
}

/*
*********************************************************************************************************
*	函 数 名: ResolveICCID
*	功能说明: 解析ICCID
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ResolveICCID_HEX(uint8_t *pBuf,uint8_t *pAim)
{
	char     *p;
	uint8_t  i = 0;

	//去掉帧头
	p = strstr((char*)pBuf,"rrpc,geticcid,");
	p+=14;
		
	//20位有效
	for(i=0;i<10;i++)
	{
		pAim[i] = (uint8_t)(Ascii2UINT8(p[2*i])<<4)|(uint8_t)(Ascii2UINT8(p[2*i+1])<<0);
	}
}

/*
*********************************************************************************************************
*	函 数 名: ResolveICCIDstr
*	功能说明: 解析ICCID为字符串
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ResolveICCIDstr(uint8_t *pBuf,uint8_t *pAim)
{
	char     *p;
	uint8_t  i = 0;

	//去掉帧头
	p = strstr((char*)pBuf,"rrpc,geticcid,");
	p+=14;
		
	//20位有效
	for(i=0;i<20;i++)
	{
		pAim[i] = p[i];
	}
}
/*
*********************************************************************************************************
*	函 数 名: ResolveNtpTime
*	功能说明: 解析ICCID网络时间
*	返 回 值: 无
*********************************************************************************************************
*/
//应答格式:rrpc,nettime,2019,12,12,22,53,31
uint8_t ResolveNtpTime(uint8_t *pBuf,_RTC *pAim)
{
	char     *p;
	uint8_t  cnt = 0;
	char     Val[5];
	
	//去掉帧头
	p = strstr((char*)pBuf,"rrpc,nettime,");
	p+=13;
	
	//解析年
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
	
	//去掉,
	p++;
	
	//解析月
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

	
	//去掉,
	p++;
	
	//解析日
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

	
	//去掉,
	p++;
	
	//解析时
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
	
	//去掉,
	p++;
	
	//解析分
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
	
	//去掉,
	p++;
	
	//解析秒
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
*	函 数 名: NetCheckSend
*	功能说明: 网络状态查询
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void NetCheckSend(void)
{
	char  SendBuf[] = {"rrpc,netstatus\r\n"};
	AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
}

/*
*********************************************************************************************************
*	函 数 名: GetNtpTimeSend
*	功能说明: 读取网络授时时间
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void GetNtpTimeSend(void)
{
	char  SendBuf[] = {"rrpc,gettime\r\n"};
	AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
}

/*
*********************************************************************************************************
*	函 数 名: GetIMEISend
*	功能说明: 读取IMEI的值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void GetIMEISend(void)
{
	char  SendBuf[] = {"rrpc,getimei\r\n"};
	if(GPRS_Para.ImeiGetOKFlag == 0)AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
}


/*
*********************************************************************************************************
*	函 数 名: GetCsqSend
*	功能说明: 读取CSQ的值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void GetCsqSend(void)
{
	char  SendBuf[] = {"rrpc,getcsq\r\n"};
	AIR208_Send((uint8_t*)SendBuf,strlen((char*)SendBuf),0);
}

/*
*********************************************************************************************************
*	函 数 名: FyIMEI
*	功能说明: 生成飞燕平台IMEI
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
void FyIMEI(uint8_t *pAim, uint8_t array[])
{
	char *pBuf;
	pBuf = (char*)mymalloc(SRAMIN, 16);
	memset(pBuf, 0 ,16);
	//strcat((char*)array,"https://g.aliplus.com/ilop/d.html?locale=all&pk=a1DNlQwHT07&dn=");//通用plc
	//strcat((char*)array,"https://g.aliplus.com/ilop/d.html?locale=all&pk=a1d6JgYInkz&dn=");//北方电磁热风机
	strcat((char*)array,"a1DNlQwHT07&dn=");//新规划plc只接收后面30个字符，前面的plc固定写死
	strncpy(pBuf, (const char*)pAim, 15);
	strcat((char*)array,(char*)pBuf);
	myfree(SRAMIN,(char*)pBuf);
}

/*
*********************************************************************************************************
*	函 数 名: FeiyanIMEI
*	功能说明: 生成飞燕平台IMEI
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
char *FeiyanIMEI(char *sIMEI, char *dst)
{
	  char *p = sIMEI;
    char *q = dst;
		*(q+30) = '\0';//将dst字符串最后一位写成结束标记
    strcat(q, "a1DNlQwHT07&dn=");
    strcat(q, p);
    return dst;
}

/*
*********************************************************************************************************
*	函 数 名: GetICCIDSend
*	功能说明: 读取ICCID的值
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: TransparentTask
*	功能说明: 透传任务
*	形    参：无
*	返 回 值: 无
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

/*****************只有首次上电时，才发送*******************/		
	if(FirstStart == 0)
	{
		if((GPRS_Para.Air208RunSta == 2)&&(GPRS_Para.ImeiGetOKFlag)&&(GPRS_Para.IccidGetOKFlag))
		{
			FirstStart = 1;
			//发送一次注册包，发送一次心跳包
//			Send_HeatBeatMsg();
		}	
	}
	
	
/*****************定时心跳*******************************/	
	if((GPRS_Para.ImeiGetOKFlag)&&(GPRS_Para.IccidGetOKFlag))
	{
		if(Air208RunSta_bak != GPRS_Para.Air208RunSta)
		{
			Air208RunSta_bak = GPRS_Para.Air208RunSta;
			if(GPRS_Para.Air208RunSta == 2)//联网,设备从断网变成联网，心跳定时开始
			{
				bsp_StartTimer(NET_SEND_TMR_ID, GPRS_Para.heartbeat_period*TIME_SEC);
			}
			else
			{
				bsp_StopTimer(NET_SEND_TMR_ID);
			}
		}

		//必须在线
		if(GPRS_Para.Air208RunSta == 2)
		{
			//时间到，定时发送
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
		
/*****************网络状态查询****************************/	
	if(bsp_CheckTimer(NET_CHECK_TMR_ID))
	{
		//读取一次IMEI
		GetIMEISend();	
		GetICCIDSend();	
		
		if((GPRS_Para.Air208RunSta == 1)||(GPRS_Para.Air208RunSta == 0))
		{
			//断网，20秒查询一次
			bsp_StartTimer(NET_CHECK_TMR_ID, 5*TIME_SEC);
			NetCheckSend();
			GetCsqSend();			
			//应答超时定时器
			bsp_StartTimer(NET_TIMEOUT_TMR_ID, 3*TIME_SEC);
		}
		else if(GPRS_Para.Air208RunSta == 2)
		{
			//联网，120秒查询一次
			bsp_StartTimer(NET_CHECK_TMR_ID, 10*TIME_SEC);
		}
	}
	//应答超时
	if(bsp_CheckTimer(NET_TIMEOUT_TMR_ID))
	{
		bsp_StopTimer(NET_TIMEOUT_TMR_ID);
		//断网
		GPRS_Para.Air208RunSta = 1;	
	}
	
/*****************网络授时执行****************************/	
	//必须联网后才进行网络授时
	if(GPRS_Para.Air208RunSta == 2)
	{
		if(GPRS_Para.NtpTimeOKFlag == 0)
		{
			//未授时,10秒后，开始查询时间
			bsp_StartTimer(NTP_TIME_TMR_ID, 10*TIME_SEC);
			GPRS_Para.NtpTimeOKFlag = 1;
    }
		else if(GPRS_Para.NtpTimeOKFlag == 1)
		{
			if(bsp_CheckTimer(NTP_TIME_TMR_ID))
			{
				//读取网络时间
				GetNtpTimeSend();
				GPRS_Para.NtpTimeOKFlag = 2;
				bsp_StartTimer(NTP_TIME_TMR_ID, 10*TIME_SEC);
			}
    }
		else if(GPRS_Para.NtpTimeOKFlag == 2)
		{
			//空闲，等待授时成功
			if(bsp_CheckTimer(NTP_TIME_TMR_ID))
			{
				bsp_StartTimer(NTP_TIME_TMR_ID, 10*TIME_SEC);
				GPRS_Para.NtpTimeOKFlag = 1;
      }
    }
		else if(GPRS_Para.NtpTimeOKFlag == 3)
		{
// 			//已经授时成功,每6小时授时一次
// 			bsp_StartTimer(NTP_TIME_TMR_ID, 6*TIME_HOUR);
// 			GPRS_Para.NtpTimeOKFlag = 1;
    }
  }
/********************************************************/


/**********************接收部分***************************/	
	//COM1，208接口
	if(bsp_CheckTimer(UART208))
	{
		bsp_StopTimer(UART208);
		
			if(strstr((char*)UART208_RXBUF,"CONNECT LINK ID:") != NULL)
			{
				//联网
				LinkID = ResolveLinkID(UART208_RXBUF);
				if((LinkID >= 1)&&(LinkID <= MAX_LINK))
				{
					GPRS_Para.link_flag[LinkID - 1] = 1;
				}
				
				//联网
				GPRS_Para.Air208RunSta = 2;
								
			}
			else if(strstr((char*)UART208_RXBUF,"CLOSE LINK ID:") != NULL)
			{
				//断网
				LinkID = ResolveLinkID(UART208_RXBUF);
				if((LinkID >= 1)&&(LinkID <= MAX_LINK))
				{
					GPRS_Para.link_flag[LinkID - 1] = 0;
			  }
	
					//全断网才断网
					for(i=0;i<MAX_LINK;i++)
					{
						if(GPRS_Para.link_flag[i])
						{
							break;
						}
					}
					if(i == MAX_LINK)
					{
						//断网
						GPRS_Para.Air208RunSta = 1;
						//断网，20秒查询一次
						bsp_StartTimer(NET_CHECK_TMR_ID, 20*TIME_SEC);
					}
					else
					{
						//联网
						GPRS_Para.Air208RunSta = 2;
					}
      }
			else if (strstr((char*)UART208_RXBUF, "rrpc,netstatus") != NULL)
			{
				//应答
				bsp_StopTimer(NET_TIMEOUT_TMR_ID);
				if (strstr((char*)UART208_RXBUF, "rrpc,netstatus,OK") != NULL)
				{
					//联网
					GPRS_Para.Air208RunSta = 2;					
				}
				else if (strstr((char*)UART208_RXBUF, "rrpc,netstatus,ERROR") != NULL)
				{
					//断网
					GPRS_Para.Air208RunSta = 1;
					memset(GPRS_Para.link_flag,0,MAX_LINK);
				}
			}
			else if (strstr((char*)UART208_RXBUF, "rrpc,nettime") != NULL)
			{
				//网络授时时间
				if (strstr((char*)UART208_RXBUF, "rrpc,nettime,error") != NULL)
				{
					//授时错误
        }
				else
				{
					{
						_RTC rtc;
						ret = ResolveNtpTime(UART208_RXBUF,&rtc);
						if(ret == 1)
						{
//							rtc.Week = getweek(rtc.Year,rtc.Mon,rtc.Day);
							//执行时间校准
//							DS1302_SetTime(rtc);
							//网络授时成功
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
					
					//获得IMEI
					GPRS_Para.ImeiGetOKFlag = 1;
        }
      }
			else if (strstr((char*)UART208_RXBUF, "rrpc,geticcid") != NULL)
			{
				if(UART208_tUART.usRxCount > 34)
				{
					//解析ICCID为hex
// 					memset(GPRS_Para.ICCID,0,10);
// 					ResolveICCID_HEX(UART208_RXBUF,GPRS_Para.ICCID);
					
					//解析ICCID为str
					memset(GPRS_Para.sICCID,0,21);
					ResolveICCIDstr(UART208_RXBUF,GPRS_Para.sICCID);
					
// 					GPRS_Para.ICCID[40] = '\0';					
// 					To_Hex((char*)GPRS_Para.sICCID, 20, (char*)GPRS_Para.ICCID);
					//获得ICCID
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
						//此时触发一次M寄存器上报
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
				  //解析json
         Parse_208((const char*)UART208_RXBUF);
      }

		//清空接收
		comClearRxFifo(UART208);
  }

/**********************发送部分***************************/	
		//循环发送队列
	  if(GPRS_Para.NetSendFlag == 0)
		{
			{
				//首先判断队列中是否存在有效命令
				if(!GPRS_IsQueueEmpty(&g_GPRSQueue))
				{
					//获得队头元素
					//GPRS_GetHead(&g_GPRSQueue,&e);
					GPRS_DeQueue(&g_GPRSQueue,&e);
					
					//数据发送
			    comSendBuf(UART208,(uint8_t*)e.buf,e.len);//SIM800C_UART
					GPRS_Para.NetSendFlag = 1;
				}			
			}
    }
    else if(GPRS_Para.NetSendFlag == 1)
		{
			//开启一个200ms定时器，最小命令时间间隔
			bsp_StartTimer(NET_SEND_DELAY_TMR_ID, 800);//之前是200
			GPRS_Para.NetSendFlag = 2;
		}			
		else if(GPRS_Para.NetSendFlag == 2)
		{
			//判断是否应答超时
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
*	函 数 名: AIR208_Task
*	功能说明: 208任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AIR208_Task(void)
{
	uint8_t ret = 0;

	//先打开电源
	if(GPRS_Para.Air208RunStep == 0)
	{
		AIR208_Power_Ctl(1);
		GPRS_Para.Air208RunStep = 1;
		bsp_StartTimer(NET_POWER_TMR_ID, 2*TIME_SEC);
	}
	//延时2秒在判断
	else if(GPRS_Para.Air208RunStep == 1)
	{
		//等时间到
		if(bsp_CheckTimer(NET_POWER_TMR_ID))
		{
			bsp_StopTimer(NET_POWER_TMR_ID);
			GPRS_Para.Air208RunStep = 2;
		}	
	}
	//开机
	else if(GPRS_Para.Air208RunStep == 2)
	{
		ret = AIR208_Switch_Ctl(1);
		if(ret == 1)
		{
			GPRS_Para.Air208RunStep = 3;
			if(GPRS_Para.Air208RunSta == 0)
			{
				GPRS_Para.Air208RunSta = 1;
				//初始化软件部分
				AIR208_InitPara();
			}
		}	
	}
	//正常收发任务
	else if(GPRS_Para.Air208RunStep == 3)
	{
		//如果已经开机
		if((GPRS_Para.Air208RunSta)&&(AIR208_Switch_Check()))
		{
			TransparentTask();
		}
		//没开机
		else
		{
			GPRS_Para.Air208RunStep = 4;
		}
	}
	//执行关闭电源
	else if(GPRS_Para.Air208RunStep == 4)
	{
		AIR208_Power_Ctl(0);
		bsp_StartTimer(NET_POWER_TMR_ID, 2*TIME_SEC);
		GPRS_Para.Air208RunStep = 5;	
	}
	//延时处理，在打开电源开机
	else
	{
		//等时间到
		if(bsp_CheckTimer(NET_POWER_TMR_ID))
		{
			bsp_StopTimer(NET_POWER_TMR_ID);
			GPRS_Para.Air208RunStep = 0;
		}	
	}
}


/*
*********************************************************************************************************
*	函 数 名: AliEnQueue
*	功能说明: 物模型入队
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AliEnQueue(uint16_t Func, uint16_t Address)
{
	  ElemType e;
		e.slave        = 1;
		e.func         = Func;
		e.addr         = Address;	 	                    //保持寄存器的首地址，显示缓冲区
		e.cnt          = 1;                            //根据从机个数确定写入的寄存器个数
		e.startTimeout = bsp_GetRunTime();
		e.timeout      = 1000;	                        //命令 超时时间
		e.startPeriod  = bsp_GetRunTime();
		e.period       = 5*1000;	                      //命令发送周期
		e.commCnt      = PERIOD_CNT;                    //不可删除命令
		e.timeoutcnt   = 0;		                          //错误计数清零
		
		T_EnQueue(&Trans_Queue, e);

}

/*
*********************************************************************************************************
*	函 数 名: Parse_208
*	功能说明: 解析208串口数据
*	形    参：无
*	返 回 值: 无
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
		json = cJSON_Parse(JSON);//解析串口的json
		if (json)
		{
			cjson_params = cJSON_GetObjectItem(json,"params");
				
			if(cjson_params!=NULL)
			{	
					//M寄存器
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
				//D寄存器
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
			cJSON_Delete(json); // 释放内存
	}
	}
}
