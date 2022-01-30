#include "bsp_sim800c_rtos.h"
#include "malloc.h"
#include "GprsQueue.h"
#include "param.h"
#include "data.h"


/***************************************************************************/
/////////////////////////////////这两条指令待研究///////////////////////////
//AT+CIICR                                  建立无线链路
//AT+CIFSR                                  获得本地IP地址

/////////////////////////////////以下指令可用///////////////////////////////
//AT+CPIN?                                  检查SIM卡状态
//AT+CSQ                                    检查网络信号强度
//AT+CREG?                                  检查网络注册状态
//AT+CGATT?                                 检查GPRS附着状态
//AT+CSTT="CMNET"                           设置APN
//AT+CIPSTART="TCP","175.49.143.24","8899"  建立TCP连接
//AT+CIPSEND                                发送数据到远端服务器
//AT+CIPCLOSE                               主动关闭连接（关闭TCP、UDP链接）
//AT+CIPSHUT                                关闭PDP上下文（关闭链接后再发送该指令，再重新链接）


//+++                                       由数据和PPP模式切换到命令模式，指令前后必须1s
//ATO                                       由命令模式切换到数据模式
//AT+CIPMODE=0或1                           0：正常模式；1：透传模式
//AT+CPOWD=1                                关机


////////////////////////////////////////////////底层相关/////////////////////////////////////////////////
//网咯状态监测定时器
OS_TMR 		   OS_TmrNetCheck; 
//GPRS数据消息队列
OS_Q         Q_GPRS_Data;
//网络参数
NETWORK_PARA NetPara;

static void OS_TmrNetCheckCallback(void *p_tmr, void *p_arg);

