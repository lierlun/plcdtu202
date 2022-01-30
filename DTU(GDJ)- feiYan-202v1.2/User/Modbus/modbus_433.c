#include <string.h>
#include <stdarg.h>
#include "modbus_433.h"
#include "crc.h"
#include "bsp.h"
#include "data.h"

uint8_t RF433RxDat[RF433_RXBUFLEN];	     
uint8_t RF433Sentdat[RF433_TXBUFLEN];	 

//433用modbus从机
RF433_PDU  g_PduRF433[RF433_SLAVE_CNT];

/**************************************************************
函数名:RF433_Slave()
输入:
输出:
功能:433用modbus从机
***************************************************************/
uint16_t RF433_Slave(void)
{
	_CMD     Cmd;
	uint16_t ret = 0;

	//当作为从机时
	Cmd.slave = GPRS_Para.slave_addr_433;
	
	//更新待读取的信息
	RF433_Read();
	
	//超时成帧检查,433串口
	if(bsp_CheckTimer(UART_433))
	{
		bsp_StopTimer(UART_433);

	  //复制到modbus消息处理缓冲区中
	  memcpy((char*)RF433RxDat,(char*)g_RxBuf1,g_tUart1.usRxCount);
		
		//modbus从机处理,始终采用结构0，作为从机数据结构
		ret = RF433_MbSl_rep(Cmd,&g_PduRF433[0]);
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
			//写保持寄存器
			RF433_Write();
		}
		
		//清空缓冲区
		comClearRxFifo(UART_433);
		return ret;	
  }
	return 0;
}

/**************************************************************
函数名:RF433_MbMt_req()
输入:
输出:
功能:modbus主机请求
***************************************************************/
uint16_t RF433_MbMt_req(_CMD Cmd,RF433_PDU *Pdu)
{
	uint8_t ID_Code,FuncCode,chr;
	uint16_t SbytAddr,Sshft,EAddr,Eshft;
	uint16_t Tcnt;
	uint32_t i,m;
	uDAT16 D16;
	uDAT32 D32;

	memset(RF433Sentdat,0,RF433_RXBUFLEN);
	Tcnt=0;						
	ID_Code=Cmd.slave;               
	RF433Sentdat[Tcnt++]=ID_Code;
	FuncCode=Cmd.func;                
	RF433Sentdat[Tcnt++]=FuncCode;
	
	switch(FuncCode)
	{
			case MB_BITCOIL_RD:
				D16.d16=Cmd.addr+RF433_BITCOIL_BADDR;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_BITINP_RD:
				D16.d16=Cmd.addr+RF433_BITINP_BADDR;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_REGHOLD_RD:
				D16.d16=Cmd.addr+RF433_REGHOLD_BADDR;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_REGINP_RD:
				D16.d16=Cmd.addr+RF433_REGINP_BADDR;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_BITCOIL_WT:
				D16.d16=Cmd.addr+RF433_BITCOIL_BADDR;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
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
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_REGHOLD_WT:
				D16.d16=Cmd.addr+RF433_REGHOLD_BADDR;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				//D16.d16=Counts;
				D16.d16=Pdu->RegHold[Cmd.addr];
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				break;

			case MB_MULTICOIL_WT:
				D16.d16=Cmd.addr+RF433_BITCOIL_BADDR;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				if((Cmd.cnt%8)==0)				
				{
					RF433Sentdat[Tcnt++]=(Cmd.cnt>>3);
				}
				else
				{
					RF433Sentdat[Tcnt++]=(Cmd.cnt>>3)+1;
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
					RF433Sentdat[Tcnt++]=D32.d16[0].d8.Hi;
					m+=8;
					if(m>=EAddr) break;
					RF433Sentdat[Tcnt++]=D32.d16[0].d8.Lo;
					m+=8;
					if(m>=EAddr) break;
				}
				if(Eshft!=0)
				{
					chr=(RF433Sentdat[Tcnt-1]&0xFF)<<(D8BITNUM-Eshft);
					chr=(chr&0xFF)>>(D8BITNUM-Eshft);
					RF433Sentdat[Tcnt-1]=chr;
				}
				break;

			case MB_MULTIREG_WT:
				D16.d16=Cmd.addr+RF433_REGHOLD_BADDR;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				D16.d16=Cmd.cnt;
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				RF433Sentdat[Tcnt++]=Cmd.cnt*2;

				EAddr=Cmd.addr+Cmd.cnt;

				for(i=Cmd.addr;i<EAddr;i++)
				{
					D16.d16=Pdu->RegHold[i];
					RF433Sentdat[Tcnt++]=D16.d8.Hi;
					RF433Sentdat[Tcnt++]=D16.d8.Lo;
				}
				break;
			default:
				break;
	}
	D16.d16=calCRC16_modbus(RF433Sentdat,Tcnt);
	RF433Sentdat[Tcnt++]=D16.d8.Hi;
	RF433Sentdat[Tcnt++]=D16.d8.Lo;

	RF433_SEND(RF433Sentdat,Tcnt)
	
	return FuncCode;
}

