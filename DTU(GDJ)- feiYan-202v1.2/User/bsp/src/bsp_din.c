#include "bsp.h"

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitDin
*	����˵��: ��������IO
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitDin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_ALL_DIN, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	
	//������������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	      /* IO������ٶ� */
	
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
*	�� �� ��: bsp_ReadDin
*	����˵��: ��ȡ����״̬
*	��    ��:  _no : ��ţ���Χ 1 - 7
*	�� �� ֵ: ��
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
*	�� �� ��: bsp_ReadDinAll
*	����˵��: ��ȡ��������IO
*	��    ��: ��
*	�� �� ֵ: ������ʱ��Ӧλ��һ
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
