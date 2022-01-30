#include <string.h>
#include "modbus_485.h"
#include "crc.h"
#include "bsp.h"
#include "data.h"

uint8_t UART485RxDat[UART485_RXBUFLEN];	     
uint8_t UART485Sentdat[UART485_TXBUFLEN];	 

//485用modbus主机
UART485_PDU  g_PduUART485[UART485_SLAVE_CNT];
_MTCMD       g_MbMtCmd485;
UART485_PDU  g_PduUART485_bak[UART485_SLAVE_CNT];//用于判断参数变化，新增加

uint16_t     Sendlen = 0;

/**************************************************************
函数名:Para_Change_Check()
输入:
输出:
功能:判断参数变化
***************************************************************/
void Para_Change_Check(void)
{
	static uint32_t start_time = 0;
	static uint16_t interval = 3000;
				 uint8_t  sign = 0;
	sign = timing(start_time, interval);
	if(sign)
	{
		start_time = bsp_GetRunTime();
		if(	g_PduUART485[0].RegHold[69] != g_PduUART485_bak[0].RegHold[69] || //IA
				g_PduUART485[0].RegHold[70] != g_PduUART485_bak[0].RegHold[70] || //
				g_PduUART485[0].RegHold[71] != g_PduUART485_bak[0].RegHold[71] || //IB
				g_PduUART485[0].RegHold[72] != g_PduUART485_bak[0].RegHold[72] || //
				g_PduUART485[0].RegHold[73] != g_PduUART485_bak[0].RegHold[73] || //IC
				g_PduUART485[0].RegHold[74] != g_PduUART485_bak[0].RegHold[74])  
		{
				g_PduUART485_bak[0].RegHold[69] = g_PduUART485[0].RegHold[69];
				g_PduUART485_bak[0].RegHold[70] = g_PduUART485[0].RegHold[70];
				g_PduUART485_bak[0].RegHold[71] = g_PduUART485[0].RegHold[71];
				g_PduUART485_bak[0].RegHold[72] = g_PduUART485[0].RegHold[72];
				g_PduUART485_bak[0].RegHold[73] = g_PduUART485[0].RegHold[73];
				g_PduUART485_bak[0].RegHold[74] = g_PduUART485[0].RegHold[74];
				

				GPRS_Para.ParamChangeFlag |= BIT(0);		
				
		}
		else if(g_PduUART485[1].RegHold[69] != g_PduUART485_bak[1].RegHold[69] || 
				g_PduUART485[1].RegHold[70] != g_PduUART485_bak[1].RegHold[70] || 
				g_PduUART485[1].RegHold[71] != g_PduUART485_bak[1].RegHold[71] || 
				g_PduUART485[1].RegHold[72] != g_PduUART485_bak[1].RegHold[72] || 
				g_PduUART485[1].RegHold[73] != g_PduUART485_bak[1].RegHold[73] || 
				g_PduUART485[1].RegHold[74] != g_PduUART485_bak[1].RegHold[74] || 
				g_PduUART485[1].RegHold[75] != g_PduUART485_bak[1].RegHold[75])   
		{
				g_PduUART485_bak[1].RegHold[69] = g_PduUART485[1].RegHold[69];
				g_PduUART485_bak[1].RegHold[70] = g_PduUART485[1].RegHold[70];
				g_PduUART485_bak[1].RegHold[71] = g_PduUART485[1].RegHold[71];
				g_PduUART485_bak[1].RegHold[72] = g_PduUART485[1].RegHold[72];
				g_PduUART485_bak[1].RegHold[73] = g_PduUART485[1].RegHold[73];
				g_PduUART485_bak[1].RegHold[74] = g_PduUART485[1].RegHold[74];
				
				GPRS_Para.ParamChangeFlag |= BIT(1);			
		}
		else if(g_PduUART485[2].RegHold[69] != g_PduUART485_bak[2].RegHold[69] || 
				g_PduUART485[2].RegHold[70] != g_PduUART485_bak[2].RegHold[70] || 
				g_PduUART485[2].RegHold[71] != g_PduUART485_bak[2].RegHold[71] || 
				g_PduUART485[2].RegHold[72] != g_PduUART485_bak[2].RegHold[72] || 
				g_PduUART485[2].RegHold[73] != g_PduUART485_bak[2].RegHold[73] || 
				g_PduUART485[2].RegHold[74] != g_PduUART485_bak[2].RegHold[74] || 
				g_PduUART485[2].RegHold[75] != g_PduUART485_bak[2].RegHold[75])   
		{
				g_PduUART485_bak[2].RegHold[69] = g_PduUART485[2].RegHold[69];
				g_PduUART485_bak[2].RegHold[70] = g_PduUART485[2].RegHold[70];
				g_PduUART485_bak[2].RegHold[71] = g_PduUART485[2].RegHold[71];
				g_PduUART485_bak[2].RegHold[72] = g_PduUART485[2].RegHold[72];
				g_PduUART485_bak[2].RegHold[73] = g_PduUART485[2].RegHold[73];
				g_PduUART485_bak[2].RegHold[74] = g_PduUART485[2].RegHold[74];
				
				GPRS_Para.ParamChangeFlag |= BIT(2);			
		}
		
		else if(g_PduUART485[3].RegHold[69] != g_PduUART485_bak[3].RegHold[69] || 
				g_PduUART485[3].RegHold[70] != g_PduUART485_bak[3].RegHold[70] || 
				g_PduUART485[3].RegHold[71] != g_PduUART485_bak[3].RegHold[71] || 
				g_PduUART485[3].RegHold[72] != g_PduUART485_bak[3].RegHold[72] || 
				g_PduUART485[3].RegHold[73] != g_PduUART485_bak[3].RegHold[73] || 
				g_PduUART485[3].RegHold[74] != g_PduUART485_bak[3].RegHold[74] || 
				g_PduUART485[3].RegHold[75] != g_PduUART485_bak[3].RegHold[75])   
		{
				g_PduUART485_bak[3].RegHold[69] = g_PduUART485[3].RegHold[69];
				g_PduUART485_bak[3].RegHold[70] = g_PduUART485[3].RegHold[70];
				g_PduUART485_bak[3].RegHold[71] = g_PduUART485[3].RegHold[71];
				g_PduUART485_bak[3].RegHold[72] = g_PduUART485[3].RegHold[72];
				g_PduUART485_bak[3].RegHold[73] = g_PduUART485[3].RegHold[73];
				g_PduUART485_bak[3].RegHold[74] = g_PduUART485[3].RegHold[74];
				
				GPRS_Para.ParamChangeFlag |= BIT(3);			
		}
	}
}