void Init_SIM800C(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		//初始化IO
		RCC_APB2PeriphClockCmd(RCC_SIM800C, ENABLE);

	  //配置输出引脚
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    /* 设为输出口 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     	/* IO口最大速度 */
	
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_KEY;
		GPIO_Init(GPIO_PORT_KEY, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_POWER;
		GPIO_Init(GPIO_PORT_POWER, &GPIO_InitStructure);
	
	  //配置输入引脚
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		/* 设为输入口 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      /* IO口最大速度 */
		
		GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_STATE;
		GPIO_Init(GPIO_PORT_STATE, &GPIO_InitStructure);
	  
	  //内部变量清零
  	memset((char*)&NetPara,0,sizeof(NETWORK_PARA));
	  //初始化循环队列
    GPRS_InitQueue(&g_GPRSQueue);
		
		//打开电源
		POWER_OFF();
}

/*
*********************************************************************************************************
*	函 数 名: Init_SIM800C_Soft
*	功能说明: 初始化软件系统参数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Init_SIM800C_Soft(void)
{
	uint8_t err;
	//创建定时器
	OSTmrCreate((OS_TMR		*)&OS_TmrNetCheck,		            //定时器
              (CPU_CHAR	*)"OS_TmrNetCheck",		            //定时器名字
              (OS_TICK	 )5000,			                      //5秒后启动
              (OS_TICK	 )1000,                           //周期1秒
              (OS_OPT		 )OS_OPT_TMR_PERIODIC,            //周期模式
              (OS_TMR_CALLBACK_PTR)OS_TmrNetCheckCallback,//定时器回调函数
              (void	    *)0,					                    //参数为0
              (OS_ERR	    *)&err);		                    //返回的错误码	
	 OSTmrStart ((OS_TMR*)&OS_TmrNetCheck,(OS_ERR*)&err);		
  
  //创建GPRS数据消息队列，队列长度为5
  OSQCreate((OS_Q*)&Q_GPRS_Data,(CPU_CHAR*)"Q_GPRS_Data",(OS_MSG_QTY)5,(OS_ERR*)&err);					
}
/*
*********************************************************************************************************
*	函 数 名: SIM800C_HardPowerOn
*	功能说明: 硬件KEY控制模块开启
*	形    参: 无
*	返 回 值: TRUE:打开成功；FALSE：打开失败；
*********************************************************************************************************
*/
uint8_t SIM800C_HardPowerOn(void)
{
	uint16_t cnt = 0;
	POWER_ON();
	BSP_OS_TimeDlyMs(300);
	//当前已经开机
	if(STATE)
	{
		DBGU_Msg("GPRS power current ON...!\r\n");	
		return TRUE;
	}
	//当前关机，需要打开
	else
	{
		BSP_OS_TimeDlyMs(100);
		DBGU_Msg("GPRS power current OFF...!\r\n");
		KEY_ON();
		BSP_OS_TimeDlyMs(1000);
		KEY_OFF();
		//至少拉低2秒以上
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
*	函 数 名: SIM800C_HardPowerOff
*	功能说明: 硬件KEY控制模块关机
*	形    参: 无
*	返 回 值: TRUE:关闭成功；FALSE：关闭失败；
*********************************************************************************************************
*/
uint8_t SIM800C_HardPowerOff(void)
{
	uint16_t cnt = 0;
	uint8_t  ret = 0;
	POWER_OFF();
	BSP_OS_TimeDlyMs(500);
	//当前已经关机
	if(!STATE)
	{
		DBGU_Msg("GPRS power current OFF...!\r\n");	
		return TRUE;
	}
	//当前开机，需要关闭
	else
	{
		DBGU_Msg("GPRS power current ON...!\r\n");
// 		KEY_ON();
// 		BSP_OS_TimeDlyMs(1000);
// 		//首先通过软件来关机
// 		ret = Printf_AT(2,8000, "NORMAL POWER DOWN", "AT+CPOWD=1\r\n"); 
// 		if(ret == 1)
// 		{
// 			BSP_OS_TimeDlyMs(100);
// 			DBGU_Msg("GPRS Soft PowerDown OK!\r\n");
//     }
// 		//如果关闭不成功，再通过硬件关机
// 		else
// 		{
// 			DBGU_Msg("GPRS Soft PowerDown Error!\r\n");
// 			KEY_OFF();
// 			//至少拉低1秒钟
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
		
		//AIR208需要延时
    BSP_OS_TimeDlyMs(3000);		
		return TRUE;
	}
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_SwitchCmdMode
*	功能说明: 切换到命令模式
*	形    参: 无
*	返 回 值: 1:成功，0:失败
*********************************************************************************************************
*/
uint8_t SIM800C_SwitchCmdMode(void)
{
	uint8_t  ret = 0;
	//切换前一秒空闲
	BSP_OS_TimeDlyMs(1500);
	SIM800C_SendStr("+++");//模式切换
	//等待应答
	ret = SIM800C_WaitATResponse("OK", 3000);
	//切换后一秒空闲
	BSP_OS_TimeDlyMs(800);

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_SwitchDataMode
*	功能说明: 切换到数据模式，透传模式
*	形    参: 无
*	返 回 值: 1:成功，0:失败
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
*	函 数 名: SIM800C_Baud_Set
*	功能说明: 设置SIM800C波特率，确保波特率为115200
*	形    参: 无
*	返 回 值: 无
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
			//恢复出厂默认设置
     	Printf_AT(3,1000, "OK", "AT&F\r\n"); 
			//设置SIM800C波特率为115200
			Printf_AT(3,1000, "OK","AT+IPR=115200\r\n");
			BSP_OS_TimeDlyMs(100);
			bsp_SetUartBaud(SIM800C_UART,115200);
			DBGU_Msg("Set sim800c baud is : 115200");			
			break;
		}
	}
	//轮询结束后，还没查询到波特率
	if(ret == 0)
	{
		DBGU_Msg("Current sim800c baud is error!\r\n");
	}
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_CMD_Init
*	功能说明: 发送初始化命令
*	形    参: 无
*	返 回 值: 无
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
	
	//重连次数清零
	NetPara.Reconnect_cnt = 0;
	//首先确认波特率
	SIM800C_Baud_Set();
	//关闭回显
	ret = SIM800C_Echo_Ctl(FALSE);
	if(ret != 1)
	{
		return 0;
  }
	
  //全功能模式
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
	//APN类型
	if(GPRS_Para.apn_type == 0)
	{
		//移动
		sprintf((char*)NetPara.APN,"CMMTM");
		//联通
//		sprintf((char*)NetPara.APN,"UNIM2M.NJM2MAPN");
	}

	//读取ICCID，同时也是检测了SIM卡是否存在
	ret = SIM800C_ReadICCID((uint8_t*)NetPara.ICCID,3,2000);
	if(ret == 1)
	{
// 		for(i=0;i<20;i++)
// 		{
// 			//如果SIM的ICCID发生了变化，则复位存储参数
// 			if(GPRS_Para.ICCID[i] != Ascii2UINT8(NetPara.ICCID[i]))
// 			{
// 				LoadParam(1);
// 				break;
// 			}
// 		}
		//存储当前ICCID
		for(i=0;i<10;i++)
		{
			GPRS_Para.ICCID[i] = (uint8_t)(Ascii2UINT8(NetPara.ICCID[2*i])<<4)|(uint8_t)(Ascii2UINT8(NetPara.ICCID[2*i+1])<<0);
		}
  }
	else
	{
		return 0;		
  }

	//检查SIM卡
	ret = SIM800C_CheckPIN(5,5000);
	if(ret != 1)
	{
		return 0;
  }
	
	//当模块从GPRS网络掉线时，会上报"+CGREG: 1,0"
	Printf_AT(2,2000, "OK","AT+CGREG=1\r\n");		

	cnt = 0;
	do
	{
		//检测网络注册情况
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
	
 	//用于读取基站信息
	Printf_AT(2,2000, "OK","AT+CREG=2\r\n");	
	SIM800C_ReadLacCi(&NetPara.Lac,&NetPara.Ci,2,2000);
#if SIM800C_TRANSPARENT
	/*****************透传模式***************/
	//设置链接模式为透传模式
	Printf_AT(2,2000, "OK", "AT+CIPMODE=1\r\n"); 
#else
	/*****************命令模式***************/	
	//设置链接模式为命令模式
	Printf_AT(2,2000, "OK", "AT+CIPMODE=0\r\n"); 
#endif
	//设置流控制，禁用
  Printf_AT(2,2000, "OK", "AT+IFC=0,0\r\n");
#if SIM800C_TRANSPARENT

#else
	//设置接收数据显示IP头(方便判断数据来源)	
	Printf_AT(2,2000, "OK","AT+CIPHEAD=1\r\n");
#endif	
	//保存参数
	Printf_AT(3,1000, "OK","AT&W\r\n");
  return 1;
}
/*
*********************************************************************************************************
*	函 数 名: SIM800C_Check_PowerSta
*	功能说明: 检查电源状态
*	形    参: 
*	返 回 值: 无
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
*	函 数 名: SIM800C_WaitATResponse
*	功能说明: 等待SIM800C返回指定的应答字符串. 比如等待 OK
*	形    参: _pAckStr : 应答的字符串， 长度不得超过255
*			 _usTimeOut : 命令执行超时，0表示一直等待. >０表示超时时间，单位1ms
*	返 回 值: 1 表示成功  0 表示失败
*********************************************************************************************************
*/
uint8_t SIM800C_WaitATResponse(char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t  ucData;
	uint16_t pos = 0;
	uint8_t  ret;
	uint8_t  *p;

	//分配一块内存
	p = mymalloc(SRAMIN,256);
	//清空临时变量缓冲区
	memset((char*)p,0,256);
	
	if (strlen(_pAckStr) > 255)
	{
		myfree(SRAMIN,p);
		return 0;
	}
	/* _usTimeOut == 0 表示无限等待 */
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
				ret = 0xff;	    /* 超时 */
#ifdef SIM800C_DBGU
			DBGU_Msg("AT CMD Timeout\r\n");	
#endif		
				break;
			}
		}
		
		//等待超时成帧
		if(bsp_CheckTimer(SIM800C_UART))
		{
			bsp_StopTimer(SIM800C_UART);
			
			//读空缓冲区
			while(comGetChar(SIM800C_UART, &ucData))
			{
				p[pos++] = ucData;
				if(pos > 255)
				{
					ret = 0;	    /* 错误 */
					break;					
				}
			}
			
			//先转换为字符串，再做处理
			sprintf((char*)p,"%s",(char*)p);
#ifdef SIM800C_DBGU
			//打印应答消息
			DBGU_Msg("%s\r\n",p);	
#endif			
			
			if (strstr((char*)_pAckStr, "CONNECT OK") != NULL)
			{
				if (strstr((char*)p, "\r\nCONNECT\r\n") != NULL)
				{
					ret = 1;	/* 收到指定的应答数据，返回成功 */
					break;
        }
      }
			
			//联网状态
			if (strstr((char*)_pAckStr, "+CREG: 0,1") != NULL)
			{
				if (strstr((char*)p, "+CREG: 0,5") != NULL)
				{
					ret = 1;	/* 收到指定的应答数据，返回成功 */
					break;
        }
      }
			
			//确认是否包含对应的字符串
			if (strstr((char*)p, _pAckStr) != NULL)
			{
				ret = 1;	/* 收到指定的应答数据，返回成功 */
				break;
			}
			else
			{
				ret = 0;	    /* 错误 */
				break;					
			}
		}
	}
	myfree(SRAMIN,p);
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_SendAT_WithAck
*	功能说明: 向GSM模块发送AT命令。 本函数自动在AT字符串口增加<CR>字符
*	形    参: _Cmd:命令，不包括/r/n
*           _pAckStr：应答字符串，不包括/r/n
*           _usTimeOut:等待应答超时时间，单位ms
*	返 回 值: TRUE：成功；FALSE:失败；
*********************************************************************************************************
*/
uint8_t SIM800C_SendAT_WithAck(char *_Cmd, char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t ret;
	//清空FIFO
	comClearFifo(SIM800C_UART);
	//发送命令
	comSendBuf(SIM800C_UART, (uint8_t *)_Cmd, strlen(_Cmd));
	comSendBuf(SIM800C_UART, (uint8_t *)"\r\n", 2);
	//等待应答
	ret = SIM800C_WaitATResponse(_pAckStr, _usTimeOut);
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_SendAT_NoAck
*	功能说明: 向GSM模块发送AT命令。 本函数自动在AT字符串口增加<CR>字符
*	形    参: _Cmd:命令，不包括/r/n
*	返 回 值: 无
*********************************************************************************************************
*/
void SIM800C_SendAT_NoAck(char *_Cmd)
{
	//清空FIFO
	comClearFifo(SIM800C_UART);
	//发送命令
	comSendBuf(SIM800C_UART, (uint8_t *)_Cmd, strlen(_Cmd));
	comSendBuf(SIM800C_UART, (uint8_t *)"\r\n", 2);
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_SendStr
*	功能说明: 发送字符串
*	形    参: _Str:需要发送的字符串
*	返 回 值: 无
*********************************************************************************************************
*/
void SIM800C_SendStr(char *_Str)
{
	//清空FIFO
	comClearFifo(SIM800C_UART);
	//发送字符串
	comSendBuf(SIM800C_UART, (uint8_t *)_Str, strlen(_Str));
}

/*
*********************************************************************************************************
*	函 数 名: Printf_AT
*	功能说明: 格式化输出的AT命令，包括等待应答
*	形    参: _try:出错时重发次数
*           _usTimeOut:超时时间
*           _pAckStr:应答字符串
*           fmt:格式化的字符串
*	返 回 值: 无
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
		//分配一块内存
		p = mymalloc(SRAMIN,128);
		memset((char*)p,0,128);
		
		//清空FIFO
		comClearFifo(SIM800C_UART);	
		//不允许格式化的字符串大于128,缓冲区长度决定
		if(strlen(fmt) > 128)
		{
			myfree(SRAMIN,p);
			return FALSE;
		}
		va_start(v_list, fmt);     
		vsprintf((char*)p, fmt, v_list); 
		va_end(v_list);	
#ifdef SIM800C_DBGU
		//打印待发送的消息
		DBGU_Msg("%s\r\n",p);	
#endif		
		//发送命令
		comSendBuf(SIM800C_UART, p, strlen((char*)p));
		
		if (strstr((char*)_pAckStr, "CONNECT OK") != NULL)
		{
			BSP_OS_TimeDlyMs(2000);
    }
		//等待应答
		ret = SIM800C_WaitATResponse(_pAckStr, _usTimeOut);
		//释放
		myfree(SRAMIN,p);
    cnt++;		
	}
	while(((ret == 0)&&(cnt < _try))||((ret == 0xff)&&(cnt < _try)));

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: Shut_Down_Phone
*	功能说明: 挂断电话
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t Shut_Down_Phone(void)
{
	//ATH命令最大应答时间是3秒
	return Printf_AT(3,3000,"OK","ATH\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: Call_To_Phone
*	功能说明: 拨打
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t Call_To_Phone(uint8_t *number)
{
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_ConnectNetwork
*	功能说明: 连接网络
*	形    参: net：网络参数
*	返 回 值: 无
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
		//首先确保为命令模式
		SIM800C_SwitchCmdMode();
  }
#else
	
#endif

		//每次重新链接会调用次函数
	net->Reconnect_cnt++;
	if(net->Reconnect_cnt > 2)
	{
		return TIMEOUT;
  }
	
	Printf_AT(1,5000,"OK", "AT+CIPCLOSE=1\r\n");
	BSP_OS_TimeDlyMs(200);
	Printf_AT(1,5000,"SHUT OK", "AT+CIPSHUT\r\n");	
	
/***********************读取信号强度**************************/
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

	//设置PDP上下文,互联网接协议,接入点等信息
	ret = Printf_AT(3,2000, "OK","AT+CGDCONT=1,\"IP\",\"%s\"\r\n",net->APN);
	//附着GPRS业务
	ret = Printf_AT(3,5000, "OK","AT+CGATT=1\r\n");	
	
	cnt = 0;
	do
	{
		//检测网络注册情况
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
	
  //获取本地IP地址
	ret = Printf_AT(2,3000, "OK","AT+CIFSR\r\n");
	
	//分配一块内存
	p = mymalloc(SRAMIN,256);
	//判断内存是否分配成功
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
			//释放内存
	    myfree(SRAMIN,p);
			goto again;
    }
  }while(((ret == 0)||(ret == 0xff)));	
	if(ret == TRUE)
	{
		//重连总次数清零
		NetPara.ReconnectTotal = 0;
		//重连次数清零
		net->Reconnect_cnt = 0;
#if SIM800C_TRANSPARENT
			//进入命令模式
	    SIM800C_SwitchCmdMode();
#endif		
		//读取信号强度
		SIM800C_ReadRSSI(&net->Rssi,2,2000);
#if SIM800C_TRANSPARENT
		 //进入数据模式
	   SIM800C_SwitchDataMode();
#endif			
#if SIM800C_TRANSPARENT
		//透传链接
		DBGU_Msg("Data Connect Network OK...TYPE=\"%s\",IP=\"%s\",PORT=\"%s\"!\r\n",net->NetType,net->IpAddr,net->IpPort);
#else
		//命令链接
		DBGU_Msg("Cmd Connect Network OK...TYPE=\"%s\",IP=\"%s\",PORT=\"%s\"!\r\n",net->NetType,net->IpAddr,net->IpPort);
#endif		
	}
	else
	{
		DBGU_Msg("Connect Network Error...!\r\n");	
	}
	//释放内存
	myfree(SRAMIN,p);
	
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_CheckPIN
*	功能说明: 检查PIN状态
*	形    参: 无
*	返 回 值: 无
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
			//分配一块内存
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CPIN?");
			
			/* _usTimeOut == 0 表示无限等待 */
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
						ret = 0;	    /* 超时 */
						break;
					}
				}
				
				//等待超时成帧
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//清空计数
					pos = 0;
					//读空缓冲区
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Check SIM recieve error...!\r\n");
							ret = 0;	    /* 错误 */
							break;					
						}
					}
					
				  //先转换为字符串，再做处理
			    sprintf((char*)p,"%s",(char*)p);
					//打印应答消息
					DBGU_Msg("%s\r\n",p);	
					
					//确认是否包含对应的字符串
					if (strstr((char*)p, "+CPIN:") != NULL)
					{
						//接收到了应答
						//在判断应答类型
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
						ret = 0;	    /* 错误 */
						break;					
					}
				}
			}
			
			//释放动态内存
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_ReadIMEI
*	功能说明: 读取序列号
*	形    参: 无
*	返 回 值: 无
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
			//分配一块内存
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+GSN");
			
			/* _usTimeOut == 0 表示无限等待 */
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
						ret = 0;	    /* 超时 */
						break;
					}
				}
				
				//等待超时成帧
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//清空计数
					pos = 0;
					//读空缓冲区
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Read IMEI Error...!\r\n");
							ret = 0;	    /* 错误 */
							break;					
						}
					}
					
				  //先转换为字符串，再做处理
			    sprintf((char*)p,"%s",(char*)p);
					//打印应答消息
					DBGU_Msg("%s\r\n",p);	
					
					//确认是否包含对应的字符串
					if (strstr((char*)p, "OK") != NULL)
					{
						//截取数字序列号
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
						ret = 0;	    /* 错误 */
						break;					
					}
				}
			}
			
			//释放动态内存
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}


