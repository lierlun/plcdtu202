#include "bsp.h"

static void     PortIO_Config(GPIOMode_TypeDef GPIO_Mode);
static void     WriteData(uint16_t data);
static uint16_t ReadData(void);
static uint16_t ILI9341_BGR2RGB(uint16_t c);
static void     ILI9341_Init(void);
static void     ILI9341_WriteRAM_Start(void);
static uint16_t ILI9341_BGR2RGB(uint16_t c);

//确定屏幕显示方向
#define ILI9341_DIR   1
/*
*********************************************************************************************************
*	函 数 名: PortIO_Config
*	功能说明: 用于配置并口IO为输入或输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void PortIO_Config(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 打开GPIO时钟 */
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
*	函 数 名: WriteData
*	功能说明: 并口IO写数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void WriteData(uint16_t data)
{
	uint16_t temp = 0;

	//低8位
	temp = ((GPIO_PORT_DBL->ODR)&0xff00)|(uint8_t)data;
	GPIO_PORT_DBL->ODR = temp;
	
	//高8位
	temp = ((GPIO_PORT_DBH->ODR)&0xff00)|(uint8_t)(data>>8);
	GPIO_PORT_DBH->ODR = temp;
}

/*
*********************************************************************************************************
*	函 数 名: ReadData
*	功能说明: 并口IO读取数据
*	形    参: 无
*	返 回 值: 无
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
 * 函数名：ILI9341_BGR2RGB()
 * 输入  ：u16 c  16bit颜色数据
 * 输出  ：红蓝交换后的颜色数据
 * 描述  ：将颜色进行红蓝交换
 * 调用  ：外部调用        
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
*	函 数 名: bsp_InitLcd
*	功能说明: 初始化LCD
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLcd(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_ALL_LCD, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); 
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */
	
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

	//并口配置为输出
	PortIO_Config(GPIO_Mode_Out_PP);	
	
	ILI9341_Init();
}

/*
*********************************************************************************************************
*	函 数 名: ILI9341_Reset
*	功能说明: 执行复位操作
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: ILI9341_WriteAddr
*	功能说明: 写地址
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: ILI9341_WriteData
*	功能说明: 写数据
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: ILI9341_ReadData
*	功能说明: 写数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t ILI9341_ReadData(void)
{ 
	uint16_t temp = 0;
	//并口配置为输入
	PortIO_Config(GPIO_Mode_IPU);	  
	RS_1();
	CS_0();
	temp = ReadData();
	RD_0();
	RD_1();
	CS_1();
	//并口配置为输出
	PortIO_Config(GPIO_Mode_Out_PP);	
	return temp;
}

/*
*********************************************************************************************************
*	函 数 名: ILI9341_WriteReg
*	功能说明: 写数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_WriteReg(uint16_t reg,uint16_t value)
{ 
	ILI9341_WriteAddr(reg);
	ILI9341_WriteData(value);
}


/*
*********************************************************************************************************
*	函 数 名: ILI9341_ReadReg
*	功能说明: 读取寄存器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t ILI9341_ReadReg(uint16_t reg)
{										   
	uint16_t IDH, IDL,buff;

	ILI9341_WriteAddr(reg);   //写入要读的寄存器号  
	//并口配置为输入
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
	RD_1();   //读到 0x93	
	IDH=ReadData();
	IDH=IDH&0XBF;//这里读回0XD3,实际是0X93才对.强制去掉第6位   


	RD_0();					   
	RD_1();   //读到 0x41	
  IDL = ReadData();	  
	CS_1(); 
	
	buff  = ((IDH << 8) & 0xff00);
	buff |= (IDL & 0x00ff);  // 合成
	  
	//并口配置为输出
	PortIO_Config(GPIO_Mode_Out_PP);	
	return buff; 
}

/**************************************************************************************************************
 * 函数名：ILI9341_WriteRAM_Start()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：开始写ILI9341的RAM
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void ILI9341_WriteRAM_Start(void)
{
  ILI9341_WriteAddr(0x2c);
}

/**************************************************************************************************************
 * 函数名：ILI9481_SetCursor()
 * 输入  ：uint16_t Xpos, uint16_t Ypos 设定的屏幕的X值和Y值
 * 输出  ：void
 * 描述  ：ILI9481设置光标位置函数,480*320
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void ILI9341_SetCursor(uint16_t Xpos, uint16_t Ypos,uint8_t _ucDir)
{
	//横屏
	if((_ucDir == 0)||(_ucDir == 1))
	{
		ILI9341_WriteAddr(0x002A);ILI9341_WriteData(Xpos>>8);
		ILI9341_WriteData(0x00FF&Xpos);                  //设定X坐标
		ILI9341_WriteData(0x0001);ILI9341_WriteData(0x003F);

		ILI9341_WriteAddr(0x002B);ILI9341_WriteData(Ypos>>8);
		ILI9341_WriteData(0x00FF&Ypos);                  //设定Y坐标
		ILI9341_WriteData(0x0000);ILI9341_WriteData(0x00EF);		
	}
	//竖屏
	else
	{
		ILI9341_WriteAddr(0x002A);ILI9341_WriteData(Xpos>>8);
		ILI9341_WriteData(0x00FF&Xpos);                  //设定X坐标
		ILI9341_WriteData(0x0000);ILI9341_WriteData(0x00EF);

		ILI9341_WriteAddr(0x002B);ILI9341_WriteData(Ypos>>8);
		ILI9341_WriteData(0x00FF&Ypos);                  //设定Y坐标
		ILI9341_WriteData(0x0001);ILI9341_WriteData(0x003F);	
	}
}

/**************************************************************************************************************
 * 函数名：ILI9481_SetWindow()
 * 输入  ：uint16_t xStar, uint16_t yStar, uint8_t xEnd, uint16_t yEnd 区域的起点和终止点
 * 输出  ：void
 * 描述  ：设置某个特定的填充区域
 * 调用  ：外部调用        
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
*	函 数 名: ILI9341_SetDispWin
*	功能说明: 设置显示窗口，进入窗口显示模式。TFT驱动芯片支持窗口显示模式，这是和一般的12864点阵LCD的最大区别。
*	形    参:
*		_usX : 水平坐标
*		_usY : 垂直坐标
*		_usHeight: 窗口高度
*		_usWidth : 窗口宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{
	ILI9341_WriteAddr(0X2A); 		/* 设置X坐标 */
	ILI9341_WriteData(_usX >> 8);	/* 起始点 */
	ILI9341_WriteData(_usX);
	ILI9341_WriteData((_usX + _usWidth - 1) >> 8);	/* 结束点 */
	ILI9341_WriteData(_usX + _usWidth - 1);

	ILI9341_WriteAddr(0X2B); 				  /* 设置Y坐标*/
	ILI9341_WriteData(_usY >> 8);   /* 起始点 */
	ILI9341_WriteData(_usY);
	ILI9341_WriteData((_usY + _usHeight - 1) >>8);		/* 结束点 */
	ILI9341_WriteData((_usY + _usHeight - 1));
}