/**************************************************************
函数名:Uart485_Task()
输入:
输出:
功能:485任务
***************************************************************/
void Uart485_Task(void)
{
	uint8_t    ret = 0;
	uint32_t   addr = 0;
	uint8_t    i = 0;
	
	static uint32_t start_time = 0;
	       uint8_t  sign = 0;

	ElemType e;
	
	//服务器未下发数据，则，485主机轮询
		if(GPRS_Para.DataFlag == 0)
		{
			ret = UART485_Master(&addr);
	
			//此句判断，主要是为了，完成上一命令
			if(ret != 0)
			{
				//读取超时，说明断线
				if(ret == 0xff)
				{
					GPRS_Para.OnlineFlag &=~BIT((addr-1));
					//清除数据变量
					memset(&g_PduUART485[(addr-1)].RegHold[100],0,20);
				}
				//正常
				else if((ret >= 1)&&(ret <= 16))
				{
					GPRS_Para.OnlineFlag |= BIT((addr-1));
				}
				
				//判断透传队列是否为空
				if(!T_IsQueueEmpty(&Trans_Queue))
				{
					//不为空，则说明有数据
					GPRS_Para.DataFlag = 1;
				}
			}
  }
	//服务器有数据待下发，则，透传到板子
	else if(GPRS_Para.DataFlag == 1)
	{
		if(!T_IsQueueEmpty(&Trans_Queue))
		{
			T_DeQueue(&Trans_Queue,&e);
			
			UART485_MbMt_req(e,(UART485_PDU *)&g_PduGprs);// g_PduUART485[e.slave-1]
			
			//发送后等待2秒，看是否应答
			GPRS_Para.DataFlag = 2;
			bsp_StartTimer(TIMER_485, 5000);
    }
		else
		{
			GPRS_Para.DataFlag = 0;
    }
  }
	else if(GPRS_Para.DataFlag == 2)
	{
		if(bsp_CheckTimer(TIMER_485))
		{
			bsp_StopTimer(TIMER_485);
			//继续判断队列中是否有数据
			GPRS_Para.DataFlag = 1;
		}
		
		//超时成帧
		if(bsp_CheckTimer(UART_485))
		{
			bsp_StopTimer(UART_485);
			GPRS_Para.DataFlag = 1;

			comClearRxFifo(UART_485);
		}			
  }
}
/**************************************************************
函数名:ModbusTask()
输入:
输出:
功能:modbus任务
***************************************************************/
void ModbusTask(void)
{
	uint32_t addr = 0;
	uint8_t  i = 0;
	static uint32_t start_time = 0;
	       uint8_t  sign = 0;

	UART485_Master(&addr);
	for(i=0;i<=19;i++)
	{
		g_PduUART485[0].RegHold[i] =  GPRS_Para.sICCID[i];
		
 	}
	
	g_PduUART485[0].RegHold[20] = 1;
	g_PduUART485[0].RegHold[21] = (GPRS_Para.Air208RunSta == 2)? 1: 0;
	
	sign = timing(start_time,1000);
	if(sign)
	{
		start_time = bsp_GetRunTime();
		g_PduUART485[0].RegHold[22]++;
		if(g_PduUART485[0].RegHold[22] >= 1000)
		{
			g_PduUART485[0].RegHold[22] = 0;
		}

	}
// 	g_PduUART485[0].RegHold[22]++;
// 	if(g_PduUART485[0].RegHold[22] >= 1000)
// 	{
// 		g_PduUART485[0].RegHold[22] = 0;
// 	}
}



