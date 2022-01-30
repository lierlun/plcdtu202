/*
*********************************************************************************************************
*
*	模块名称 : DHT21 驱动模块(1-wire 数字温度传感器）
*	文件名称 : bsp_dht21.c
*	版    本 : V1.0
*	说    明 : DH211和CPU之间采用1个GPIO接口。建议调用 DHT21_ReadData()的频率不要大于1Hz。
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0    2014-01-24  armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	DHT21 可以用杜邦线连接到开发板。

     DHT21      STM32F407开发板
	  VCC   ------  3.3V
	  DQ    ------  PB14   (开发板上有 4.7K 上拉电阻. 和 USER 键复用)
	  GND   ------  GND
*/

/* 定义GPIO端口 */
#define RCC_DQ		RCC_APB2Periph_GPIOB
#define PORT_DQ		GPIOB
#define PIN_DQ		GPIO_Pin_14

#define DQ_0()		PORT_DQ->BRR = PIN_DQ
#define DQ_1()		PORT_DQ->BSRR = PIN_DQ

/* 判断DQ输入是否为低 */
#define DQ_IS_LOW()	((PORT_DQ->IDR & PIN_DQ) == 0)


static uint8_t DHT21_ReadByte(void);

/*
*********************************************************************************************************
*	函 数 名: bsp_InitDHT21
*	功能说明: 配置STM32的GPIO和SPI接口，用于连接 DHT11
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitDHT21(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_DQ, ENABLE);

	DQ_1();

	/* 配置DQ为开漏输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	

	GPIO_InitStructure.GPIO_Pin = PIN_DQ;
	GPIO_Init(PORT_DQ, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: DHT21_ReadData
*	功能说明: 复位DHT11。 拉低DQ为低，持续最少480us，然后等待
*	形    参: 无
*	返 回 值: 0 失败； 1 表示成功
*********************************************************************************************************
*/
uint8_t DHT21_ReadData(DHT21_T *_pDTH)
{
#if uCOS_EN == 1 
	CPU_SR_ALLOC();
#endif
	uint8_t   sum;
	uint16_t  temp;
	uint8_t   i;
  uint8_t   cnt = 0;
	
	DISABLE_INT();/* 禁止全局中断 */
	
	/* 1. 主机发送起始信号, DQ拉低时间 600us */
	DQ_0();		/* DQ = 0 */
	bsp_DelayUS(600);
	DQ_1();		/* DQ = 1 */

	bsp_DelayUS(30);	/* 等待30us */

	if(DQ_IS_LOW())
	{
			cnt = 0;
			while(DQ_IS_LOW())     //DHT拉低80us
			{
				bsp_DelayUS(1);
				cnt++;
				if(cnt > 150)
				{
					ENABLE_INT();	/* 使能全局中断 */
					return 0;
				}
			}

			cnt = 0;
			while(!DQ_IS_LOW())    //DHT拉高80us
			{
				bsp_DelayUS(1);
				cnt++;
				if(cnt > 150)
				{
					ENABLE_INT();	/* 使能全局中断 */
					return 0;
				}  
			}

			for(i=0;i<5;i++)
			{
				_pDTH->Buf[i] = DHT21_ReadByte();
      }

			bsp_DelayUS(100);
			/* 计算校验和 */
			sum = _pDTH->Buf[0] + _pDTH->Buf[1] + _pDTH->Buf[2] + _pDTH->Buf[3];
			if (sum == _pDTH->Buf[4])
			{
				//计算温度
				temp = (((uint16_t)_pDTH->Buf[2])<<8)|_pDTH->Buf[3];
				//判断温度是否为零下
				if(temp&0x8000)
				{
					_pDTH->Temp = -(double)temp/10.0;	
				}
				else
				{
					_pDTH->Temp = (double)temp/10.0;		
				}
				
				//计算湿度
				temp = (((uint16_t)_pDTH->Buf[0])<<8)|_pDTH->Buf[1];
				
				_pDTH->Hum = (double)temp/10.0;	
				ENABLE_INT();	/* 使能全局中断 */
				return 1;
			}
  }
	
	ENABLE_INT();	/* 使能全局中断 */
	return 0;
}
/*
*********************************************************************************************************
*	函 数 名: DHT21_ReadByte
*	功能说明: 向DHT21读取1字节数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static uint8_t DHT21_ReadByte(void)
{
  uint8_t i;
  uint8_t temp = 0;
  uint8_t cnt = 0;
  for(i=0;i<8;i++)
  {
    cnt = 0;
    while(DQ_IS_LOW()) //等待DHT低电平开始,DHT拉低50us表数据开始
    {
			bsp_DelayUS(1); 
      cnt++;
      if(cnt > 150)
      {
        return 0;
      }    
    }
    bsp_DelayUS(40);                 //如果高电平时长26~28us表示"0",70us表示"1"，此处延时40us再判断
    if(!DQ_IS_LOW())                 //如果还为高电平，则为"1"
    {
      temp |= (0x80>>i);
    }
    cnt = 0;
    while(!DQ_IS_LOW())     //等待DHT高电平结束
    {
			bsp_DelayUS(1); 
      cnt++;
      if(cnt > 150)
      {
        return 0;
      }     
    }
  }
  return temp;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
