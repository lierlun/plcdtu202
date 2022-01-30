#include "bsp.h"

/* 定义SPI总线的 GPIO端口 */
#define RCC_SCK 	  RCC_APB2Periph_GPIOB
#define PORT_SCK	  GPIOB
#define PIN_SCK		  GPIO_Pin_13

#define RCC_MISO 	  RCC_APB2Periph_GPIOB
#define PORT_MISO	  GPIOB
#define PIN_MISO	  GPIO_Pin_15

#define RCC_MOSI 	  RCC_APB2Periph_GPIOB
#define PORT_MOSI	  GPIOB
#define PIN_MOSI	  GPIO_Pin_14

/* 串行Flsh的片选GPIO端口  */
#define RCC_NSS 		RCC_APB2Periph_GPIOB
#define PORT_NSS		GPIOB
#define PIN_NSS			GPIO_Pin_12

/* RF有数据  */
#define RCC_RF_INTER 		  RCC_APB2Periph_GPIOA
#define PORT_RF_INTER 		GPIOA
#define PIN_RF_INTER 			GPIO_Pin_15

// spi definition
#define NSS_CLR()  				PORT_NSS->BRR  = PIN_NSS
#define NSS_SET()  				PORT_NSS->BSRR = PIN_NSS

#define MOSI_CLR() 				PORT_MOSI->BRR  = PIN_MOSI
#define MOSI_SET() 				PORT_MOSI->BSRR = PIN_MOSI

#define SCK_CLR()  				PORT_SCK->BRR  = PIN_SCK
#define SCK_SET()  				PORT_SCK->BSRR = PIN_SCK

#define MISO_H()          (GPIO_ReadInputDataBit(PORT_MISO, PIN_MISO) == Bit_SET)
#define RF_INTER_H()      (GPIO_ReadInputDataBit(PORT_RF_INTER, PIN_RF_INTER) == Bit_SET)