/**************************************************************
函数名:UART485_Init()
输入:
输出:
功能:modbus初始化
***************************************************************/
void UART485_Init(void)
{	
	ElemType e;
	uint8_t  i = 0;
	//初始化循环队列
  InitQueue(&g_M485Cmd);
	//初始化透传队列
	T_InitQueue(&Trans_Queue);
	
	g_MbMtCmd485.rep        = FALSE;                  //未应答
	g_MbMtCmd485.PLCCOMFlag = TRUE;		                //通讯打开

		
	for(i=1;i<=GPRS_Para.MaxSlaveCnt;i++)
	{	
		e.slave        = i;
		e.func         = MB_REGHOLD_RD;
		e.addr         = 69;	 	                        //保持寄存器的首地址，显示缓冲区
		e.cnt          = 6;                             //根据从机个数确定写入的寄存器个数
		e.startTimeout = bsp_GetRunTime();
		e.timeout      = 1000;	                        //命令 超时时间
		e.startPeriod  = bsp_GetRunTime();
		e.period       = 5*1000;	                      //命令发送周期
		e.commCnt      = PERIOD_CNT;                    //不可删除命令
		e.timeoutcnt   = 0;		                          //错误计数清零
		EnQueue(&g_M485Cmd, e);		

		e.slave        = i;
		e.func         = MB_REGHOLD_RD;
		e.addr         = 57;	 	                        //保持寄存器的首地址，显示缓冲区
		e.cnt          = 6;                             //根据从机个数确定写入的寄存器个数
		e.startTimeout = bsp_GetRunTime();
		e.timeout      = 1000;	                        //命令 超时时间
		e.startPeriod  = bsp_GetRunTime();
		e.period       = 5*1000;	                      //命令发送周期
		e.commCnt      = PERIOD_CNT;                    //不可删除命令
		e.timeoutcnt   = 0;		                          //错误计数清零
		EnQueue(&g_M485Cmd, e);
		
		
		
// 		e.slave        = i;
// 		e.func         = MB_BITCOIL_RD;                 //读取线圈
// 		e.addr         = 10;	 	                        //保持寄存器的首地址，显示缓冲区
// 		e.cnt          = 10;                             //根据从机个数确定写入的寄存器个数
// 		e.startTimeout = bsp_GetRunTime();
// 		e.timeout      = 1000;	                        //命令 超时时间
// 		e.startPeriod  = bsp_GetRunTime();
// 		e.period       = 5*1000;	                      //命令发送周期
// 		e.commCnt      = PERIOD_CNT;                    //不可删除命令
// 		e.timeoutcnt   = 0;		                          //错误计数清零
// 		EnQueue(&g_M485Cmd, e);	
		
  }
	
	//清空PDU
	for(i=0;i<UART485_SLAVE_CNT;i++)
	{
		memset((INT16U*)&g_PduUART485[i],0,sizeof(UART485_PDU));
  }
}

