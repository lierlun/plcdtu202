#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_InitDin
*	功能说明: 配置输入IO
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitDin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_ALL_DIN, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	
	//配置输入引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		/* 设为输入口 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      /* IO口最大速度 */
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_DIN1;
	GPIO_Init(GPIO_PORT_DIN1, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_DIN2;
	GPIO_Init(GPIO_PORT_DIN2, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_DIN3;
	GPIO_Init(GPIO_PORT_DIN3, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_DIN4;
	GPIO_Init(GPIO_PORT_DIN4, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_DIN5;
	GPIO_Init(GPIO_PORT_DIN5, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_DIN6;
	GPIO_Init(GPIO_PORT_DIN6, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_PIN_DIN7;
	GPIO_Init(GPIO_PORT_DIN7, &GPIO_InitStructure);
	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_ReadDin
*	功能说明: 读取输入状态
*	形    参:  _no : 序号，范围 1 - 7
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_ReadDin(uint8_t _no)
{
	uint8_t ret = 0;
	_no--;
	if(_no == 0)
	{
		ret = (!(GPIO_ReadInputData(GPIO_PORT_DIN1)&GPIO_PIN_DIN1)) ? 1 : 0;
	}
	else if(_no == 1)
	{
		ret = (!(GPIO_ReadInputData(GPIO_PORT_DIN2)&GPIO_PIN_DIN2)) ? 1 : 0;
	}
	else if(_no == 2)
	{
		ret = (!(GPIO_ReadInputData(GPIO_PORT_DIN3)&GPIO_PIN_DIN3)) ? 1 : 0;
	}
	else if(_no == 3)
	{
		ret = (!(GPIO_ReadInputData(GPIO_PORT_DIN4)&GPIO_PIN_DIN4)) ? 1 : 0;
	}
	else if(_no == 4)
	{
		ret = ((GPIO_ReadInputData(GPIO_PORT_DIN5)&GPIO_PIN_DIN5)) ? 1 : 0;
	}
	else if(_no == 5)
	{
		ret = ((GPIO_ReadInputData(GPIO_PORT_DIN6)&GPIO_PIN_DIN6)) ? 1 : 0;
	}
	else if(_no == 6)
	{
		ret = (!(GPIO_ReadInputData(GPIO_PORT_DIN7)&GPIO_PIN_DIN7)) ? 1 : 0;
	}	
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_ReadDinAll
*	功能说明: 读取所有输入IO
*	形    参: 无
*	返 回 值: 有输入时相应位置一
*********************************************************************************************************
*/
uint8_t bsp_ReadDinAll(void)
{
	uint8_t ret = 0;
	
	if(!(GPIO_ReadInputData(GPIO_PORT_DIN1)&GPIO_PIN_DIN1))
	{
		ret |= 0x01;
  }
	else
	{
		ret &=~0x01;
  }
	
	if(!(GPIO_ReadInputData(GPIO_PORT_DIN2)&GPIO_PIN_DIN2))
	{
		ret |= 0x02;
  }
	else
	{
		ret &=~0x02;
  }
	
	if(!(GPIO_ReadInputData(GPIO_PORT_DIN3)&GPIO_PIN_DIN3))	
	{
		ret |= 0x04;
  }
	else
	{
		ret &=~0x04;
  }
	
	if(!(GPIO_ReadInputData(GPIO_PORT_DIN4)&GPIO_PIN_DIN4))
	{
		ret |= 0x08;
  }
	else
	{
		ret &=~0x08;
  }
	
	if(GPIO_ReadInputData(GPIO_PORT_DIN5)&GPIO_PIN_DIN5)
	{
		ret |= 0x10;
  }
	else
	{
		ret &=~0x10;
  }

	if(GPIO_ReadInputData(GPIO_PORT_DIN6)&GPIO_PIN_DIN6)
	{
		ret |= 0x20;
  }
	else
	{
		ret &=~0x20;
  }
	
	if(!(GPIO_ReadInputData(GPIO_PORT_DIN7)&GPIO_PIN_DIN7))
	{
		ret |= 0x40;
  }
	else
	{
		ret &=~0x40;
  }
	
	return ret;
}