/*
*********************************************************************************************************
*	函 数 名: bsp_InitRfm69hc
*	功能说明: 初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitRfm69hc(void)
{
	spi_init();
	ManualReset();
	RxInit();
	TxInit();
	// enter Stdby mode
	SwtichState(OP_MODE_STDBY);
}

/*
*********************************************************************************************************
*	函 数 名: MX_GPIO_Init
*	功能说明: IO端口初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO | RCC_NSS, ENABLE);	

	/* 配置SPI引脚SCK、MISO 和 MOSI为复用推挽模式 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */
	GPIO_InitStructure.GPIO_Pin = PIN_SCK;
	GPIO_Init(PORT_SCK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_MOSI;
	GPIO_Init(PORT_MOSI, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = PIN_NSS;
	GPIO_Init(PORT_NSS, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_InitStructure.GPIO_Pin = PIN_MISO;
	GPIO_Init(PORT_MISO, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: spiRead
*	功能说明: IO模拟SPI，读取
*	形    参: addr:地址值
*	返 回 值: 读取到的一个字节
*********************************************************************************************************
*/
uint8_t spiRead(uint8_t addr)
{
	uint8_t i,val;
	val=0x00;
	//set 0 to MSB for read 
	addr&=0x7f;

	NSS_CLR();
	bsp_DelayUS(2);

	//write address
	for (i=0;i<8;i++)
	{
		if(addr&0x80)
			MOSI_SET();
		else
			MOSI_CLR();

		SCK_CLR();
		bsp_DelayUS(2);

		SCK_SET();
		bsp_DelayUS(2);

		addr<<=1;
	}
	
	//read value
	for (i=0;i<8;i++)
	{
		SCK_CLR();
		bsp_DelayUS(2);

		SCK_SET();
		bsp_DelayUS(2);
		//sample data
		if (MISO_H())
			val|=(0x01<<(7-i));
	}

	//end 
	SCK_CLR();
	bsp_DelayUS(2);
	NSS_SET();

	// return 
	return val;	
}

/*
*********************************************************************************************************
*	函 数 名: spiWrite
*	功能说明: IO模拟SPI，向指定地址写数据
*	形    参: addr:地址值
*           val:将要写入的值
*	返 回 值: 无
*********************************************************************************************************
*/
void spiWrite(uint8_t addr,uint8_t val)
{
	uint8_t i;

	//set 1 to MSB for write
	addr|=0x80;

	NSS_CLR();
	bsp_DelayUS(2);

	//write address
	for (i=0;i<8;i++)
	{
		if(addr&0x80)
			MOSI_SET();
		else
			MOSI_CLR();

		SCK_CLR();
		bsp_DelayUS(2);

		SCK_SET();
		bsp_DelayUS(2);

		addr<<=1;
	}
	
    //write value
	for (i=0;i<8;i++)
	{
		if(val&0x80)
			MOSI_SET();
		else
			MOSI_CLR();

		SCK_CLR();
		bsp_DelayUS(2);

		SCK_SET();
		bsp_DelayUS(2);

		val<<=1;
	}
	
	//end
	SCK_CLR();
	bsp_DelayUS(2);
	NSS_SET();
}
	
/*
*********************************************************************************************************
*	函 数 名: spi_MultiRead
*	功能说明: IO模拟SPI，读取多个值
*	形    参: addr:地址值
*           p:指向缓冲区
*           len:需要读取的长度
*	返 回 值: 无
*********************************************************************************************************
*/
void spi_MultiRead(uint8_t addr,uint8_t *p,uint8_t len)
{
	uint8_t i,k,val;
	val=0x00;
	//set 0 to MSB for read 
	addr&=0x7f;

	NSS_CLR();
	bsp_DelayUS(2);

	//write address
	for (i=0;i<8;i++)
	{
		if(addr&0x80)
			MOSI_SET();
		else
			MOSI_CLR();

		SCK_CLR();
		bsp_DelayUS(2);

		SCK_SET();
		bsp_DelayUS(2);

		addr<<=1;
	}
	// read multiple value
	for (k=0;k<len;k++)
	{
		val=0x00;
		//read value
		for (i=0;i<8;i++)
		{
			SCK_CLR();
			bsp_DelayUS(2);

			SCK_SET();
			bsp_DelayUS(2);
			//sample data
			if (MISO_H())
				val|=(0x01<<(7-i));
		}
		p[k]=val;

	}
	
	//end 
	SCK_CLR();
	bsp_DelayUS(2);
	NSS_SET();


}

/*
*********************************************************************************************************
*	函 数 名: spi_MultiWrite
*	功能说明: IO模拟SPI，写多个值
*	形    参: addr:地址值
*           p:指向缓冲区
*           len:需要写入的长度
*	返 回 值: 无
*********************************************************************************************************
*/
void spi_MultiWrite(uint8_t addr,uint8_t *p,int len)
{
	uint8_t i,k,val;

	//set 1 to MSB for write
	addr|=0x80;

	NSS_CLR();
	bsp_DelayUS(2);

	//write address
	for (i=0;i<8;i++)
	{
		if(addr&0x80)
			MOSI_SET();
		else
			MOSI_CLR();

		SCK_CLR();
		bsp_DelayUS(2);

		SCK_SET();
		bsp_DelayUS(2);

		addr<<=1;
	}
	
	for (k=0;k<len;k++)
	{
		val=p[k];
		//write value
		for (i=0;i<8;i++)
		{
			if(val&0x80)
				MOSI_SET();
			else
				MOSI_CLR();

			SCK_CLR();
			bsp_DelayUS(2);

			SCK_SET();
			bsp_DelayUS(2);

			val<<=1;
		}
		
	}
    
	
	//end
	SCK_CLR();
	bsp_DelayUS(2);
	NSS_SET();
	
}

/*
*********************************************************************************************************
*	函 数 名: spi_readFIFO
*	功能说明: 读取数据流
*	形    参: p:指向缓冲区
*           len:需要读取的长度
*	返 回 值: 无
*********************************************************************************************************
*/
void spi_readFIFO(uint8_t *p,uint8_t len)
{
	spi_MultiRead(0x00,p,len);
}

/*
*********************************************************************************************************
*	函 数 名: spi_writeFIFO
*	功能说明: 写入数据流
*	形    参: p:指向缓冲区
*           len:需要写入的长度
*	返 回 值: 无
*********************************************************************************************************
*/
void spi_writeFIFO(uint8_t *p,uint8_t len)
{
	spi_MultiWrite(0x00,p,len);
}

/*
*********************************************************************************************************
*	函 数 名: SwtichState
*	功能说明: 切换工作模式
*	形    参: mode:模式类型
*	返 回 值: 无
*********************************************************************************************************
*/
// switch the operation mode
void SwtichState(uint8_t mode)
{
	// write mode
	spiWrite(RegOpMode,mode);
	
	// verify the mode
	//val=spiRead(RegIrqFlags1);
  // verify ModeReady
	// Verify the specified mode ready or not
}

/*
*********************************************************************************************************
*	函 数 名: RFM69H_ClearFIFO
*	功能说明: 清空FIFO
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RFM69H_ClearFIFO(void)
{
	// enter Stdby mode
	SwtichState(OP_MODE_STDBY);
	// enter Rx mode
	SwtichState(OP_MODE_RX);
}

/*
*********************************************************************************************************
*	函 数 名: ManualReset
*	功能说明: 复位模块
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// reset rf module
void ManualReset(void)
{
// 	//assert logic high to the reset pin hold at least 100us
// 	RESET_SET();
// 	bsp_DelayUS(200); // delay 200us
//         //delay_us(200); // delay 200us
// 	RESET_CLR();

// 	// delay 5ms 
// 	bsp_DelayUS(5000);
}

/*
*********************************************************************************************************
*	函 数 名: spi_init
*	功能说明: spi初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void spi_init(void)
{
	// mosi,sck,nss,reset output,miso input
	MX_GPIO_Init();
    
	//set output level
	MOSI_SET();// MOSI output high
	SCK_CLR(); // SCK output low
	NSS_SET(); // NSS output high

//	RESET_CLR(); // RESET output low
}

/*
*********************************************************************************************************
*	函 数 名: RxInit
*	功能说明: 接收初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// Receiver init 
void RxInit(void)
{
	RX_FSK_Config();
}

/*
*********************************************************************************************************
*	函 数 名: TxInit
*	功能说明: 发送初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// transmitter init 
void TxInit(void)
{
	TX_FSK_Config();
}


/*
*********************************************************************************************************
*	函 数 名: TX_FSK_Config
*	功能说明: 发送配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
//Transmit Configuration: FSK modulation
void TX_FSK_Config(void)
{
	// FSK modulation
	// set FSK modulation, no shaping ,Sleep mode
	spiWrite(RegOpMode,0x80);
	spiWrite(RegDataModul,0x00);

	//Bit rate is 2.4k  msb is 0x34 . lsb is 0x15
	// 0x3415 is 13333=32M/2.4k
	spiWrite(RegBitrateMsb,0x34);
	spiWrite(RegBitrateLsb,0x15);
    
	//5kHz is 0x52, 35Khz is 0x52*7=0x23E  (00f6)
	spiWrite(RegFdevMsb,0x02);
	spiWrite(RegFdevLsb,0x3E);
	
	// the rf carrier frequency is 868Mhz
	//Fstep=64.0Hz, (868/64)*1024*1024=14221312, the hex is 0xD90000
//	spiWrite(RegFrfMsb,0xD9);
//	spiWrite(RegFrfMid,0x00);
//	spiWrite(RegFrfLsb,0x00);
	// the rf carrier frequency is 434Mhz
	//Fstep=64.0Hz, (434/64)*1024*1024=7110565, the hex is 0x6C8000
	spiWrite(RegFrfMsb,0x6C);
	spiWrite(RegFrfMid,0x80);
	spiWrite(RegFrfLsb,0x00);
	
	// preamble detector is on, preamble detector size is 2byte, PreambleDetectTol is 0x0A
	//spiWrite(RegPreambleDete,0xAA);
	// the preamble size to be sent is 16bytes
	spiWrite(RegPreambleMsb,0x00);
	spiWrite(RegPreambleLsb,0x10);

	// 3byte sync,the value is 1A2B3C
	spiWrite(RegSyncConfig,0x98);

	spiWrite(RegSyncValue1,0x2D);
	spiWrite(RegSyncValue2,0x2D);
	//spiWrite(RegSyncValue3,0xD4);

	// the packet format is variable length, crc off
	spiWrite(RegPacketConfig1,0x00);

	// data processing mode is packet 
	spiWrite(RegPacketConfig2,0x00);


	// the fifo threshold size is 20 bytes, FIFOLevel interrupt
	// when you write 21 bytes to the FIFO, trigger FIFOLEVEL interrupt, and then TxStartCondition is fulfilled
	//spiWrite(RegFifoThresh,0x14);
	spiWrite(RegFifoThresh,0x80);
	
	// set output power to 20dbm for 69HC(PA1+PA2 with high power)
	spiWrite(RegPaLevel,0x7f);
	spiWrite(RegOcp,0x0F);
	spiWrite(RegTestPa1,0x5D);
	spiWrite(RegTestPa2,0x7C);

// set output power to 13dBm
//	spiWrite(RegPaLevel,0x9f);
//	spiWrite(RegOcp,0x0F);
//	spiWrite(RegTestPa1,0x55);
//	spiWrite(RegTestPa2,0x70);

}

/*
*********************************************************************************************************
*	函 数 名: RX_FSK_Config
*	功能说明: 接收配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
// RX FSK config
void RX_FSK_Config(void)
{
	// FSK modulation
	// set FSK modulation, no shaping ,Sleep mode
	spiWrite(RegOpMode,0x80);
	spiWrite(RegDataModul,0x00);
	
	//Bit rate is 2.4k  msb is 0x34 . lsb is 0x15
	// 0x3415 is 13333=32M/2.4k
	spiWrite(RegBitrateMsb,0x34);
	spiWrite(RegBitrateLsb,0x15);


	//5kHz is 0x52, 35Khz is 0x52*7=0x23E
	spiWrite(RegFdevMsb,0x02);
	spiWrite(RegFdevLsb,0x3E);
	
	// the rf carrier frequency is 868Mhz
	//Fstep=64.0Hz, (868/64)*1024*1024=14221312, the hex is 0xD90000
//	spiWrite(RegFrfMsb,0xD9);
//	spiWrite(RegFrfMid,0x00);
//	spiWrite(RegFrfLsb,0x00);

	// the rf carrier frequency is 434Mhz
	//Fstep=64.0Hz, (434/64)*1024*1024=7110565, the hex is 0x6C8000
	spiWrite(RegFrfMsb,0x6C);
	spiWrite(RegFrfMid,0x80);
	spiWrite(RegFrfLsb,0x00);

	//Lna Setting
	spiWrite(RegLna,0x09);

	//RxBw,the default value is 0x15 ,bandwidth is 10k
	// 0x0A is 100K
	spiWrite(RegRxBw,0x0A);

	// 3byte sync,the value is 1A2B3C
	spiWrite(RegSyncConfig,0x98);

	spiWrite(RegSyncValue1,0x2D);
	spiWrite(RegSyncValue2,0x2D);
	//spiWrite(RegSyncValue3,0xD4);

	// the packet format is variable length, crc off
	spiWrite(RegPacketConfig1,0x00);

	// data processing mode is packet 
	spiWrite(RegPacketConfig2,0x00);

	// the payload length is 64 bytes
	//spiWrite(RegPayloadLength,0x40);
	spiWrite(RegPayloadLength,0x40);
}

/*
*********************************************************************************************************
*	函 数 名: SendMessage
*	功能说明: 发送消息
*	形    参: p:指向待发送数据的缓冲区
*           len:待发送数据的长度
*	返 回 值: 无
*********************************************************************************************************
*/
// send message in the packet mode
int16_t SendMessage(uint8_t *p, uint8_t len)
{
	uint8_t  val = 0;
	uint16_t cnt = 0;

	// enter Stdby mode
	SwtichState(OP_MODE_STDBY);	
	
	// write uint8_t to fifo 
	spi_writeFIFO(p,len);

	// verify FifoNotEmpty interrupt
	cnt = 0;
	do {
		val = spiRead(RegIrqFlags2);
		if (val&FifoNotEmpty)
		{
			break;
		}
		
		BSP_OS_TimeDlyMs(1);
		cnt++;
		if(cnt > 100)
		{
			return -1;
    }		
	} while(1);

	// enter Tx mode
	SwtichState(OP_MODE_TX);
	
	//verify ModeReady an TxReady
	cnt = 0;
	do {
		val = spiRead(RegIrqFlags1);
		if ((val&0xA0)==0xA0)
		{
			break;
		}
		
		BSP_OS_TimeDlyMs(1);
		cnt++;
		if(cnt > 100)
		{
			return -1;
    }		
	} while(1);
	
	
	// TxStartCondition is fulfilled, transmitter begin
	// until the packet is sent
	cnt = 0;
	do {
		val = spiRead(RegIrqFlags2);
		if ((val&0x08)==0x08)
		{
			break;
		}
		
		BSP_OS_TimeDlyMs(1);
		cnt++;
		if(cnt > 500)
		{
			return -1;
    }		
	} while(1);
	
	// enter Stdby mode
	SwtichState(OP_MODE_STDBY);	

	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: GetMessage
*	功能说明: 读取消息
*	形    参: p:指向接收数据缓冲区
*           len:接收长度
*	返 回 值: 无
*********************************************************************************************************
*/
//read message in the packet mode
int16_t GetMessage(uint8_t *p,uint8_t len)
{
	uint8_t  val;
	
	// enter Rx mode
	SwtichState(OP_MODE_RX);

	//verify ModeReady an RxReady
	val = spiRead(RegIrqFlags1);
	if ((val&0xC0)==0xC0)
	{
		val = spiRead(RegIrqFlags2);
		if ((val&0x04)==0x04)
		{
			spi_readFIFO(p,len);
			
			RFM69H_ClearFIFO();
			return len;
    }
	}
	return -1;
}