/**************************************************************
函数名:UART485_Master()
输入:
输出:
功能:modbus主机
***************************************************************/
uint8_t UART485_Master(uint32_t *addr)
{	
	ElemType e;
	uint8_t  sign = 0;
	uint16_t ret = 0;
	uint16_t i =0;
	
	//首先判断队列中是否存在有效命令
	if(!IsQueueEmpty(&g_M485Cmd))
	{
		//获得队头元素
		GetHead(&g_M485Cmd,&e);
		
		if(g_MbMtCmd485.rep==FALSE)
		{
			sign = timing(e.startPeriod,e.period);
			if(sign)												
			{	
				UART485_MbMt_req(e,&g_PduUART485[e.slave-1]);								
				e.startTimeout = bsp_GetRunTime();		
				e.startPeriod  = bsp_GetRunTime();	
				//修改队头元素的相关变量
				SetHead(&g_M485Cmd,e);		

				g_MbMtCmd485.rep=TRUE;					
			}
			else
			{
				//先出队，再入队，将该条命令放在后面
				DeQueue(&g_M485Cmd,&e);	
				EnQueue(&g_M485Cmd,e);
				BSP_OS_TimeDlyMs(100);
				//现在无正在传输的命令
				return 0xfe;
			}				
		}
		else
		{
			sign = timing(e.startTimeout,e.timeout);
			if(sign)											
			{
				g_MbMtCmd485.rep=FALSE;	

				//修改指定命令的超时计数
				e.timeoutcnt++;	
				//修改队头元素的相关变量
				SetHead(&g_M485Cmd,e);	
				
				if(e.timeoutcnt >= 3)					   
				{
						e.timeoutcnt = 0;
						//修改队头元素的相关变量
				    SetHead(&g_M485Cmd,e);	
						DeQueue(&g_M485Cmd,&e);	
						EnQueue(&g_M485Cmd,e);
						BSP_OS_TimeDlyMs(100);
	//				g_MbMtCmd.PLCCOMFlag=OFF;	
            *addr = e.slave;					
						return 0xff;								
				}
			}													    
			else												
			{
					//超时成帧检查,485串口
					if(bsp_CheckTimer(UART_485))
					{
						bsp_StopTimer(UART_485);

						//复制到modbus消息处理缓冲区中
						memcpy((char*)UART485RxDat,(char*)UART485_RXBUF,UART485_tUART.usRxCount);
						
						ret = UART485_MbMt_rep(e,&g_PduUART485[e.slave-1]);					      

						//超时计数清零s
						e.timeoutcnt=0;	
						//修改队头元素的相关变量
				    SetHead(&g_M485Cmd,e);
						
						g_MbMtCmd485.rep=FALSE;						   

						//通讯次数变更
						if((e.commCnt != PERIOD_CNT)&&(e.commCnt != 0))
						{
							e.commCnt--;
							//修改队头元素的相关变量
							SetHead(&g_M485Cmd,e);		
						}
						
						//通讯次数为0时，删除该条命令
						if(e.commCnt == 0)    
						{
							//删除当前命令
							DeQueue(&g_M485Cmd,&e);						
						}
						
						//清空缓冲区
						comClearRxFifo(UART_485);	
						
						//复制
// 						for(i=0;i<MAX3_REGHOLD_D16;i++)
// 						{
// 							g_PduGprs.RegHold[i] = g_PduUART485[0].RegHold[i];
//             }
// 						if(g_PduUART485[0].RegHold[271] == 0)
// 						{
// 							i = 0;
// 						}
						
						*addr = e.slave;
						return ret;
					}
			}
		}
	}
	else
	{
		g_MbMtCmd485.PLCCOMFlag=FALSE;
		if(g_MbMtCmd485.PLCCOMFlag==FALSE) return 0xff;
	}
	return 0;	
}


/**************************************************************
函数名:UART485_Slave()
输入:
输出:
功能:485接口modbus从机
***************************************************************/
uint16_t UART485_Slave(void)
{
	_CMD     Cmd;
	uint16_t ret = 0;

	//当作为从机时,固定为1，组态屏为主站
	Cmd.slave = 1;
	
	//超时成帧检查,485串口
	if(bsp_CheckTimer(UART_485))
	{
		bsp_StopTimer(UART_485);

	  //复制到modbus消息处理缓冲区中
	  memcpy((char*)UART485RxDat,(char*)UART485_RXBUF,UART485_tUART.usRxCount);
		
		//寄存器地址
		Cmd.addr = ((uint16_t)UART485RxDat[2]<<8)|UART485RxDat[3];
		//更新待读取的信息
		UART485_Read();
		
		//modbus从机处理,始终采用结构0，作为从机数据结构
		ret = UART485_MbSl_rep(Cmd,&g_PduUART485[0]);
		if(ret == MB_REGHOLD_RD)
		{
		}
		else if(ret == MB_BITINP_RD)
		{
		}
		else if((ret == MB_BITCOIL_WT)||(ret == MB_MULTICOIL_WT))
		{
		}
		else if((ret == MB_REGHOLD_WT)||(ret == MB_MULTIREG_WT))
		{
				UART485_Write();
		}
		
		//清空缓冲区
		comClearRxFifo(UART_485);
		
		//485应答延时定时器
		bsp_StartTimer(TIMER_485, 10);
		return ret;	
  }
	
	//485延时发送处理部分
	if(bsp_CheckTimer(TIMER_485))
	{
		bsp_StopTimer(TIMER_485);
		
		//延时应答
		comSendBuf(UART_485, UART485Sentdat, Sendlen);
		memset((char*)UART485Sentdat,0,UART485_TXBUFLEN);
  }
	return 0;
}