/**************************************************************
函数名:RF433_MbMt_rep()
输入:
输出:
功能:modbus主机响应
***************************************************************/
uint16_t RF433_MbMt_rep(_CMD Cmd,RF433_PDU *Pdu)
{
	uint8_t  ID_Code,FuncCode,ByteCnt;
	uint16_t SAddr,SbytAddr,Sshft,EAddr,CRCValue;
	uint16_t Rcnt,Counts;
	uint32_t i,j,m,n,tmp1;
	uDAT16   D16;

	Rcnt=0;
	
	ID_Code=RF433RxDat[Rcnt++];
	if(ID_Code!=Cmd.slave)       
	{
		return 1;
	}
	
	FuncCode=RF433RxDat[Rcnt++];
	if(FuncCode!=Cmd.func)
	{
		return 1;
	}
	switch(FuncCode)
	{
			case MB_BITCOIL_RD:
				ByteCnt=RF433RxDat[Rcnt++];
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
						D16.d16=RF433RxDat[Rcnt++];
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
				ByteCnt=RF433RxDat[Rcnt++];
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
						D16.d16=RF433RxDat[Rcnt++];
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
				ByteCnt=RF433RxDat[Rcnt++];
				Counts=Cmd.cnt*2;
				if(ByteCnt!=Counts)		
				{
					return 1;
				}
				SAddr=Cmd.addr;
				
				for(m=0,i=SAddr;m<ByteCnt;i++)
				{
					D16.d8.Hi=RF433RxDat[Rcnt++];
					m++;
					if(m<ByteCnt)	D16.d8.Lo=RF433RxDat[Rcnt++];
					else			    D16.d8.Lo=Pdu->RegHold[i];
					m++;
					Pdu->RegHold[i]=D16.d16;
				}
				break;

			case MB_REGINP_RD:		
				ByteCnt=RF433RxDat[Rcnt++];
				Counts=Cmd.cnt*2;
				if(ByteCnt!=Counts)
				{
					return 1;
				}
				SAddr=Cmd.addr;
				
				for(m=0,i=SAddr;m<ByteCnt;i++)
				{
					D16.d8.Hi=RF433RxDat[Rcnt++];
					m++;
					if(m<ByteCnt)	D16.d8.Lo=RF433RxDat[Rcnt++];
					else			    D16.d8.Lo=Pdu->RegInp[i];
					m++;
					Pdu->RegInp[i]=D16.d16;
				}
				break;

			case MB_BITCOIL_WT:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16-RF433_BITCOIL_BADDR;
				if(SAddr!=Cmd.addr)		
				{
					return 1;
				}
				
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				if(D16.d16!=Cmd.cnt)
				{
					return 1;
				}
				break;

			case MB_REGHOLD_WT:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16-RF433_REGHOLD_BADDR;
				if(SAddr!=Cmd.addr)		
				{
					return 1;
				}
				
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				break;

			case MB_MULTICOIL_WT:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16-RF433_BITCOIL_BADDR;
				if(SAddr!=Cmd.addr)			
				{
					return 1;
				}
				
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				if(D16.d16!=Cmd.cnt)
				{
					return 1;
				}
				break;

			case MB_MULTIREG_WT:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16-RF433_REGHOLD_BADDR;
				if(SAddr!=Cmd.addr)	
				{
					return 1;
				}
				
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				if(D16.d16!=Cmd.cnt)
				{
					return 1;
				}
				break;
			default:
				return FuncCode;
	}
	CRCValue=calCRC16_modbus(RF433RxDat, Rcnt);	
	D16.d8.Hi=RF433RxDat[Rcnt++];
	D16.d8.Lo=RF433RxDat[Rcnt++];
	if(CRCValue!=D16.d16)		
	{
	   return 1;
	}
	return FuncCode;	
}

