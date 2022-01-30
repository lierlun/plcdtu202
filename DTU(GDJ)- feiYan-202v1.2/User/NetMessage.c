#include "NetMessage.h"
#include "luat_air208.h"
#include "modbus_gprs_iccid.h"
#include "data.h"
#include "param.h"
#include "bsp_key.h"
#include "cJSON.h"


/*
*********************************************************************************************************
*	函 数 名: Send_JsonHeatBeatMsg
*	功能说明: 发送心跳消息
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void  Send_JsonHeatBeatMsg(void)
{
		uint8_t i;
		typedef union
		{
			float f;
			uint16_t RegHold[2];
		}union_t;
		union_t Ia[4]={0},Ib[4]={0},Ic[4]={0};
		char  *pBuf = NULL;
		cJSON *root, *item;
		
		for(i=0; i<4; i++)
		{
			Ia[i].RegHold[1] = g_PduUART485[i].RegHold[69];
			Ia[i].RegHold[0] = g_PduUART485[i].RegHold[70];
			Ib[i].RegHold[1] = g_PduUART485[i].RegHold[71];
			Ib[i].RegHold[0] = g_PduUART485[i].RegHold[72];
			Ic[i].RegHold[1] = g_PduUART485[i].RegHold[73];
			Ic[i].RegHold[0] = g_PduUART485[i].RegHold[74];
		}  
	  root = cJSON_CreateObject();	 
		item = cJSON_CreateObject();
		cJSON_AddItemToObject(item,"IA0",cJSON_CreateNumber(Ia[0].f));
	  cJSON_AddItemToObject(item,"IB0",cJSON_CreateNumber(Ib[0].f));
		cJSON_AddItemToObject(item,"IC0",cJSON_CreateNumber(Ic[0].f));
		cJSON_AddItemToObject(item,"IA1",cJSON_CreateNumber(Ia[1].f));
	  cJSON_AddItemToObject(item,"IB1",cJSON_CreateNumber(Ib[1].f));
		cJSON_AddItemToObject(item,"IC1",cJSON_CreateNumber(Ic[1].f));
		cJSON_AddItemToObject(item,"IA2",cJSON_CreateNumber(Ia[2].f));
	  cJSON_AddItemToObject(item,"IB2",cJSON_CreateNumber(Ib[2].f));
		cJSON_AddItemToObject(item,"IC2",cJSON_CreateNumber(Ic[2].f));
		cJSON_AddItemToObject(item,"IA3",cJSON_CreateNumber(Ia[3].f));
	  cJSON_AddItemToObject(item,"IB3",cJSON_CreateNumber(Ib[3].f));	
		cJSON_AddItemToObject(item,"IC3",cJSON_CreateNumber(Ic[3].f));
		cJSON_AddItemToObject(root,"params",item);
		pBuf = cJSON_PrintUnformatted(root);		 
    //发送
		AIR208_Send((uint8_t*)pBuf,strlen((char*)pBuf),0);		
	  cJSON_Delete(root);//释放cJSON_CreateObject分配出来的内存空间	
		if(pBuf != NULL)
		{
			free(pBuf);
		}

}