/*
*********************************************************************************************************
*	函 数 名: ILI9341_Clear
*	功能说明: 清屏
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_Clear(uint16_t Color)
{
	uint32_t index=0;      
	ILI9341_SetCursor(0x00,0x00,ILI9341_DIR);     //设置光标位置 
	ILI9341_WriteRAM_Start();                     //开始写入GRAM	 	  
	for(index=0;index<320*240;index++)
	{
		ILI9341_WriteData(Color);    
	}
}  

/*
*********************************************************************************************************
*	函 数 名: ILI9341_SetDirection
*	功能说明: 设置显示方向。
*	形    参:  _ucDir : 显示方向代码 0 横屏正常, 1=横屏180度翻转, 2=竖屏, 3=竖屏180度翻转
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_SetDirection(uint8_t _ucDir)
{
	ILI9341_WriteAddr(0x36);
	if (_ucDir == 0)
	{
		ILI9341_WriteData(0xa8);	/* 横屏,DC插头处为0点*/ 
	}
	else if (_ucDir == 1)
	{
		ILI9341_WriteData(0x68);	/* 横屏,指示灯处为0点*/ 
	}
	else if (_ucDir == 2)
	{
		ILI9341_WriteData(0x08);	/* 竖屏,SIM800C处为0点*/
	}
	else if (_ucDir == 3)
	{
		ILI9341_WriteData(0xc8);	/* 竖屏,SWD接口处为0点 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: ILI9341_Init
*	功能说明: 初始化
*	形    参: 无
*	返 回 值: 无
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
 * 函数名：ILI9341_PutPixel()
 * 输入  ：int16_t x, int16_t y  点的坐标
 * 输出  ：void
 * 描述  ：画一个象素点
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void ILI9341_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{ 
	ILI9341_SetCursor(_usX,_usY,ILI9341_DIR);
	ILI9341_WriteAddr(0x2C);
  ILI9341_WriteData(_usColor);
}

/**************************************************************************************************************
 * 函数名：ILI9341_GetPixel()
 * 输入  ：int16_t x, int16_t y  点的坐标
 * 输出  ：uint16_t 读到的像素点的颜色
 * 描述  ：获取一个象素点
 * 调用  ：外部调用        
 *****************************************************************************************************************/
uint16_t ILI9341_GetPixel(uint16_t x,uint16_t y)
{
   ILI9341_SetCursor(x,y,ILI9341_DIR);
   return ILI9341_BGR2RGB(ILI9341_ReadData());	
}

/**************************************************************************************************************
 * 函数名ILI9341_DrawHLine()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Length 起点X和Y坐标及长度
 * 输出  ：void
 * 描述  ：画水平线
 * 调用  ：外部调用        
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
 * 函数名：ILI9481_DrawVLine()
 * 输入  ：uint16_t Xpos, uint16_t Ypos, uint16_t Length 起点X和Y坐标及长度
 * 输出  ：void
 * 描述  ：画垂直线
 * 调用  ：外部调用        
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
*	函 数 名: ILI9481_DrawLine
*	功能说明: 采用 Bresenham 算法，在2点间画一条直线。
*	形    参:
*			_usX1, _usY1 ：起始点坐标
*			_usX2, _usY2 ：终止点Y坐标
*			_usColor     ：颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
	int32_t dx , dy ;
	int32_t tx , ty ;
	int32_t inc1 , inc2 ;
	int32_t d , iTag ;
	int32_t x , y ;

	/* 采用 Bresenham 算法，在2点间画一条直线 */

	ILI9341_PutPixel(_usX1 , _usY1 , _usColor);

	/* 如果两点重合，结束后面的动作。*/
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

	if ( dx < dy )   /*如果dy为计长方向，则交换纵横坐标。*/
	{
		uint16_t temp;

		iTag = 1 ;
		temp = _usX1; _usX1 = _usY1; _usY1 = temp;
		temp = _usX2; _usX2 = _usY2; _usY2 = temp;
		temp = dx; dx = dy; dy = temp;
	}
	tx = _usX2 > _usX1 ? 1 : -1 ;    /* 确定是增1还是减1 */
	ty = _usY2 > _usY1 ? 1 : -1 ;
	x = _usX1 ;
	y = _usY1 ;
	inc1 = 2 * dy ;
	inc2 = 2 * ( dy - dx );
	d = inc1 - dx ;
	while ( x != _usX2 )     /* 循环画点 */
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
*	函 数 名: ILI9341_FillRect
*	功能说明: 填充矩形。
*	形    参:
*			_usX,_usY：矩形左上角的坐标
*			_usHeight ：矩形的高度
*			_usWidth  ：矩形的宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	uint32_t i;

	/*
	 ---------------->---
	|(_usX，_usY)        |
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
*	函 数 名: ILI9341_DrawRect
*	功能说明: 绘制水平放置的矩形。
*	形    参:
*			_usX,_usY：矩形左上角的坐标
*			_usHeight ：矩形的高度
*			_usWidth  ：矩形的宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	/*
	 ---------------->---
	|(_usX，_usY)        |
	V                    V  _usHeight
	|                    |
	 ---------------->---
		  _usWidth
	*/

	ILI9341_DrawLine(_usX, _usY, _usX + _usWidth - 1, _usY, _usColor);	/* 顶 */
	ILI9341_DrawLine(_usX, _usY + _usHeight - 1, _usX + _usWidth - 1, _usY + _usHeight - 1, _usColor);	/* 底 */

	ILI9341_DrawLine(_usX, _usY, _usX, _usY + _usHeight - 1, _usColor);	/* 左 */
	ILI9341_DrawLine(_usX + _usWidth - 1, _usY, _usX + _usWidth - 1, _usY + _usHeight, _usColor);	/* 右 */
}

/*
*********************************************************************************************************
*	函 数 名: ILI9341_DrawHColorLine
*	功能说明: 绘制一条彩色水平线 （主要用于UCGUI的接口函数）
*	形    参:  _usX1    ：起始点X坐标
*			  _usY1    ：水平线的Y坐标
*			  _usWidth ：直线的宽度
*			  _pColor : 颜色缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, const uint16_t *_pColor)
{
	uint16_t i;
	
	ILI9341_SetWindow(_usX1, _usY1, _usX1+1, _usY1+_usWidth);
	

	ILI9341_WriteRAM_Start();

	/* 写显存 */
	for (i = 0; i <_usWidth; i++)
	{
		ILI9341_WriteData(*_pColor++);
	}
}

/*
*********************************************************************************************************
*	函 数 名: ILI9341_DrawCircle
*	功能说明: 绘制一个圆，笔宽为1个像素
*	形    参:
*			_usX,_usY  ：圆心的坐标
*			_usRadius  ：圆的半径
*	返 回 值: 无
*********************************************************************************************************
*/
void ILI9341_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* 当前 X 值 */
	uint32_t  CurY;		/* 当前 Y 值 */

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