/**************************************************************
函数名:RF433_MbSl_rep()
输入:
输出:
功能:modbus从机响应
***************************************************************/
uint16_t RF433_MbSl_rep(_CMD Cmd,RF433_PDU *Pdu)
{
	uint8_t    ID_Code,FuncCode,ErrCode,chr;
	uint16_t   SAddr,SbytAddr,Sshft,EAddr,Eshft,ByteCnt,CRCValue;
	uint16_t   Rcnt,Tcnt,Counts;
	uint32_t   i,j,m,n,tmp1,tmp2;
	uDAT16   	 D16;
	uDAT32   	 D32;

  memset(RF433Sentdat,0,RF433_TXBUFLEN);
	Rcnt=Tcnt=0;
	ErrCode=0;
	
	ID_Code=RF433RxDat[Rcnt++];
	//广播地址为0xff
	if((ID_Code!=Cmd.slave)&&(ID_Code != 0xff))	return 0;
	else				RF433Sentdat[Tcnt++]=ID_Code;
	
	FuncCode=RF433RxDat[Rcnt++];
	RF433Sentdat[Tcnt++]=FuncCode;                 
	
	switch(FuncCode)
	{
			case MB_BITCOIL_RD:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX2_BITCOIL)		 
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
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
					RF433Sentdat[Tcnt++]=tmp1;
				}
				else
				{
					RF433Sentdat[Tcnt++]=tmp1+1;
				}
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX2_BITCOIL)				
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
					RF433Sentdat[Tcnt++]=D32.d16[0].d8.Lo;
					m+=8;
					if(m>=EAddr) break;
					RF433Sentdat[Tcnt++]=D32.d16[0].d8.Hi;
					m+=8;
					if(m>=EAddr) break;
				}

				chr=(RF433Sentdat[Tcnt-1]&0xFF)<<(D8BITNUM-Eshft);
				chr=(chr&0xFF)>>(D8BITNUM-Eshft);
				RF433Sentdat[Tcnt-1]=chr;
				break;

			case MB_BITINP_RD:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX2_BITINP)			
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
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
					RF433Sentdat[Tcnt++]=tmp1;
				}
				else
				{
					RF433Sentdat[Tcnt++]=tmp1+1;
				}
				EAddr=SAddr+Counts;
				if(EAddr>MAX2_BITINP)					
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
					RF433Sentdat[Tcnt++]=D32.d16[0].d8.Lo;
					m+=8;
					if(m>=EAddr) break;
					RF433Sentdat[Tcnt++]=D32.d16[0].d8.Hi;
					m+=8;
					if(m>=EAddr) break;
				}
				chr=(RF433Sentdat[Tcnt-1]&0xFF)<<(D8BITNUM-Eshft);
				chr=(chr&0xFF)>>(D8BITNUM-Eshft);
				RF433Sentdat[Tcnt-1]=chr;
				break;
			case MB_REGHOLD_RD:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX2_REGHOLD)					  
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGHOLD_RD)		
				{
					ErrCode=0x03;
					break;
				}
				RF433Sentdat[Tcnt++]=Counts*2;		
				EAddr=SAddr+Counts;
				if(EAddr>MAX2_REGHOLD)				
				{
					ErrCode=0x02;
					break;
				}
				for(i=SAddr;i<EAddr;i++)
				{
					D16.d16=Pdu->RegHold[i];
				  RF433Sentdat[Tcnt++]=D16.d8.Hi;
					RF433Sentdat[Tcnt++]=D16.d8.Lo;
				}
				break;
			case MB_REGINP_RD:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX2_REGINP)				
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGINP_RD)		
				{
					ErrCode=0x03;
					break;
				}
				RF433Sentdat[Tcnt++]=Counts*2;				
				EAddr=SAddr+Counts;
				if(EAddr>MAX2_REGINP)			
				{
					ErrCode=0x02;
					break;
				}
				for(i=SAddr;i<EAddr;i++)
				{
					D16.d16=Pdu->RegInp[i];
				  RF433Sentdat[Tcnt++]=D16.d8.Hi;
					RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				}
				break;
			case MB_BITCOIL_WT:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX2_BITCOIL)				
				{
					ErrCode=0x02;
					break;
				}
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;

				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
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
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				break;
			case MB_REGHOLD_WT:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX2_REGHOLD)				
				{
					ErrCode=0x02;
					break;
				}
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				Pdu->RegHold[SAddr]=D16.d16;

				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				break;
			case MB_MULTICOIL_WT:
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX2_BITCOIL)					
				{
					ErrCode=0x02;
					break;
				}
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_BITCOIL_WT)			
				{
					ErrCode=0x03;
					break;
				}
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX2_BITCOIL)				
				{
					ErrCode=0x02;
					break;
				}
				
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;
				
				Eshft=EAddr%D8BITNUM;

				ByteCnt=RF433RxDat[Rcnt++];
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
						D16.d16=RF433RxDat[Rcnt++];
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
				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX2_REGHOLD)					
				{
					ErrCode=0x02;
					break;
				}
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;

				D16.d8.Hi=RF433RxDat[Rcnt++];
				D16.d8.Lo=RF433RxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGHOLD_WT)		
				{
					ErrCode=0x03;
					break;
				}
				RF433Sentdat[Tcnt++]=D16.d8.Hi;
				RF433Sentdat[Tcnt++]=D16.d8.Lo;
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX2_REGHOLD)			
				{
					ErrCode=0x02;
					break;
				}
				
				ByteCnt=RF433RxDat[Rcnt++];
				if(ByteCnt!=Counts*2)					
				{
					ErrCode=0x03;
					break;
				}			
				for(i=SAddr;i<EAddr;i++)
				{
				  D16.d8.Hi=RF433RxDat[Rcnt++];
					D16.d8.Lo=RF433RxDat[Rcnt++];
					Pdu->RegHold[i]=D16.d16;
				}
				break;
			  default:
					ErrCode=MB_ERR_FUNCODE;
				break;
	}
	CRCValue=calCRC16_modbus(RF433RxDat, Rcnt);	
	D16.d8.Hi=RF433RxDat[Rcnt++];
	D16.d8.Lo=RF433RxDat[Rcnt++];
	memset(RF433RxDat,0,Rcnt);
	if(CRCValue!=D16.d16)				
	{
	   return 0;
	}
		
	if(ErrCode!=0)	                  
	{
		RF433Sentdat[1]+=0x80;
		RF433Sentdat[Tcnt++]=ErrCode;
	}
	D16.d16=calCRC16_modbus(RF433Sentdat,Tcnt);
	RF433Sentdat[Tcnt++]=D16.d8.Hi;
	RF433Sentdat[Tcnt++]=D16.d8.Lo;
	
	RF433_SEND(RF433Sentdat,Tcnt)
	
	if(ErrCode!=0)	                   
	{
	  return 0;
	}
	else
	{
	 return FuncCode;
	}	
}