/*
*********************************************************************************************************
*	函 数 名: SIM800C_ReadICCID
*	功能说明: 读取序列号
*	形    参: 无
*	返 回 值: 无
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
			//分配一块内存
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CCID");
			
			/* _usTimeOut == 0 表示无限等待 */
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
						ret = 0;	    /* 超时 */
						break;
					}
				}
				
				//等待超时成帧
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//清空计数
					pos = 0;
					//读空缓冲区
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Read ICCID Error...!\r\n");
							ret = 0;	    /* 错误 */
							break;					
						}
					}
					
				  //先转换为字符串，再做处理
			    sprintf((char*)p,"%s",(char*)p);
					//打印应答消息
					DBGU_Msg("%s\r\n",p);	
					
					//确认是否包含对应的字符串
					if (strstr((char*)p, "OK") != NULL)
					{
						//截取数字序列号
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
						ret = 0;	    /* 错误 */
						break;					
					}
				}
			}
			
			//释放动态内存
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_ReadRSSI
*	功能说明: 读取信号强度
*	形    参: 无
*	返 回 值: 无
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
			//分配一块内存
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CSQ");
			
			/* _usTimeOut == 0 表示无限等待 */
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
						ret = 0;	    /* 超时 */
						break;
					}
				}
				
				//等待超时成帧
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//清空计数
					pos = 0;
					//读空缓冲区
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Read RSSI Error...!\r\n");
							ret = 0;	    /* 错误 */
							break;					
						}
					}
					
				  //先转换为字符串，再做处理
			    sprintf((char*)p,"%s",(char*)p);
					//打印应答消息
					DBGU_Msg("%s\r\n",p);	
					
					//确认是否包含对应的字符串
					if (strstr((char*)p, "OK") != NULL)
					{
						p = (uint8_t*)strstr((char*)p,(char*)":");
						//去掉分号
						p++;
						//去掉空格
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
						ret = 0;	    /* 错误 */
						break;					
					}
				}
			}
			
			//释放动态内存
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_ReadLacCi
*	功能说明: 基站定位
*	形    参: 无
*	返 回 值: 无
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
			//分配一块内存
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CREG?");
			
			/* _usTimeOut == 0 表示无限等待 */
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
						ret = 0;	    /* 超时 */
						break;
					}
				}
				
				//等待超时成帧
				if(bsp_CheckTimer(SIM800C_UART))
				{
					bsp_StopTimer(SIM800C_UART);
					//清空计数
					pos = 0;
					//读空缓冲区
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							DBGU_Msg("Read Error...!\r\n");
							ret = 0;	    /* 错误 */
							break;					
						}
					}
					
				  //先转换为字符串，再做处理
			    sprintf((char*)p,"%s",(char*)p);
					//打印应答消息
					DBGU_Msg("%s\r\n",p);	
					
					//确认是否包含对应的字符串
					if (strstr((char*)p, "+CREG") != NULL)
					{
						p = (uint8_t*)strstr((char*)p,(char*)"\"");
						//左侧双引号
						p++;
						*Lac = CharToHex(*p++);
						*Lac <<= 4;
						*Lac |= CharToHex(*p++);
						*Lac <<= 4;
						*Lac |= CharToHex(*p++);
						*Lac <<= 4;
						*Lac |= CharToHex(*p++);
						//右侧双引号
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
						ret = 0;	    /* 错误 */
						break;					
					}
				}
			}
			
			//释放动态内存
			myfree(SRAMIN,p);
			cnt++;
	}while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_ATMode
