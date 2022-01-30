#include "bsp.h"

/* ����SPI���ߵ� GPIO�˿� */
#define RCC_SCK 	  RCC_APB2Periph_GPIOB
#define PORT_SCK	  GPIOB
#define PIN_SCK		  GPIO_Pin_13

#define RCC_MISO 	  RCC_APB2Periph_GPIOB
#define PORT_MISO	  GPIOB
#define PIN_MISO	  GPIO_Pin_15

#define RCC_MOSI 	  RCC_APB2Periph_GPIOB
#define PORT_MOSI	  GPIOB
#define PIN_MOSI	  GPIO_Pin_14

/* ����Flsh��ƬѡGPIO�˿�  */
#define RCC_NSS 		RCC_APB2Periph_GPIOB
#define PORT_NSS		GPIOB
#define PIN_NSS			GPIO_Pin_12

/* RF������  */
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
*	�� �� ��: bsp_InitRfm69hc
*	����˵��: ��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: MX_GPIO_Init
*	����˵��: IO�˿ڳ�ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO | RCC_NSS, ENABLE);	

	/* ����SPI����SCK��MISO �� MOSIΪ��������ģʽ */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* �������ģʽ */
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
*	�� �� ��: spiRead
*	����˵��: IOģ��SPI����ȡ
*	��    ��: addr:��ֵַ
*	�� �� ֵ: ��ȡ����һ���ֽ�
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
*	�� �� ��: spiWrite
*	����˵��: IOģ��SPI����ָ����ַд����
*	��    ��: addr:��ֵַ
*           val:��Ҫд���ֵ
*	�� �� ֵ: ��
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
*	�� �� ��: spi_MultiRead
*	����˵��: IOģ��SPI����ȡ���ֵ
*	��    ��: addr:��ֵַ
*           p:ָ�򻺳���
*           len:��Ҫ��ȡ�ĳ���
*	�� �� ֵ: ��
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
*	�� �� ��: spi_MultiWrite
*	����˵��: IOģ��SPI��д���ֵ
*	��    ��: addr:��ֵַ
*           p:ָ�򻺳���
*           len:��Ҫд��ĳ���
*	�� �� ֵ: ��
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
*	�� �� ��: spi_readFIFO
*	����˵��: ��ȡ������
*	��    ��: p:ָ�򻺳���
*           len:��Ҫ��ȡ�ĳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void spi_readFIFO(uint8_t *p,uint8_t len)
{
	spi_MultiRead(0x00,p,len);
}

/*
*********************************************************************************************************
*	�� �� ��: spi_writeFIFO
*	����˵��: д��������
*	��    ��: p:ָ�򻺳���
*           len:��Ҫд��ĳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void spi_writeFIFO(uint8_t *p,uint8_t len)
{
	spi_MultiWrite(0x00,p,len);
}

/*
*********************************************************************************************************
*	�� �� ��: SwtichState
*	����˵��: �л�����ģʽ
*	��    ��: mode:ģʽ����
*	�� �� ֵ: ��
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
*	�� �� ��: RFM69H_ClearFIFO
*	����˵��: ���FIFO
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: ManualReset
*	����˵��: ��λģ��
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: spi_init
*	����˵��: spi��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: RxInit
*	����˵��: ���ճ�ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// Receiver init 
void RxInit(void)
{
	RX_FSK_Config();
}

/*
*********************************************************************************************************
*	�� �� ��: TxInit
*	����˵��: ���ͳ�ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
// transmitter init 
void TxInit(void)
{
	TX_FSK_Config();
}


/*
*********************************************************************************************************
*	�� �� ��: TX_FSK_Config
*	����˵��: ��������
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: RX_FSK_Config
*	����˵��: ��������
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: SendMessage
*	����˵��: ������Ϣ
*	��    ��: p:ָ����������ݵĻ�����
*           len:���������ݵĳ���
*	�� �� ֵ: ��
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
*	�� �� ��: GetMessage
*	����˵��: ��ȡ��Ϣ
*	��    ��: p:ָ��������ݻ�����
*           len:���ճ���
*	�� �� ֵ: ��
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
