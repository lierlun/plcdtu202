#include <string.h>
#include "modbus_gprs_iccid.h"
#include "crc.h"
#include "bsp.h"
#include "data.h"

uint8_t GprsRxDat[GPRS_RXBUFLEN];	     
uint8_t GprsSentdat[GPRS_TXBUFLEN];	 

//SIM800C用modbus从机
GPRS_PDU   g_PduGprs;
	
/**************************************************************
函数名:GPRS_Slave()
输入:
输出:
功能:SIM800C用modbus从机
***************************************************************/
uint16_t GPRS_Slave(char *pBuf,uint16_t len)
{
	_CMD     Cmd;
	uint16_t ret = 0;

	//当作为从机时
	Cmd.slave = GPRS_SLAVE_ADDR;
	
	//更新待读取的信息
	GPRS_Read();
	
	//复制到modbus消息处理缓冲区中
	memcpy((char*)GprsRxDat,(char*)pBuf,len);
	//modbus从机处理
	ret = GPRS_MbSl_rep(Cmd,&g_PduGprs);
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
		GPRS_Write();
	}
  return ret;	
}


/**************************************************************
函数名:GPRS_MbSl_rep()
输入:
输出:
功能:modbus从机响应
***************************************************************/
uint16_t GPRS_MbSl_rep(_CMD Cmd,GPRS_PDU *Pdu)
{
	uint8_t    ID_Code,FuncCode,ErrCode,chr;
	uint16_t   SAddr,SbytAddr,Sshft,EAddr,Eshft,ByteCnt,CRCValue;
	uint16_t   Rcnt,Tcnt,Counts;
	uint32_t   i,j,m,n,tmp1,tmp2;
	uDAT16   	 D16;
	uDAT32   	 D32;

  memset(GprsSentdat,0,GPRS_TXBUFLEN);
	Rcnt=Tcnt=0;
	ErrCode=0;

	ID_Code=GprsRxDat[Rcnt++];
	//广播地址为0xff
	if((ID_Code!=Cmd.slave)&&(ID_Code != 0xff))	return 0;
	else				GprsSentdat[Tcnt++]=ID_Code;
	
	FuncCode=GprsRxDat[Rcnt++];
	GprsSentdat[Tcnt++]=FuncCode;                 
	
	switch(FuncCode)
	{
			case MB_BITCOIL_RD:
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX1_BITCOIL)		 
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
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
					GprsSentdat[Tcnt++]=tmp1;
				}
				else
				{
					GprsSentdat[Tcnt++]=tmp1+1;
				}
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX1_BITCOIL)				
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
					GprsSentdat[Tcnt++]=D32.d16[0].d8.Lo;
					m+=8;
					if(m>=EAddr) break;
					GprsSentdat[Tcnt++]=D32.d16[0].d8.Hi;
					m+=8;
					if(m>=EAddr) break;
				}

				chr=(GprsSentdat[Tcnt-1]&0xFF)<<(D8BITNUM-Eshft);
				chr=(chr&0xFF)>>(D8BITNUM-Eshft);
				GprsSentdat[Tcnt-1]=chr;
				break;

			case MB_BITINP_RD:
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX1_BITINP)			
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
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
					GprsSentdat[Tcnt++]=tmp1;
				}
				else
				{
					GprsSentdat[Tcnt++]=tmp1+1;
				}
				EAddr=SAddr+Counts;
				if(EAddr>MAX1_BITINP)					
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
					GprsSentdat[Tcnt++]=D32.d16[0].d8.Lo;
					m+=8;
					if(m>=EAddr) break;
					GprsSentdat[Tcnt++]=D32.d16[0].d8.Hi;
					m+=8;
					if(m>=EAddr) break;
				}
				chr=(GprsSentdat[Tcnt-1]&0xFF)<<(D8BITNUM-Eshft);
				chr=(chr&0xFF)>>(D8BITNUM-Eshft);
				GprsSentdat[Tcnt-1]=chr;
				break;
			case MB_REGHOLD_RD:
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX1_REGHOLD)					  
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGHOLD_RD)		
				{
					ErrCode=0x03;
					break;
				}
				GprsSentdat[Tcnt++]=Counts*2;		
				EAddr=SAddr+Counts;
				if(EAddr>MAX1_REGHOLD)				
				{
					ErrCode=0x02;
					break;
				}
				for(i=SAddr;i<EAddr;i++)
				{
					D16.d16=Pdu->RegHold[i];
				  GprsSentdat[Tcnt++]=D16.d8.Hi;
					GprsSentdat[Tcnt++]=D16.d8.Lo;
				}
				break;
			case MB_REGINP_RD:
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX1_REGINP)				
				{
					ErrCode=0x02;
					break;
				}
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGINP_RD)		
				{
					ErrCode=0x03;
					break;
				}
				GprsSentdat[Tcnt++]=Counts*2;				
				EAddr=SAddr+Counts;
				if(EAddr>MAX1_REGINP)			
				{
					ErrCode=0x02;
					break;
				}
				for(i=SAddr;i<EAddr;i++)
				{
					D16.d16=Pdu->RegInp[i];
				  GprsSentdat[Tcnt++]=D16.d8.Hi;
					GprsSentdat[Tcnt++]=D16.d8.Lo;
				
				}
				break;
			case MB_BITCOIL_WT:
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX1_BITCOIL)				
				{
					ErrCode=0x02;
					break;
				}
				GprsSentdat[Tcnt++]=D16.d8.Hi;
				GprsSentdat[Tcnt++]=D16.d8.Lo;
				
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;

				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
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
				GprsSentdat[Tcnt++]=D16.d8.Hi;
				GprsSentdat[Tcnt++]=D16.d8.Lo;
				
				break;
			case MB_REGHOLD_WT:
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX1_REGHOLD)				
				{
					ErrCode=0x02;
					break;
				}
				GprsSentdat[Tcnt++]=D16.d8.Hi;
				GprsSentdat[Tcnt++]=D16.d8.Lo;
				
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				Pdu->RegHold[SAddr]=D16.d16;

				GprsSentdat[Tcnt++]=D16.d8.Hi;
				GprsSentdat[Tcnt++]=D16.d8.Lo;
				
				break;
			case MB_MULTICOIL_WT:
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX1_BITCOIL)					
				{
					ErrCode=0x02;
					break;
				}
				GprsSentdat[Tcnt++]=D16.d8.Hi;
				GprsSentdat[Tcnt++]=D16.d8.Lo;
				
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_BITCOIL_WT)			
				{
					ErrCode=0x03;
					break;
				}
				GprsSentdat[Tcnt++]=D16.d8.Hi;
				GprsSentdat[Tcnt++]=D16.d8.Lo;
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX1_BITCOIL)				
				{
					ErrCode=0x02;
					break;
				}
				
				SbytAddr=SAddr/D16BITNUM;
				Sshft=SAddr%D16BITNUM;
				
				Eshft=EAddr%D8BITNUM;

				ByteCnt=GprsRxDat[Rcnt++];
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
						D16.d16=GprsRxDat[Rcnt++];
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
				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				SAddr=D16.d16;
				if(SAddr>MAX1_REGHOLD)					
				{
					ErrCode=0x02;
					break;
				}
				GprsSentdat[Tcnt++]=D16.d8.Hi;
				GprsSentdat[Tcnt++]=D16.d8.Lo;

				D16.d8.Hi=GprsRxDat[Rcnt++];
				D16.d8.Lo=GprsRxDat[Rcnt++];
				Counts=D16.d16;
				if(Counts>MBMAX_REGHOLD_WT)		
				{
					ErrCode=0x03;
					break;
				}
				GprsSentdat[Tcnt++]=D16.d8.Hi;
				GprsSentdat[Tcnt++]=D16.d8.Lo;
				
				EAddr=SAddr+Counts;
				if(EAddr>MAX1_REGHOLD)			
				{
					ErrCode=0x02;
					break;
				}
				
				ByteCnt=GprsRxDat[Rcnt++];
				if(ByteCnt!=Counts*2)					
				{
					ErrCode=0x03;
					break;
				}			
				for(i=SAddr;i<EAddr;i++)
				{
				  D16.d8.Hi=GprsRxDat[Rcnt++];
					D16.d8.Lo=GprsRxDat[Rcnt++];
					Pdu->RegHold[i]=D16.d16;
				}
				break;
			  default:
					ErrCode=MB_ERR_FUNCODE;
				break;
	}
	CRCValue=calCRC16_modbus(GprsRxDat, Rcnt);	
	D16.d8.Hi=GprsRxDat[Rcnt++];
	D16.d8.Lo=GprsRxDat[Rcnt++];
	memset(GprsRxDat,0,Rcnt);
	if(CRCValue!=D16.d16)				
	{
	   return 0;
	}
		
	if(ErrCode!=0)	                  
	{
		GprsSentdat[1]+=0x80;
		GprsSentdat[Tcnt++]=ErrCode;
	}
	D16.d16=calCRC16_modbus(GprsSentdat,Tcnt);
	GprsSentdat[Tcnt++]=D16.d8.Hi;
	GprsSentdat[Tcnt++]=D16.d8.Lo;
	
	AIR208_Send(GprsSentdat,Tcnt,0);
	
	if(ErrCode!=0)	                   
	{
	  return 0;
	}
	else
	{
	 return FuncCode;
	}	
}
