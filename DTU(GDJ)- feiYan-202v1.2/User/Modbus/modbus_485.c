#include <string.h>
#include "modbus_485.h"
#include "crc.h"
#include "bsp.h"
#include "data.h"

uint8_t UART485RxDat[UART485_RXBUFLEN];	     
uint8_t UART485Sentdat[UART485_TXBUFLEN];	 

//485��modbus����
UART485_PDU  g_PduUART485[UART485_SLAVE_CNT];
_MTCMD       g_MbMtCmd485;
UART485_PDU  g_PduUART485_bak[UART485_SLAVE_CNT];//�����жϲ����仯��������

uint16_t     Sendlen = 0;

/**************************************************************
������:Para_Change_Check()
����:
���:
����:�жϲ����仯
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
������:Uart485_Task()
����:
���:
����:485����
***************************************************************/
void Uart485_Task(void)
{
	uint8_t    ret = 0;
	uint32_t   addr = 0;
	uint8_t    i = 0;
	
	static uint32_t start_time = 0;
	       uint8_t  sign = 0;

	ElemType e;
	
	//������δ�·����ݣ���485������ѯ
		if(GPRS_Para.DataFlag == 0)
		{
			ret = UART485_Master(&addr);
	
			//�˾��жϣ���Ҫ��Ϊ�ˣ������һ����
			if(ret != 0)
			{
				//��ȡ��ʱ��˵������
				if(ret == 0xff)
				{
					GPRS_Para.OnlineFlag &=~BIT((addr-1));
					//������ݱ���
					memset(&g_PduUART485[(addr-1)].RegHold[100],0,20);
				}
				//����
				else if((ret >= 1)&&(ret <= 16))
				{
					GPRS_Para.OnlineFlag |= BIT((addr-1));
				}
				
				//�ж�͸�������Ƿ�Ϊ��
				if(!T_IsQueueEmpty(&Trans_Queue))
				{
					//��Ϊ�գ���˵��������
					GPRS_Para.DataFlag = 1;
				}
			}
  }
	//�����������ݴ��·�����͸��������
	else if(GPRS_Para.DataFlag == 1)
	{
		if(!T_IsQueueEmpty(&Trans_Queue))
		{
			T_DeQueue(&Trans_Queue,&e);
			
			UART485_MbMt_req(e,(UART485_PDU *)&g_PduGprs);// g_PduUART485[e.slave-1]
			
			//���ͺ�ȴ�2�룬���Ƿ�Ӧ��
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
			//�����ж϶������Ƿ�������
			GPRS_Para.DataFlag = 1;
		}
		
		//��ʱ��֡
		if(bsp_CheckTimer(UART_485))
		{
			bsp_StopTimer(UART_485);
			GPRS_Para.DataFlag = 1;

			comClearRxFifo(UART_485);
		}			
  }
}
/**************************************************************
������:ModbusTask()
����:
���:
����:modbus����
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
������:UART485_Init()
����:
���:
����:modbus��ʼ��
***************************************************************/
void UART485_Init(void)
{	
	ElemType e;
	uint8_t  i = 0;
	//��ʼ��ѭ������
  InitQueue(&g_M485Cmd);
	//��ʼ��͸������
	T_InitQueue(&Trans_Queue);
	
	g_MbMtCmd485.rep        = FALSE;                  //δӦ��
	g_MbMtCmd485.PLCCOMFlag = TRUE;		                //ͨѶ��

		
	for(i=1;i<=GPRS_Para.MaxSlaveCnt;i++)
	{	
		e.slave        = i;
		e.func         = MB_REGHOLD_RD;
		e.addr         = 69;	 	                        //���ּĴ������׵�ַ����ʾ������
		e.cnt          = 6;                             //���ݴӻ�����ȷ��д��ļĴ�������
		e.startTimeout = bsp_GetRunTime();
		e.timeout      = 1000;	                        //���� ��ʱʱ��
		e.startPeriod  = bsp_GetRunTime();
		e.period       = 5*1000;	                      //���������
		e.commCnt      = PERIOD_CNT;                    //����ɾ������
		e.timeoutcnt   = 0;		                          //�����������
		EnQueue(&g_M485Cmd, e);		

		e.slave        = i;
		e.func         = MB_REGHOLD_RD;
		e.addr         = 57;	 	                        //���ּĴ������׵�ַ����ʾ������
		e.cnt          = 6;                             //���ݴӻ�����ȷ��д��ļĴ�������
		e.startTimeout = bsp_GetRunTime();
		e.timeout      = 1000;	                        //���� ��ʱʱ��
		e.startPeriod  = bsp_GetRunTime();
		e.period       = 5*1000;	                      //���������
		e.commCnt      = PERIOD_CNT;                    //����ɾ������
		e.timeoutcnt   = 0;		                          //�����������
		EnQueue(&g_M485Cmd, e);
		
		
		
// 		e.slave        = i;
// 		e.func         = MB_BITCOIL_RD;                 //��ȡ��Ȧ
// 		e.addr         = 10;	 	                        //���ּĴ������׵�ַ����ʾ������
// 		e.cnt          = 10;                             //���ݴӻ�����ȷ��д��ļĴ�������
// 		e.startTimeout = bsp_GetRunTime();
// 		e.timeout      = 1000;	                        //���� ��ʱʱ��
// 		e.startPeriod  = bsp_GetRunTime();
// 		e.period       = 5*1000;	                      //���������
// 		e.commCnt      = PERIOD_CNT;                    //����ɾ������
// 		e.timeoutcnt   = 0;		                          //�����������
// 		EnQueue(&g_M485Cmd, e);	
		
  }
	
	//���PDU
	for(i=0;i<UART485_SLAVE_CNT;i++)
	{
		memset((INT16U*)&g_PduUART485[i],0,sizeof(UART485_PDU));
  }
}