/**************************************************************
函数名:UART485_MbMt_req()
输入:
输出:
功能:modbus主机请求
***************************************************************/
uint16_t UART485_MbMt_req(_CMD Cmd,UART485_PDU *Pdu)
{
	uint8_t ID_Code,FuncCode,chr;
	uint16_t SbytAddr,Sshft,EAddr,Eshft;
	uint16_t Tcnt;
	uint32_t i,m;
	uDAT16 D16;
	uDAT32 D32;

	memset(UART485Sentdat,0,UART485_RXBUFLEN);
	Tcnt=0;						
	ID_Code=Cmd.slave;               
	UART485Sentdat[Tcnt++]=ID_Code;
	FuncCode=Cmd.func;                
	UART485Sentdat[Tcnt++]=FuncCode;
	
	switch(FuncCode)
	{
			case MB_BITCOIL_RD:
				D16.d16=Cmd.addr+UART485_BITCOIL_BADDR;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_BITINP_RD:
				D16.d16=Cmd.addr+UART485_BITINP_BADDR;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_REGHOLD_RD:
				D16.d16=Cmd.addr+UART485_REGHOLD_BADDR;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_REGINP_RD:
				D16.d16=Cmd.addr+UART485_REGINP_BADDR;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_BITCOIL_WT:
				D16.d16=Cmd.addr+UART485_BITCOIL_BADDR;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				SbytAddr=Cmd.addr/D16BITNUM;
				Sshft=Cmd.addr%D16BITNUM;
				D16.d16=0xFFFF;
				if(Pdu->BitCoil[SbytAddr]&BIT(Sshft))
				{
					D16.d16=0xFF00; 
				}
				else
				{
					D16.d16=0x0000; 
				}
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_REGHOLD_WT:
				D16.d16=Cmd.addr+UART485_REGHOLD_BADDR;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				//D16.d16=Counts;
				D16.d16=Pdu->RegHold[Cmd.addr];
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_MULTICOIL_WT:
				D16.d16=Cmd.addr+UART485_BITCOIL_BADDR;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				if((Cmd.cnt%8)==0)				
				{
					UART485Sentdat[Tcnt++]=(Cmd.cnt>>3);
				}
				else
				{
					UART485Sentdat[Tcnt++]=(Cmd.cnt>>3)+1;
				}
				
				EAddr=Cmd.addr+Cmd.cnt;
				
				SbytAddr=Cmd.addr/D16BITNUM;
				Sshft=Cmd.addr%D16BITNUM;
				
				Eshft=EAddr%D8BITNUM;
				for(i=SbytAddr,m=Cmd.addr;m<EAddr;i++)
				{
					D32.d16[0].d16=Pdu->BitCoil[i];
					D32.d16[1].d16=Pdu->BitCoil[i+1];
					D32.d32=D32.d32>>Sshft;
					UART485Sentdat[Tcnt++]=D32.d16[0].d8.Hi;
					m+=8;
					if(m>=EAddr) break;
					UART485Sentdat[Tcnt++]=D32.d16[0].d8.Lo;
					m+=8;
					if(m>=EAddr) break;
				}
				if(Eshft!=0)
				{
					chr=(UART485Sentdat[Tcnt-1]&0xFF)<<(D8BITNUM-Eshft);
					chr=(chr&0xFF)>>(D8BITNUM-Eshft);
					UART485Sentdat[Tcnt-1]=chr;
				}
				break;

			case MB_MULTIREG_WT:
				D16.d16=Cmd.addr+UART485_REGHOLD_BADDR;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				UART485Sentdat[Tcnt++]=Cmd.cnt*2;

				EAddr=Cmd.addr+Cmd.cnt;

				for(i=Cmd.addr;i<EAddr;i++)
				{
					D16.d16=Pdu->RegHold[i];
					UART485Sentdat[Tcnt++]=D16.d8.Hi;
					UART485Sentdat[Tcnt++]=D16.d8.Lo;
				}
				break;
			default:
				break;
	}
	D16.d16=calCRC16_modbus(UART485Sentdat,Tcnt);
	UART485Sentdat[Tcnt++]=D16.d8.Hi;
	UART485Sentdat[Tcnt++]=D16.d8.Lo;

	UART485_SEND(UART485Sentdat,Tcnt)
	
	return FuncCode;
}