/*
*********************************************************************************************************
*	函 数 名: RF433_WaitATResponse
*	功能说明: 等待SIM800C返回指定的应答字符串. 比如等待 OK
*	形    参: _pAckStr : 应答的字符串， 长度不得超过255
*			 _usTimeOut : 命令执行超时，0表示一直等待. >０表示超时时间，单位1ms
*	返 回 值: 1 表示成功  0 表示失败
*********************************************************************************************************
*/
uint8_t RF433_WaitATResponse(char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t  ucData;
	uint16_t pos = 0;
	uint8_t  ret;
	uint8_t  p[32];
	
	if (strlen(_pAckStr) > 32)
	{
		return 0;
	}
	/* _usTimeOut == 0 表示无限等待 */
	if (_usTimeOut > 0)
	{
		bsp_StartTimer(RF433_TMR_ID, _usTimeOut);	
	}
	while (1)
	{
		bsp_DelayUS(1000);
		if (_usTimeOut > 0)
		{
			if (bsp_CheckTimer(RF433_TMR_ID))
			{
				ret = 0;	    /* 超时 */	
				break;
			}
		}
		
		//等待超时成帧
		if(bsp_CheckTimer(UART_433))
		{
			bsp_StopTimer(UART_433);
			
			//读空缓冲区
			while(comGetChar(UART_433, &ucData))
			{
				p[pos++] = ucData;
				if(pos > 32)
				{
					ret = 0;	    /* 错误 */
					break;					
				}
			}
			
			//先转换为字符串，再做处理
			sprintf((char*)p,"%s",(char*)p);	
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
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: Send433_AT
*	功能说明: 格式化输出的AT命令，包括等待应答
*	形    参: _try:出错时重发次数
*           _usTimeOut:超时时间
*           _pAckStr:应答字符串
*           fmt:格式化的字符串
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t Send433_AT(uint8_t _try,uint16_t _usTimeOut,char* _pAckStr, char *fmt,...)
{
	__va_list v_list;
	uint8_t   ret;
	uint8_t   cnt = 0;
	uint8_t   p[32];
	
	do
	{
		//清空FIFO
		comClearFifo(UART_433);	
		//不允许格式化的字符串大于32,缓冲区长度决定
		if(strlen(fmt) > 32)
		{
			return FALSE;
		}
		va_start(v_list, fmt);     
		vsprintf((char*)p, fmt, v_list); 
		va_end(v_list);	
	
		//发送命令
		comSendBuf(UART_433, p, strlen((char*)p));
		//等待应答
		ret = RF433_WaitATResponse(_pAckStr, _usTimeOut);
    cnt++;		
	}
	while((ret == 0)&&(cnt < _try));

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: RF433_SwitchAT
*	功能说明: 工作模式切换
*	形    参: sta:0:进入AT命令模式，1：退出AT命令模式
*	返 回 值: 无
*********************************************************************************************************
*/
void RF433_SwitchAT(uint8_t sta)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_ALL_SET433, ENABLE);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	/* 推挽输出模式 */
	//设置为输出
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SET433;
	GPIO_Init(GPIO_PORT_SET433, &GPIO_InitStructure);
	
	if(sta == 0)
	{
		//输出低电平
		GPIO_PORT_SET433->BRR = GPIO_PIN_SET433;
		//延时500ms
	  bsp_DelayMS(500);
  }
	else
	{
		//输出高电平
		GPIO_PORT_SET433->BSRR = GPIO_PIN_SET433;
		//延时1000ms
		bsp_DelayMS(1000);
  }
}

/***************************************************************************************/
//433模块常用的AT指令
//1,测试指令：        AT               OK
//2,波特率设置指令：  AT+Bxxxx         OK+Bxxxx    x值：1200 2400 4800 9600 19200 38400 57600 115200
//3,信道设置指令：    AT+Cxxx          OK+Cxxx     x值范围：1~100
//4,发射功率设置指令: AT+Px            OK+Px       x值范围：1~8
/***************************************************************************************/
uint8_t RF33_Set(uint8_t cmd,uint8_t value)
{
	uint8_t ret = 0;

	switch(cmd)
	{
		case 1:
		{
			ret = Send433_AT(3,500,"OK","AT");
    }break;
		case 2:
		{
			ret = Send433_AT(3,500,"OK","AT+B%d",value);
    }break;
		case 3:
		{
			if(value < 10)
			{
				ret = Send433_AT(3,500,"OK","AT+C00%d",value);
      }
			else if((value >= 10)&(value < 100))
			{
				ret = Send433_AT(3,500,"OK","AT+C0%d",value);
      }
			else
			{
				ret = Send433_AT(3,500,"OK","AT+C%d",value);
      }
    }break;
		case 4:
		{
			ret = Send433_AT(3,500,"OK","AT+P%d",value);
    }break;
		default:break;
  }
	return ret;
}