*	功能说明: 进入AT命令模式
*	形    参: 
*	返 回 值: 无
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
*	函 数 名: SIM800C_Echo_Ctl
*	功能说明: 回显控制
*	形    参: 
*	返 回 值: 无
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
*	函 数 名: SIM800C_Check_NetSta
*	功能说明: 检查网络状态
*	形    参: 
*	返 回 值: 无
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
			//分配一块内存
			p = mymalloc(SRAMIN,256);
			memset((char*)p,0,256);
			
			SIM800C_SendAT_NoAck("AT+CIPSTATUS");
			
			/* _usTimeOut == 0 表示无限等待 */
			if (_usTimeOut > 0)
			{
				bsp_StartTimer(SIM800C_TMR_ID, _usTimeOut);	
			}

			while (1)
			{
				BSP_OS_TimeDlyMs(1);
				if (_usTimeOut > 0)//如果不是_usTimeTimer ==0, 不是无限等待，那么开始bsp_checkTimer
				{
					if (bsp_CheckTimer(SIM800C_TMR_ID))
					{
						ret = 0;	    /* 超时 */
						break;
					}
				}
				
				
				if(bsp_CheckTimer(SIM800C_UART))//等待超时成帧
				{
					bsp_StopTimer(SIM800C_UART);
					//清空计数
					pos = 0;
					//读空缓冲区
					while(comGetChar(SIM800C_UART, &ucData))
					{
						p[pos++] = ucData;
						
						if(pos > 255)
						{
							ret = 0;	    /* 错误 */
							break;					
						}
					}
					
				  //先转换为字符串，再做处理
			    sprintf((char*)p,"%s",(char*)p);
					
					//确认是否包含对应的字符串
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
					else if (strstr((char*)p, "CONNECT OK") != NULL)//联网后返回6
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
		//释放动态内存
		myfree(SRAMIN,p);
		cnt++;
	}while((ret == 0)&&(cnt < _try));		
			
	return ret;
}
/*
*********************************************************************************************************
*	函 数 名: SIM800C_Send
*	功能说明: 命令模式发送数据
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t SIM800C_Send(uint8_t *pBuf,uint16_t len)
{
	ElemType_G e;
	uint8_t    ret = 0;
#if SIM800C_TRANSPARENT
// 	//清空FIFO
// 	comClearTxFifo(SIM800C_UART);	
// 		//发送字符串
// 	comSendBuf(SIM800C_UART, pBuf, len);
// 	BSP_OS_TimeDlyMs(200);
	//加入队列中
	e.len = len;
	memcpy(e.buf,pBuf,e.len);
	GPRS_EnQueue(&g_GPRSQueue, e);
#else
	ret = Printf_AT(2,500,">","AT+CIPSEND=%d\r\n",len);
	if(ret)
	{
			//清空FIFO
		  comClearTxFifo(SIM800C_UART);	
			//发送字符串
			comSendBuf(SIM800C_UART, pBuf, len);
	}
	else
	{
			//清空FIFO
		  comClearTxFifo(SIM800C_UART);	
		  //取消发送
		  comSendChar(SIM800C_UART, 0x1b);//ESC
	}
#endif
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_Recv_Len
*	功能说明: 计算接收到的有效数据长度
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
//+IPD,16:http://en.usr.cn
uint16_t SIM800C_Recv_Len(uint8_t *pBuf)
{
	uint8_t  cnt = 0;
	uint8_t  *p;
	//存储长度字符
	uint8_t  *pLen;
	uint16_t len = 0;
	
	//声明动态内存
	pLen = mymalloc(SRAMIN,256);
	memset((char*)pLen,0,256);
	
	//搜索第一个出现“,"的位置
	p = (uint8_t*)strstr((char*)pBuf, ",");
	//不要','，这个字符
	p++;
	//在搜索":"
	do
	{
		pLen[cnt++] = *p++;
	}while(*p != ':');
	//转换为字符串
	sprintf((char*)pLen,"%s",(char*)pLen);
  
	len = atoi((char*)pLen);
	
	//释放动态内存
	myfree(SRAMIN,(char*)pLen);
	return len;
}


////////////////////////////////////////////////应用相关/////////////////////////////////////////////////

/*
*********************************************************************************************************
*	函 数 名: Send_HeatBeatMsg
*	功能说明: 发送心跳消息
*	形    参: 
*	返 回 值: 无
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
		
		//分配一块动态内存
		pBuf = (char*)mymalloc(SRAMIN,256);
		p    = (char*)mymalloc(SRAMIN,256);
		//判断内存申请是否正确
		if(pBuf == NULL)
		{
			myfree(SRAMIN,pBuf);
			return;
		}
		//动态内存清零
		memset((char*)pBuf,0,256);	
		memset((char*)p,0,256);
		//心跳包序号增加
		GPRS_Para.heartbeat_sn++;
		
		//转换为字符串，ICCID
		sprintf((char*)p,"%s",(char*)NetPara.ICCID);    
		//帧头
		strcat((char*)pBuf,"glxxx,ICCID,");
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//厂商编号
		sprintf((char*)p,"%d",77701);//plc上下限型
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//心跳包序号
		sprintf((char*)p,"%d",GPRS_Para.heartbeat_sn);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
// 		//心跳周期
// 		sprintf((char*)p,"%d",GPRS_Para.heartbeat_period);
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,",");
		//设备在线标志
// 		sprintf((char*)p,"%d",GPRS_Para.OnlineFlag);
// 		strcat((char*)pBuf,(char*)p);
// 		strcat((char*)pBuf,",");	
    //出水温度
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[313]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
    //回水温度
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[310]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//热源温度
		//sprintf((char*)p,"%d",g_PduUART485[0].RegHold[333]);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		//开机状态
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[338]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//运行模式
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[339]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//水位报警
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[345]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//超温报警
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[346]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//温度探头异常状态
		//sprintf((char*)p,"%d",GPRS_Para.TempProbeSta);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		//漏电报警状态
		//sprintf((char*)p,"%d",GPRS_Para.PowerSta);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");	
		//水泵1故障
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[347]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//水泵2故障
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[348]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
    //水泵1运行状态
		//sprintf((char*)p,"%d",GPRS_Para.PumpRunSta[0]);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");	
		//水泵2运行状态
		//sprintf((char*)p,"%d",GPRS_Para.PumpRunSta[1]);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
			//水位状态
	//sprintf((char*)p,"%d",GPRS_Para.WaterLevelSta);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
	 //采集电压值
		//sprintf((char*)p,"%.1f",(GPRS_Para.VoltVal/10.0));
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		//采集电流值
		//sprintf((char*)p,"%.1f",(GPRS_Para.CurVal/10.0));
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		
		//报警温度
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[320]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
			//预留位
		
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

    //防冻温度
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[318]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
			//锁定
		//sprintf((char*)p,"%.1f",(GPRS_Para.CurVal/10.0));
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		//目标温度上限
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[271]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
    //目标温度下限
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[270]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");
		//当前档位
		sprintf((char*)p,"%d",g_PduUART485[0].RegHold[315]);
		strcat((char*)pBuf,(char*)p);
		strcat((char*)pBuf,",");	
    //转火温度
		//sprintf((char*)p,"%d",g_PduUART485[0].RegHold[310]);
		strcat((char*)pBuf,"0");
		strcat((char*)pBuf,",");
		
 //计算出帧长度
    len = strlen((char*)pBuf);
		//填充帧长度
		pBuf[2] = (len%1000/100)+0x30;
		pBuf[3] = (len%100/10)+0x30;
		pBuf[4] = (len%10/1)+0x30;
	
	
		//发送数据
		SIM800C_Send((uint8_t*)pBuf,strlen(pBuf));
		
		myfree(SRAMIN,pBuf);
		myfree(SRAMIN,p);
  }
}

/*
*********************************************************************************************************
*	函 数 名: SIM800C_Task
*	功能说明: 
*	形    参: 
*	返 回 值: 无
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
	//必须等到SIM卡连接上
	do
	{
		SIM800C_HardPowerOff();
		BSP_OS_TimeDlyMs(200);
		SIM800C_HardPowerOn();
		BSP_OS_TimeDlyMs(1000);
		ret = SIM800C_CMD_Init();
	}while(ret == 0);
	//链接网络
	ret = SIM800C_ConnectNetwork(&NetPara);
	if(ret == 1)
	{
		//新加客户应用
		{
			GPRS_Para.gprs_rssi = NetPara.Rssi;
    }
		//链接正常
		NetPara.Connect_Sta = TRUE;
		//立刻发送心跳
	  Send_HeatBeatMsg(1000);	
  }
	else if(ret == TIMEOUT)
	{
		//链接失败
		NetPara.Connect_Sta = FALSE;
		goto reset;
  }
	else
	{
		//链接失败
		NetPara.Connect_Sta = FALSE;
  }
	
	//初始默认值
	NetPara.NetCheckTime = 2000;
	
	bsp_StartTimer(SIM800C_TMR_ID1, NetPara.NetCheckTime);//2秒钟判断一次sim800电源状态
	
	NetPara.NetCheckTime1 = 30*1000;
	bsp_StartTimer(SIM800C_TMR_CHECK_NET, NetPara.NetCheckTime1);//30秒检测一次网络状态
	
	
	while(1)
	{
		
		bsp_StartTimer(SIM800C_TMR_CHECK_RUN,5*60*1000);//用于判断5分钟都未执行该循环，则直接重启设备

/*********************************命令检测网络连接状态**********************************/
		//网络状态检测,命令行检测
		if(bsp_CheckTimer(SIM800C_TMR_CHECK_NET))//3分钟检测一次网络状态
		{			
#if SIM800C_TRANSPARENT//在头文件中宏定义为1
			//进入命令模式
	    SIM800C_SwitchCmdMode();//发送了+++模式切换
#endif	
      	
			if(SIM800C_Check_NetSta(1,1000) == 6)//判断联网是否正常	，ret == 6，为connect ok	
			{
				NetPara.NetCheckTime1 = 2*60*1000;
				NetPara.Connect_Sta   = TRUE;
#if SIM800C_TRANSPARENT
			 
			 SIM800C_SwitchDataMode();//进入数据模式，透传模式
#endif		
      }
			else
			{
				NetPara.Connect_Sta = FALSE;
				//5秒后重新；链接网络
				bsp_StartTimer(SIM800C_TMR_ID1, 2000);
				NetPara.NetCheckTime1 = 30*1000;
      }
			
			bsp_StartTimer(SIM800C_TMR_CHECK_NET, NetPara.NetCheckTime1);//30秒
		}
		