/**************************************************************
函数名:UART485_MbMt_rep()
输入:
输出:
功能:modbus主机响应
***************************************************************/
uint16_t UART485_MbMt_rep(_CMD Cmd,UART485_PDU *Pdu)
{
	uint8_t  ID_Code,FuncCode,ByteCnt;
	uint16_t SAddr,SbytAddr,Sshft,EAddr,CRCValue;
	uint16_t Rcnt,Counts;
	uint32_t i,j,m,n,tmp1;
	uDAT16   D16;

	Rcnt=0;
	
	ID_Code=UART485RxDat[Rcnt++];
	if(ID_Code!=Cmd.slave)       
	{
		return 1;
	}
	
	FuncCode=UART485RxDat[Rcnt++];
	if(FuncCode!=Cmd.func)
	{
		return 1;
	}
	switch(FuncCode)
	{
			case MB_BITCOIL_RD:
				ByteCnt=UART485RxDat[Rcnt++];
				Counts=Cmd.cnt;
				if((Counts%8)==0)						
				{
					if(ByteCnt!=(Counts>>3))
					{
						return 1;
					}
				}
				else
				{
					if(ByteCnt!=((Counts>>3)+1))
					{
						return 1;
					}
				}
				SAddr=Cmd.addr;
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;
				EAddr=SAddr+Counts;

				
				for(i=SbytAddr,j=Sshft,n=D8BITNUM,m=SAddr;m<EAddr;j++,n++,m++)
				{
					if(n>=D8BITNUM)
					{
						n=0;
						D16.d16=UART485RxDat[Rcnt++];
					}
					tmp1=D16.d16>>n;
					if(j>=D16BITNUM)
					{
						i++;
						j=0;
					}
					if(tmp1&0x01)
					{
						Pdu->BitCoil[i]|=BIT(j);
					}
					else
					{
						Pdu->BitCoil[i]&=~ BIT(j);
					}
				}
				break;

			case MB_BITINP_RD:
				ByteCnt=UART485RxDat[Rcnt++];
				Counts=Cmd.cnt;
				if((Counts%8)==0)					
				{
					if(ByteCnt!=(Counts>>3))
					{
						return 1;
					}
				}
				else
				{
					if(ByteCnt!=((Counts>>3)+1))
					{
						return 1;
					}
				}
				SAddr=Cmd.addr;
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;
				EAddr=SAddr+Counts;

				
				for(i=SbytAddr,j=Sshft,n=D8BITNUM,m=SAddr;m<EAddr;j++,n++,m++)
				{
					if(n>=D8BITNUM)
					{
						n=0;
						D16.d16=UART485RxDat[Rcnt++];
					}
					tmp1=D16.d16>>n;
					if(j>=D16BITNUM)
					{
						i++;
						j=0;
					}
					if(tmp1&0x01)
					{
						Pdu->BitInp[i]|=BIT(j);
					}
					else
					{
						Pdu->BitInp[i]&=~ BIT(j);
					}
				}
				break;

			case MB_REGHOLD_RD:
				ByteCnt=UART485RxDat[Rcnt++];
				Counts=Cmd.cnt*2;
				if(ByteCnt!=Counts)		
				{
					return 1;
				}
				SAddr=Cmd.addr;
				
				for(m=0,i=SAddr;m<ByteCnt;i++)
				{
					D16.d8.Hi=UART485RxDat[Rcnt++];
					m++;
					if(m<ByteCnt)	D16.d8.Lo=UART485RxDat[Rcnt++];
					else			    D16.d8.Lo=Pdu->RegHold[i];
					m++;
					Pdu->RegHold[i]=D16.d16;
				}
				break;

			case MB_REGINP_RD:		
				ByteCnt=UART485RxDat[Rcnt++];
				Counts=Cmd.cnt*2;
				if(ByteCnt!=Counts)
				{
					return 1;
				}
				SAddr=Cmd.addr;
				
				for(m=0,i=SAddr;m<ByteCnt;i++)
				{
					D16.d8.Hi=UART485RxDat[Rcnt++];
					m++;
					if(m<ByteCnt)	D16.d8.Lo=UART485RxDat[Rcnt++];
					else			    D16.d8.Lo=Pdu->RegInp[i];
					m++;
					Pdu->RegInp[i]=D16.d16;
				}
				break;

			case MB_BITCOIL_WT:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16-UART485_BITCOIL_BADDR;
				if(SAddr!=Cmd.addr)		
				{
					return 1;
				}
				
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				if(D16.d16!=Cmd.cnt)
				{
					return 1;
				}
				break;

			case MB_REGHOLD_WT:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16-UART485_REGHOLD_BADDR;
				if(SAddr!=Cmd.addr)		
				{
					return 1;
				}
				
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				break;

			case MB_MULTICOIL_WT:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16-UART485_BITCOIL_BADDR;
				if(SAddr!=Cmd.addr)			
				{
					return 1;
				}
				
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				if(D16.d16!=Cmd.cnt)
				{
					return 1;
				}
				break;

			case MB_MULTIREG_WT:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16-UART485_REGHOLD_BADDR;
				if(SAddr!=Cmd.addr)	
				{
					return 1;
				}
				
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				if(D16.d16!=Cmd.cnt)
				{
					return 1;
				}
				break;
			default:
				return FuncCode;
	}
	CRCValue=calCRC16_modbus(UART485RxDat, Rcnt);	
	D16.d8.Hi=UART485RxDat[Rcnt++];
	D16.d8.Lo=UART485RxDat[Rcnt++];
	if(CRCValue!=D16.d16)		
	{
	   return 1;
	}
	return FuncCode;	
}