/**************************************************************
������:UART485_Master()
����:
���:
����:modbus����
***************************************************************/
uint8_t UART485_Master(uint32_t *addr)
{	
	ElemType e;
	uint8_t  sign = 0;
	uint16_t ret = 0;
	uint16_t i =0;
	
	//�����ж϶������Ƿ������Ч����
	if(!IsQueueEmpty(&g_M485Cmd))
	{
		//��ö�ͷԪ��
		GetHead(&g_M485Cmd,&e);
		
		if(g_MbMtCmd485.rep==FALSE)
		{
			sign = timing(e.startPeriod,e.period);
			if(sign)												
			{	
				UART485_MbMt_req(e,&g_PduUART485[e.slave-1]);								
				e.startTimeout = bsp_GetRunTime();		
				e.startPeriod  = bsp_GetRunTime();	
				//�޸Ķ�ͷԪ�ص���ر���
				SetHead(&g_M485Cmd,e);		

				g_MbMtCmd485.rep=TRUE;					
			}
			else
			{
				//�ȳ��ӣ�����ӣ�������������ں���
				DeQueue(&g_M485Cmd,&e);	
				EnQueue(&g_M485Cmd,e);
				BSP_OS_TimeDlyMs(100);
				//���������ڴ��������
				return 0xfe;
			}				
		}
		else
		{
			sign = timing(e.startTimeout,e.timeout);
			if(sign)											
			{
				g_MbMtCmd485.rep=FALSE;	

				//�޸�ָ������ĳ�ʱ����
				e.timeoutcnt++;	
				//�޸Ķ�ͷԪ�ص���ر���
				SetHead(&g_M485Cmd,e);	
				
				if(e.timeoutcnt >= 3)					   
				{
						e.timeoutcnt = 0;
						//�޸Ķ�ͷԪ�ص���ر���
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
					//��ʱ��֡���,485����
					if(bsp_CheckTimer(UART_485))
					{
						bsp_StopTimer(UART_485);

						//���Ƶ�modbus��Ϣ����������
						memcpy((char*)UART485RxDat,(char*)UART485_RXBUF,UART485_tUART.usRxCount);
						
						ret = UART485_MbMt_rep(e,&g_PduUART485[e.slave-1]);					      

						//��ʱ��������s
						e.timeoutcnt=0;	
						//�޸Ķ�ͷԪ�ص���ر���
				    SetHead(&g_M485Cmd,e);
						
						g_MbMtCmd485.rep=FALSE;						   

						//ͨѶ�������
						if((e.commCnt != PERIOD_CNT)&&(e.commCnt != 0))
						{
							e.commCnt--;
							//�޸Ķ�ͷԪ�ص���ر���
							SetHead(&g_M485Cmd,e);		
						}
						
						//ͨѶ����Ϊ0ʱ��ɾ����������
						if(e.commCnt == 0)    
						{
							//ɾ����ǰ����
							DeQueue(&g_M485Cmd,&e);						
						}
						
						//��ջ�����
						comClearRxFifo(UART_485);	
						
						//����
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
������:UART485_Slave()
����:
���:
����:485�ӿ�modbus�ӻ�
***************************************************************/
uint16_t UART485_Slave(void)
{
	_CMD     Cmd;
	uint16_t ret = 0;

	//����Ϊ�ӻ�ʱ,�̶�Ϊ1����̬��Ϊ��վ
	Cmd.slave = 1;
	
	//��ʱ��֡���,485����
	if(bsp_CheckTimer(UART_485))
	{
		bsp_StopTimer(UART_485);

	  //���Ƶ�modbus��Ϣ����������
	  memcpy((char*)UART485RxDat,(char*)UART485_RXBUF,UART485_tUART.usRxCount);
		
		//�Ĵ�����ַ
		Cmd.addr = ((uint16_t)UART485RxDat[2]<<8)|UART485RxDat[3];
		//���´���ȡ����Ϣ
		UART485_Read();
		
		//modbus�ӻ�����,ʼ�ղ��ýṹ0����Ϊ�ӻ����ݽṹ
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
		
		//��ջ�����
		comClearRxFifo(UART_485);
		
		//485Ӧ����ʱ��ʱ��
		bsp_StartTimer(TIMER_485, 10);
		return ret;	
  }
	
	//485��ʱ���ʹ�����
	if(bsp_CheckTimer(TIMER_485))
	{
		bsp_StopTimer(TIMER_485);
		
		//��ʱӦ��
		comSendBuf(UART_485, UART485Sentdat, Sendlen);
		memset((char*)UART485Sentdat,0,UART485_TXBUFLEN);
  }
	return 0;
}

/**************************************************************
������:UART485_MbMt_req()
����:
���:
����:modbus��������
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
������:UART485_MbMt_rep()
����:
���:
����:modbus������Ӧ
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
������:UART485_MbSl_rep()
����:
���:
����:modbus�ӻ���Ӧ
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
	//�㲥��ַΪ0xff
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