/*********************************电源检测，同时检测网络状态**********************************/    
    //电源状态检测		
		if(bsp_CheckTimer(SIM800C_TMR_ID1))//定义为2秒
		{
			bsp_StopTimer(SIM800C_TMR_ID1);
			//监测电源状态
			ret = SIM800C_Check_PowerSta();//stm32的PB5脚，连接到sim800c的42脚，PowerState
			if(ret == FALSE)
			{
				//重新启动模块
				goto reset;
			}
			
			//如果连接正常
			if(NetPara.Connect_Sta)//确认连接是在AT+CIPSTART发送后，返回connect ok
			{
				NetPara.NetCheckTime = 10000;//初始值为2秒，联网成功后改成10秒
			}
			//如果连接失败
			else
			{
				ret = SIM800C_ConnectNetwork(&NetPara);//链接网络
				if(ret == TRUE)
				{
					//链接正常
					NetPara.Connect_Sta = TRUE;
					//立刻发送心跳
					Send_HeatBeatMsg(1000);	
					//修改检测时间
					NetPara.NetCheckTime = 10000;//网络检测时间改成10秒
				}
				else if(ret == TIMEOUT)
				{
					//链接失败
					NetPara.Connect_Sta = FALSE;
					goto reset;
        }
				else
				{
					//链接失败
					NetPara.Connect_Sta = FALSE;
					NetPara.NetCheckTime = 5000;//网络检测时间改成5秒
				}
      }
 			//重新启动网络监测定时器
 			bsp_StartTimer(SIM800C_TMR_ID1, NetPara.NetCheckTime);
 		}

		//新加客户应用
		{
			GPRS_Para.gprs_rssi = NetPara.Rssi;//信号强度，SIM800C_ReadRSSI（）
		}
		
		//链接正常，则发送心跳信息
		if(NetPara.Connect_Sta)
		{
				//心跳信息发送，时间可配置
				Send_HeatBeatMsg(GPRS_Para.heartbeat_period*1000);
    }
		
		//超时成帧检查
		if(bsp_CheckTimer(SIM800C_UART))
		{
			bsp_StopTimer(SIM800C_UART);
			
			//分配一块动态内存
      pOld = (char*)mymalloc(SRAMIN,256);
			//判断内存申请是否正确
			if(pOld == NULL)
			{
				DBGU_Msg("Malloc Error!\r\n");	
				myfree(SRAMIN,pOld);
				break;
			}
			//动态内存清零
			memset((char*)pOld,0,256);

			//长度计数清零
			len = 0;
			//读空缓冲区
			while(comGetChar(SIM800C_UART, &ucData))
			{
				pOld[len++] = ucData;
				if(len >= 1024)
				{
					//不允许接收数据长度，大于分配的内存缓冲区
					DBGU_Msg("Recieve Len Overflow!\r\n");
					myfree(SRAMIN,pOld);
					break;
				}
			}
			//转换为字符串
			sprintf((char*)pOld,"%s",(char*)pOld);	
			
#if SIM800C_TRANSPARENT
			(void)pNew;
			
      if (strstr((char*)pOld, "\r\nCLOSED\r\n") != NULL)
			{
				//网络断开，等待重连网络
				//链接失败
				NetPara.Connect_Sta = FALSE;
			}	
			else  if (strstr((char*)pOld, "+CGREG: 1,0") != NULL)
			{
				//网络附着失败，重新连接
				NetPara.Connect_Sta = FALSE;
      }
			else if ((strstr((char*)pOld, "\r\nCONNECT\r\n") != NULL)||
				       (strstr((char*)pOld, "CONNECT OK")      != NULL))
			{
				//链接正常
				NetPara.Connect_Sta = TRUE;
			}	
			else
			{
				  cnt = 0;
					//确认地址
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
						//去掉帧头
						pOld += 10;
						len  -= 10;
						ret = GPRS_Slave((char*)pOld,len);
						if(ret == 0)
						{
							//确认地址不超标
							if((pOld[0] >= 1)&&(pOld[0] <= 16))
							{
								//存放到队列中，等待下发到485
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
			//判断接收到的是否为数据帧
			if (strstr((char*)pOld, "+IPD,") != NULL)
			{
					//搜索第一个出现“:"的位置
					pNew = strstr((char*)pOld, ":");
					//不要冒号
					pNew++;
				  
				  //modbus从机处理
				  ret = GPRS_Slave((char*)pNew,SIM800C_Recv_Len((uint8_t*)pOld));				
			}
			//判断是否有来电
			else if (strstr((char*)pOld, "+CLIP:") != NULL)
			{
				//挂断电话
				Shut_Down_Phone();
			}
			//判断是否有短信息
			else if (strstr((char*)pOld, "+CMTI:") != NULL)
			{
				//接收并处理短信
			}	
			//收到该消息表示网络断开
		  else if (strstr((char*)pOld, "CLOSED") != NULL)
			{
				//网络断开，等待重连网络
				//链接失败
				NetPara.Connect_Sta = FALSE;
			}	
#endif			
			//清空缓冲区
			comClearRxFifo(SIM800C_UART);	
			myfree(SRAMIN,pOld);
		}			

#if SIM800C_TRANSPARENT		
		//循环发送队列
		{
			static ElemType_G e;
			//首先判断队列中是否存在有效命令
			if(!GPRS_IsQueueEmpty(&g_GPRSQueue))
			{
				//获得队头元素
				GPRS_GetHead(&g_GPRSQueue,&e);
				//发送字符串
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
*	函 数 名: OS_TmrNetCheckCallback
*	功能说明: 操作系统定时器回调函数
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void OS_TmrNetCheckCallback(void *p_tmr, void *p_arg)			
{

}