/**************************************************************
函数名:UART485_MbSl_rep()
输入:
输出:
功能:modbus从机响应
***************************************************************/
uint16_t UART485_MbSl_rep(_CMD Cmd,UART485_PDU *Pdu)
{
	uint8_t    ID_Code,FuncCode,ErrCode,chr;
	uint16_t   SAddr,SbytAddr,Sshft,EAddr,Eshft,ByteCnt,CRCValue;
	uint16_t   Rcnt,Tcnt,Counts;
	uint32_t   i,j,m,n,tmp1,tmp2;
	uDAT16   	 D16;
	uDAT32   	 D32;

  memset(UART485Sentdat,0,UART485_TXBUFLEN);
	Rcnt=Tcnt=0;
	ErrCode=0;
	
	ID_Code=UART485RxDat[Rcnt++];
	//广播地址为0xff
	if((ID_Code!=Cmd.slave)&&(ID_Code != 0xff))	return 0;
	else				UART485Sentdat[Tcnt++]=ID_Code;
	
	FuncCode=UART485RxDat[Rcnt++];
	UART485Sentdat[Tcnt++]=FuncCode;                 
	
	switch(FuncCode)
	{
			case MB_BITCOIL_RD:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX3_BITCOIL)		 
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_BITCOIL_RD)	
				{
					ErrCode=0x03;
					break;
				}
				tmp1=Counts>>3;
				tmp2=Counts-(tmp1<<3);
				if(tmp2==0)
				{
					UART485Sentdat[Tcnt++]=tmp1;
				}
				else
				{
					UART485Sentdat[Tcnt++]=tmp1+1;
				}
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX3_BITCOIL)				
				{
					ErrCode=0x02;
					break;
				}
				
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;

				Eshft=EAddr%D8BITNUM;
				for(i=SbytAddr,m=SAddr;m<EAddr;i++)
				{
					D32.d16[0].d16=Pdu->BitCoil[i];
					D32.d16[1].d16=Pdu->BitCoil[i+1];
					D32.d32=D32.d32>>Sshft;
					UART485Sentdat[Tcnt++]=D32.d16[0].d8.Lo;
					m+=8;
					if(m>=EAddr) break;
					UART485Sentdat[Tcnt++]=D32.d16[0].d8.Hi;
					m+=8;
					if(m>=EAddr) break;
				}

				chr=(UART485Sentdat[Tcnt-1]&0xFF)<<(D8BITNUM-Eshft);
				chr=(chr&0xFF)>>(D8BITNUM-Eshft);
				UART485Sentdat[Tcnt-1]=chr;
				break;

			case MB_BITINP_RD:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX3_BITINP)			
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_BITINP_RD)		
				{
					ErrCode=0x03;
					break;
				}
				tmp1=Counts>>3;
				tmp2=Counts-(tmp1<<3);
				if(tmp2==0)
				{
					UART485Sentdat[Tcnt++]=tmp1;
				}
				else
				{
					UART485Sentdat[Tcnt++]=tmp1+1;
				}
				EAddr=SAddr+Counts;
				if(EAddr>MAX3_BITINP)					
				{
					ErrCode=0x02;
					break;
				}
				
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;
				
				
				Eshft=EAddr%D8BITNUM;
				for(i=SbytAddr,m=SAddr;m<EAddr;i++)
				{
					D32.d16[0].d16=Pdu->BitInp[i];
					D32.d16[1].d16=Pdu->BitInp[i+1];
					D32.d32=D32.d32>>Sshft;
					UART485Sentdat[Tcnt++]=D32.d16[0].d8.Lo;
					m+=8;
					if(m>=EAddr) break;
					UART485Sentdat[Tcnt++]=D32.d16[0].d8.Hi;
					m+=8;
					if(m>=EAddr) break;
				}
				chr=(UART485Sentdat[Tcnt-1]&0xFF)<<(D8BITNUM-Eshft);
				chr=(chr&0xFF)>>(D8BITNUM-Eshft);
				UART485Sentdat[Tcnt-1]=chr;
				break;
			case MB_REGHOLD_RD:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX3_REGHOLD)					  
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGHOLD_RD)		
				{
					ErrCode=0x03;
					break;
				}
				UART485Sentdat[Tcnt++]=Counts*2;		
				EAddr=SAddr+Counts;
				if(EAddr>MAX3_REGHOLD)				
				{
					ErrCode=0x02;
					break;
				}
				for(i=SAddr;i<EAddr;i++)
				{
					D16.d16=Pdu->RegHold[i];
				  UART485Sentdat[Tcnt++]=D16.d8.Hi;
					UART485Sentdat[Tcnt++]=D16.d8.Lo;
				}
				break;
			case MB_REGINP_RD:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX3_REGINP)				
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGINP_RD)		
				{
					ErrCode=0x03;
					break;
				}
				UART485Sentdat[Tcnt++]=Counts*2;				
				EAddr=SAddr+Counts;
				if(EAddr>MAX3_REGINP)			
				{
					ErrCode=0x02;
					break;
				}
				for(i=SAddr;i<EAddr;i++)
				{
					D16.d16=Pdu->RegInp[i];
				  UART485Sentdat[Tcnt++]=D16.d8.Hi;
					UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				}
				break;
			case MB_BITCOIL_WT:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX3_BITCOIL)				
				{
					ErrCode=0x02;
					break;
				}
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;

				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				if(D16.d16==MB_OFF)
				{
					Pdu->BitCoil[SbytAddr]&=~ BIT(Sshft);
				}
				else if(D16.d16==MB_ON)
				{
					Pdu->BitCoil[SbytAddr]|=BIT(Sshft);
				}
				else	
				{
					ErrCode=0x03;
					break;
				}
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				break;
			case MB_REGHOLD_WT:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX3_REGHOLD)				
				{
					ErrCode=0x02;
					break;
				}
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				Pdu->RegHold[SAddr]=D16.d16;

				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				break;
			case MB_MULTICOIL_WT:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX3_BITCOIL)					
				{
					ErrCode=0x02;
					break;
				}
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_BITCOIL_WT)			
				{
					ErrCode=0x03;
					break;
				}
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX3_BITCOIL)				
				{
					ErrCode=0x02;
					break;
				}
				
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;
				
				Eshft=EAddr%D8BITNUM;

				ByteCnt=UART485RxDat[Rcnt++];
				if((Counts%8)==0)						
				{
					if(ByteCnt!=(Counts>>3))

					{
						ErrCode=0x03;
						break;
					}
				}
				else
				{
					if(ByteCnt!=((Counts>>3)+1))
					{
						ErrCode=0x03;
						break;
					}
				}
				for(i=SbytAddr,j=Sshft,n=D8BITNUM,m=SAddr;m<EAddr;j++,n++,m++)
				{
					if(n>=D8BITNUM)
					{
						n=0;
						D16.d16=UART485RxDat[Rcnt++];
					}
					tmp1=D16.d16>>n;
					if(j>=D16BITNUM)
					{
						i++;
						j=0;
					}
					if(tmp1&0x01)
					{
						Pdu->BitCoil[i]|=BIT(j);
					}
					else
					{
						Pdu->BitCoil[i]&=~ BIT(j);
					}
				}
				break;
			case MB_MULTIREG_WT:
				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX3_REGHOLD)					
				{
					ErrCode=0x02;
					break;
				}
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;

				D16.d8.Hi=UART485RxDat[Rcnt++];
				D16.d8.Lo=UART485RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGHOLD_WT)		
				{
					ErrCode=0x03;
					break;
				}
				UART485Sentdat[Tcnt++]=D16.d8.Hi;
				UART485Sentdat[Tcnt++]=D16.d8.Lo;
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX3_REGHOLD)			
				{
					ErrCode=0x02;
					break;
				}
				
				ByteCnt=UART485RxDat[Rcnt++];
				if(ByteCnt!=Counts*2)					
				{
					ErrCode=0x03;
					break;
				}			
				for(i=SAddr;i<EAddr;i++)
				{
				  D16.d8.Hi=UART485RxDat[Rcnt++];
					D16.d8.Lo=UART485RxDat[Rcnt++];
					Pdu->RegHold[i]=D16.d16;
				}
				break;
			  default:
					ErrCode=MB_ERR_FUNCODE;
				break;
	}
	CRCValue=calCRC16_modbus(UART485RxDat, Rcnt);	
	D16.d8.Hi=UART485RxDat[Rcnt++];
	D16.d8.Lo=UART485RxDat[Rcnt++];
	memset(UART485RxDat,0,Rcnt);
	if(CRCValue!=D16.d16)				
	{
	   return 0;
	}
		
	if(ErrCode!=0)	                  
	{
		UART485Sentdat[1]+=0x80;
		UART485Sentdat[Tcnt++]=ErrCode;
	}
	D16.d16=calCRC16_modbus(UART485Sentdat,Tcnt);
	UART485Sentdat[Tcnt++]=D16.d8.Hi;
	UART485Sentdat[Tcnt++]=D16.d8.Lo;
	
	UART485_SEND(UART485Sentdat,Tcnt)
	
	if(ErrCode!=0)	                   
	{
	  return 0;
	}
	else
	{
	 return FuncCode;
	}	
}
