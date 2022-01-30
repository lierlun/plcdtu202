#include "bsp.h"

static void     PortIO_Config(GPIOMode_TypeDef GPIO_Mode);
static void     WriteData(uint16_t data);
static uint16_t ReadData(void);
static uint16_t ILI9341_BGR2RGB(uint16_t c);
static void     ILI9341_Init(void);
static void     ILI9341_WriteRAM_Start(void);
static uint16_t ILI9341_BGR2RGB(uint16_t c);

//ȷ����Ļ��ʾ����
#define ILI9341_DIR   1
/*
*********************************************************************************************************
*	�� �� ��: PortIO_Config
*	����˵��: �������ò���IOΪ��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PortIO_Config(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_ALL_LCD, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode;	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_DB0|GPIO_PIN_DB1|GPIO_PIN_DB2|GPIO_PIN_DB3|
																GPIO_PIN_DB4|GPIO_PIN_DB5|GPIO_PIN_DB6|GPIO_PIN_DB7;
	GPIO_Init(GPIO_PORT_DBL, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_DB8|GPIO_PIN_DB9|GPIO_PIN_DB10|GPIO_PIN_DB11|
																GPIO_PIN_DB12|GPIO_PIN_DB13|GPIO_PIN_DB14|GPIO_PIN_DB15;
	GPIO_Init(GPIO_PORT_DBH, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: WriteData
*	����˵��: ����IOд����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void WriteData(uint16_t data)
{
	uint16_t temp = 0;

	//��8λ
	temp = ((GPIO_PORT_DBL->ODR)&0xff00)|(uint8_t)data;
	GPIO_PORT_DBL->ODR = temp;
	
	//��8λ
	temp = ((GPIO_PORT_DBH->ODR)&0xff00)|(uint8_t)(data>>8);
	GPIO_PORT_DBH->ODR = temp;
}

/*
*********************************************************************************************************
*	�� �� ��: ReadData
*	����˵��: ����IO��ȡ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t ReadData(void)
{	
	uint16_t templ = 0;
	uint16_t temph = 0;
	
	templ = GPIO_PORT_DBL->IDR;
	temph = GPIO_PORT_DBH->IDR;
	
	return ((temph<<8)|templ);
}

/**************************************************************************************************************
 * ��������ILI9341_BGR2RGB()
 * ����  ��u16 c  16bit��ɫ����
 * ���  ���������������ɫ����
 * ����  ������ɫ���к�������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
uint16_t ILI9341_BGR2RGB(uint16_t c)
{
  uint16_t  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);
}	
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLcd
*	����˵��: ��ʼ��LCD
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitLcd(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_ALL_LCD, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); 
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* �������ģʽ */
	
	//RESET
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RESET;
	GPIO_Init(GPIO_PORT_RESET, &GPIO_InitStructure);
	
	//CS
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_CS;
	GPIO_Init(GPIO_PORT_CS, &GPIO_InitStructure);
	
	//RS
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RS;
	GPIO_Init(GPIO_PORT_RS, &GPIO_InitStructure);
	
	//WR
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_WR;
	GPIO_Init(GPIO_PORT_WR, &GPIO_InitStructure);
	
	//RD
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RD;
	GPIO_Init(GPIO_PORT_RD, &GPIO_InitStructure);

	//��������Ϊ���
	PortIO_Config(GPIO_Mode_Out_PP);	
	
	ILI9341_Init();
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_Reset
*	����˵��: ִ�и�λ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_Reset(void)
{ 
	RESET_1();
	bsp_DelayMS(1);
	RESET_0();
	bsp_DelayMS(20);
	RESET_1();
	bsp_DelayMS(20);
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_WriteAddr
*	����˵��: д��ַ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_WriteAddr(uint16_t data)
{ 
	RS_0();
	CS_0();
	WriteData(data);
	WR_0();
	WR_1();
	CS_1();
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_WriteData
*	����˵��: д����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_WriteData(uint16_t data)
{ 
	RS_1();
	CS_0();
	WriteData(data);
	WR_0();
	WR_1();
	CS_1();
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_ReadData
*	����˵��: д����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t ILI9341_ReadData(void)
{ 
	uint16_t temp = 0;
	//��������Ϊ����
	PortIO_Config(GPIO_Mode_IPU);	  
	RS_1();
	CS_0();
	temp = ReadData();
	RD_0();
	RD_1();
	CS_1();
	//��������Ϊ���
	PortIO_Config(GPIO_Mode_Out_PP);	
	return temp;
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_WriteReg
*	����˵��: д����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_WriteReg(uint16_t reg,uint16_t value)
{ 
	ILI9341_WriteAddr(reg);
	ILI9341_WriteData(value);
}


/*
*********************************************************************************************************
*	�� �� ��: ILI9341_ReadReg
*	����˵��: ��ȡ�Ĵ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t ILI9341_ReadReg(uint16_t reg)
{										   
	uint16_t IDH, IDL,buff;

	ILI9341_WriteAddr(reg);   //д��Ҫ���ļĴ�����  
	//��������Ϊ����
	PortIO_Config(GPIO_Mode_IPU);	  

	RS_1();
	CS_0();
				   
	RD_0();				   
	RD_1();   // dummy read
	IDH=ReadData();

	RD_0();				   
	RD_1();   // 0x00
	IDH=ReadData();


	RD_0();					   
	RD_1();   //���� 0x93	
	IDH=ReadData();
	IDH=IDH&0XBF;//�������0XD3,ʵ����0X93�Ŷ�.ǿ��ȥ����6λ   


	RD_0();					   
	RD_1();   //���� 0x41	
  IDL = ReadData();	  
	CS_1(); 
	
	buff  = ((IDH << 8) & 0xff00);
	buff |= (IDL & 0x00ff);  // �ϳ�
	  
	//��������Ϊ���
	PortIO_Config(GPIO_Mode_Out_PP);	
	return buff; 
}

/**************************************************************************************************************
 * ��������ILI9341_WriteRAM_Start()
 * ����  ��void
 * ���  ��void
 * ����  ����ʼдILI9341��RAM
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void ILI9341_WriteRAM_Start(void)
{
  ILI9341_WriteAddr(0x2c);
}

/**************************************************************************************************************
 * ��������ILI9481_SetCursor()
 * ����  ��uint16_t Xpos, uint16_t Ypos �趨����Ļ��Xֵ��Yֵ
 * ���  ��void
 * ����  ��ILI9481���ù��λ�ú���,480*320
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void ILI9341_SetCursor(uint16_t Xpos, uint16_t Ypos,uint8_t _ucDir)
{
	//����
	if((_ucDir == 0)||(_ucDir == 1))
	{
		ILI9341_WriteAddr(0x002A);ILI9341_WriteData(Xpos>>8);
		ILI9341_WriteData(0x00FF&Xpos);                  //�趨X����
		ILI9341_WriteData(0x0001);ILI9341_WriteData(0x003F);

		ILI9341_WriteAddr(0x002B);ILI9341_WriteData(Ypos>>8);
		ILI9341_WriteData(0x00FF&Ypos);                  //�趨Y����
		ILI9341_WriteData(0x0000);ILI9341_WriteData(0x00EF);		
	}
	//����
	else
	{
		ILI9341_WriteAddr(0x002A);ILI9341_WriteData(Xpos>>8);
		ILI9341_WriteData(0x00FF&Xpos);                  //�趨X����
		ILI9341_WriteData(0x0000);ILI9341_WriteData(0x00EF);

		ILI9341_WriteAddr(0x002B);ILI9341_WriteData(Ypos>>8);
		ILI9341_WriteData(0x00FF&Ypos);                  //�趨Y����
		ILI9341_WriteData(0x0001);ILI9341_WriteData(0x003F);	
	}
}

/**************************************************************************************************************
 * ��������ILI9481_SetWindow()
 * ����  ��uint16_t xStar, uint16_t yStar, uint8_t xEnd, uint16_t yEnd �����������ֹ��
 * ���  ��void
 * ����  ������ĳ���ض����������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void ILI9341_SetWindow(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint8_t yEnd)
{
	ILI9341_WriteAddr(0x2A);
	ILI9341_WriteData(xStar>>8);
  ILI9341_WriteData(xStar);                  
  ILI9341_WriteData(xEnd>>8);
	ILI9341_WriteData(xEnd);

  ILI9341_WriteAddr(0x2B);
	ILI9341_WriteData(yStar>>8);
  ILI9341_WriteData(yStar);                 
  ILI9341_WriteData(yEnd>>8);
	ILI9341_WriteData(yEnd);
  ILI9341_SetCursor(xStar, yStar,ILI9341_DIR);
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_SetDispWin
*	����˵��: ������ʾ���ڣ����봰����ʾģʽ��TFT����оƬ֧�ִ�����ʾģʽ�����Ǻ�һ���12864����LCD���������
*	��    ��:
*		_usX : ˮƽ����
*		_usY : ��ֱ����
*		_usHeight: ���ڸ߶�
*		_usWidth : ���ڿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{
	ILI9341_WriteAddr(0X2A); 		/* ����X���� */
	ILI9341_WriteData(_usX >> 8);	/* ��ʼ�� */
	ILI9341_WriteData(_usX);
	ILI9341_WriteData((_usX + _usWidth - 1) >> 8);	/* ������ */
	ILI9341_WriteData(_usX + _usWidth - 1);

	ILI9341_WriteAddr(0X2B); 				  /* ����Y����*/
	ILI9341_WriteData(_usY >> 8);   /* ��ʼ�� */
	ILI9341_WriteData(_usY);
	ILI9341_WriteData((_usY + _usHeight - 1) >>8);		/* ������ */
	ILI9341_WriteData((_usY + _usHeight - 1));
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_Clear
*	����˵��: ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_Clear(uint16_t Color)
{
	uint32_t index=0;      
	ILI9341_SetCursor(0x00,0x00,ILI9341_DIR);     //���ù��λ�� 
	ILI9341_WriteRAM_Start();                     //��ʼд��GRAM	 	  
	for(index=0;index<320*240;index++)
	{
		ILI9341_WriteData(Color);    
	}
}  

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_SetDirection
*	����˵��: ������ʾ����
*	��    ��:  _ucDir : ��ʾ������� 0 ��������, 1=����180�ȷ�ת, 2=����, 3=����180�ȷ�ת
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_SetDirection(uint8_t _ucDir)
{
	ILI9341_WriteAddr(0x36);
	if (_ucDir == 0)
	{
		ILI9341_WriteData(0xa8);	/* ����,DC��ͷ��Ϊ0��*/ 
	}
	else if (_ucDir == 1)
	{
		ILI9341_WriteData(0x68);	/* ����,ָʾ�ƴ�Ϊ0��*/ 
	}
	else if (_ucDir == 2)
	{
		ILI9341_WriteData(0x08);	/* ����,SIM800C��Ϊ0��*/
	}
	else if (_ucDir == 3)
	{
		ILI9341_WriteData(0xc8);	/* ����,SWD�ӿڴ�Ϊ0�� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_Init
*	����˵��: ��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_Init(void)
{ 
	RD_1();
	WR_1();
	RS_1();
	CS_1();
	ILI9341_Reset();
	
	ILI9341_WriteAddr(0xCF);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0xC1);
	ILI9341_WriteData(0X30);
	ILI9341_WriteAddr(0xED);
	ILI9341_WriteData(0x64);
	ILI9341_WriteData(0x03);
	ILI9341_WriteData(0X12);
	ILI9341_WriteData(0X81);
	
	ILI9341_WriteAddr(0xE8);
	ILI9341_WriteData(0x85);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x78);
	
	ILI9341_WriteAddr(0xCB);
	ILI9341_WriteData(0x39);
	ILI9341_WriteData(0x2C);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x34);
	ILI9341_WriteData(0x02);
	
	ILI9341_WriteAddr(0xF7);
	ILI9341_WriteData(0x20);
	
	ILI9341_WriteAddr(0xEA);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x00);
	
	ILI9341_WriteAddr(0xC0); //Power control
	ILI9341_WriteData(0x1D); //VRH[5:0]
	
	ILI9341_WriteAddr(0xC1); //Power control
	ILI9341_WriteData(0x12); //SAP[2:0];BT[3:0]
	
	ILI9341_WriteAddr(0xC5); //VCM control
	ILI9341_WriteData(0x30);
	ILI9341_WriteData(0x3f);
	
	ILI9341_WriteAddr(0xC7); //VCM control
	ILI9341_WriteData(0x90);
	
	ILI9341_WriteAddr(0x3A); // Memory Access Control
	ILI9341_WriteData(0x55);
	
	ILI9341_SetDirection(ILI9341_DIR);

	ILI9341_WriteAddr(0xB1);
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x17);
	
	ILI9341_WriteAddr(0xB6); // Display Function Control
	ILI9341_WriteData(0x0A);
	ILI9341_WriteData(0xA2);//82
	
	ILI9341_WriteAddr(0xF2); // 3Gamma Function Disable
	ILI9341_WriteData(0x00);
	
	ILI9341_WriteAddr(0x26); //Gamma curve selected
	ILI9341_WriteData(0x01);
	
	ILI9341_WriteAddr(0xE0); //Set Gamma
	ILI9341_WriteData(0x0F);
	ILI9341_WriteData(0x22);
	ILI9341_WriteData(0x1C);
	ILI9341_WriteData(0x1B);
	ILI9341_WriteData(0x08);
	ILI9341_WriteData(0x0F);
	ILI9341_WriteData(0x48);
	ILI9341_WriteData(0xB8);
	ILI9341_WriteData(0x34);
	ILI9341_WriteData(0x05);
	ILI9341_WriteData(0x0C);
	ILI9341_WriteData(0x09);
	ILI9341_WriteData(0x0F);
	ILI9341_WriteData(0x07);
	ILI9341_WriteData(0x00);
	
	ILI9341_WriteAddr(0XE1); //Set Gamma
	ILI9341_WriteData(0x00);
	ILI9341_WriteData(0x23);
	ILI9341_WriteData(0x24);
	ILI9341_WriteData(0x07);
	ILI9341_WriteData(0x10);
	ILI9341_WriteData(0x07);
	ILI9341_WriteData(0x38);
	ILI9341_WriteData(0x47);
	ILI9341_WriteData(0x4B);
	ILI9341_WriteData(0x0A);
	ILI9341_WriteData(0x13);
	ILI9341_WriteData(0x06);
	ILI9341_WriteData(0x30);
	ILI9341_WriteData(0x38);
	ILI9341_WriteData(0x0F);
	ILI9341_WriteAddr(0x11); //Exit Sleep
	bsp_DelayMS(100);
	ILI9341_WriteAddr(0x29); //Display on
	
//	ILI9341_Clear(WHITE);
}

 

/**************************************************************************************************************
 * ��������ILI9341_PutPixel()
 * ����  ��int16_t x, int16_t y  �������
 * ���  ��void
 * ����  ����һ�����ص�
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void ILI9341_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{ 
	ILI9341_SetCursor(_usX,_usY,ILI9341_DIR);
	ILI9341_WriteAddr(0x2C);
  ILI9341_WriteData(_usColor);
}

/**************************************************************************************************************
 * ��������ILI9341_GetPixel()
 * ����  ��int16_t x, int16_t y  �������
 * ���  ��uint16_t ���������ص����ɫ
 * ����  ����ȡһ�����ص�
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
uint16_t ILI9341_GetPixel(uint16_t x,uint16_t y)
{
   ILI9341_SetCursor(x,y,ILI9341_DIR);
   return ILI9341_BGR2RGB(ILI9341_ReadData());	
}

/**************************************************************************************************************
 * �������IILI9341_DrawHLine()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint16_t Length ���X��Y���꼰����
 * ���  ��void
 * ����  ����ˮƽ��
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void ILI9341_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usColor)
{
	  uint32_t i = 0;
    ILI9341_SetCursor(_usX1, _usY1,ILI9341_DIR);
    ILI9341_WriteRAM_Start(); /* Prepare to write GRAM */
    for(i = 0; i < _usX2-_usX1 + 1; i++)
    {
      ILI9341_WriteData(_usColor);
    }
}

/**************************************************************************************************************
 * ��������ILI9481_DrawVLine()
 * ����  ��uint16_t Xpos, uint16_t Ypos, uint16_t Length ���X��Y���꼰����
 * ���  ��void
 * ����  ������ֱ��
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void ILI9341_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint16_t _usColor)
{
	uint32_t i = 0;

    for(i = _usY1; i < _usY2; i++)
		{
				ILI9341_PutPixel(_usX1, i, _usColor);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9481_DrawLine
*	����˵��: ���� Bresenham �㷨����2��仭һ��ֱ�ߡ�
*	��    ��:
*			_usX1, _usY1 ����ʼ������
*			_usX2, _usY2 ����ֹ��Y����
*			_usColor     ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
	int32_t dx , dy ;
	int32_t tx , ty ;
	int32_t inc1 , inc2 ;
	int32_t d , iTag ;
	int32_t x , y ;

	/* ���� Bresenham �㷨����2��仭һ��ֱ�� */

	ILI9341_PutPixel(_usX1 , _usY1 , _usColor);

	/* ��������غϣ���������Ķ�����*/
	if ( _usX1 == _usX2 && _usY1 == _usY2 )
	{
		return;
	}

	iTag = 0 ;
	/* dx = abs ( _usX2 - _usX1 ); */
	if (_usX2 >= _usX1)
	{
		dx = _usX2 - _usX1;
	}
	else
	{
		dx = _usX1 - _usX2;
	}

	/* dy = abs ( _usY2 - _usY1 ); */
	if (_usY2 >= _usY1)
	{
		dy = _usY2 - _usY1;
	}
	else
	{
		dy = _usY1 - _usY2;
	}

	if ( dx < dy )   /*���dyΪ�Ƴ������򽻻��ݺ����ꡣ*/
	{
		uint16_t temp;

		iTag = 1 ;
		temp = _usX1; _usX1 = _usY1; _usY1 = temp;
		temp = _usX2; _usX2 = _usY2; _usY2 = temp;
		temp = dx; dx = dy; dy = temp;
	}
	tx = _usX2 > _usX1 ? 1 : -1 ;    /* ȷ������1���Ǽ�1 */
	ty = _usY2 > _usY1 ? 1 : -1 ;
	x = _usX1 ;
	y = _usY1 ;
	inc1 = 2 * dy ;
	inc2 = 2 * ( dy - dx );
	d = inc1 - dx ;
	while ( x != _usX2 )     /* ѭ������ */
	{
		if ( d < 0 )
		{
			d += inc1 ;
		}
		else
		{
			y += ty ;
			d += inc2 ;
		}
		if ( iTag )
		{
			ILI9341_PutPixel ( y , x , _usColor) ;
		}
		else
		{
			ILI9341_PutPixel ( x , y , _usColor) ;
		}
		x += tx ;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_FillRect
*	����˵��: �����Ρ�
*	��    ��:
*			_usX,_usY���������Ͻǵ�����
*			_usHeight �����εĸ߶�
*			_usWidth  �����εĿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	uint32_t i;

	/*
	 ---------------->---
	|(_usX��_usY)        |
	V                    V  _usHeight
	|                    |
	 ---------------->---
		  _usWidth
	*/

	ILI9341_SetDispWin(_usX, _usY, _usHeight, _usWidth);

	ILI9341_WriteRAM_Start(); /* Prepare to write GRAM */
	for (i = 0; i < _usHeight * _usWidth; i++)
	{
		ILI9341_WriteData(_usColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_DrawRect
*	����˵��: ����ˮƽ���õľ��Ρ�
*	��    ��:
*			_usX,_usY���������Ͻǵ�����
*			_usHeight �����εĸ߶�
*			_usWidth  �����εĿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	/*
	 ---------------->---
	|(_usX��_usY)        |
	V                    V  _usHeight
	|                    |
	 ---------------->---
		  _usWidth
	*/

	ILI9341_DrawLine(_usX, _usY, _usX + _usWidth - 1, _usY, _usColor);	/* �� */
	ILI9341_DrawLine(_usX, _usY + _usHeight - 1, _usX + _usWidth - 1, _usY + _usHeight - 1, _usColor);	/* �� */

	ILI9341_DrawLine(_usX, _usY, _usX, _usY + _usHeight - 1, _usColor);	/* �� */
	ILI9341_DrawLine(_usX + _usWidth - 1, _usY, _usX + _usWidth - 1, _usY + _usHeight, _usColor);	/* �� */
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_DrawHColorLine
*	����˵��: ����һ����ɫˮƽ�� ����Ҫ����UCGUI�Ľӿں�����
*	��    ��:  _usX1    ����ʼ��X����
*			  _usY1    ��ˮƽ�ߵ�Y����
*			  _usWidth ��ֱ�ߵĿ��
*			  _pColor : ��ɫ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, const uint16_t *_pColor)
{
	uint16_t i;
	
	ILI9341_SetWindow(_usX1, _usY1, _usX1+1, _usY1+_usWidth);
	

	ILI9341_WriteRAM_Start();

	/* д�Դ� */
	for (i = 0; i <_usWidth; i++)
	{
		ILI9341_WriteData(*_pColor++);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ILI9341_DrawCircle
*	����˵��: ����һ��Բ���ʿ�Ϊ1������
*	��    ��:
*			_usX,_usY  ��Բ�ĵ�����
*			_usRadius  ��Բ�İ뾶
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ILI9341_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{
		ILI9341_PutPixel(_usX + CurX, _usY + CurY, _usColor);
		ILI9341_PutPixel(_usX + CurX, _usY - CurY, _usColor);
		ILI9341_PutPixel(_usX - CurX, _usY + CurY, _usColor);
		ILI9341_PutPixel(_usX - CurX, _usY - CurY, _usColor);
		ILI9341_PutPixel(_usX + CurY, _usY + CurX, _usColor);
		ILI9341_PutPixel(_usX + CurY, _usY - CurX, _usColor);
		ILI9341_PutPixel(_usX - CurY, _usY + CurX, _usColor);
		ILI9341_PutPixel(_usX - CurY, _usY - CurX, _usColor);

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}
