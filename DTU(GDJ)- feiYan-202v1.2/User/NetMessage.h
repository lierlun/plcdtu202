#ifndef __NET_MESSAGE_H
#define __NET_MESSAGE_H
#include "bsp.h"

extern void    GPRS_Read(void);
extern void    GPRS_Write(void);
extern void    Send_HeatBeatMsg(void);
extern void  	 Send_JsonHeatBeatMsg(void);
extern void    Send_JsonAllParamsMsg1(void);
extern void    Send_JsonAllParamsMsg2(void);
extern void    Send_JsonAllParamsMsg3(void);
extern void    Send_JsonAllParamsMsg4(void);
extern void    Send_JsonAllParamsMsg5(void);
extern void    Send_JsonAllParamsMsg6(void);

#endif
