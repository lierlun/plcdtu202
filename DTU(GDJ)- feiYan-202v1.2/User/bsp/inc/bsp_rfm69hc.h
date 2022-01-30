#ifndef __BSP_RFM96HC_H_
#define __BSP_RFM96HC_H_

#include "bsp.h"


//register configuration
//--------------------------
#define		RegFifo					0x00
#define		RegOpMode				0x01
	
#define OP_MODE_MASK	0x80 // Listen On is off, Sequencer Off is Enable

#define OP_MODE_SLEEP	(0x00<<2)|OP_MODE_MASK
#define OP_MODE_STDBY	(0x01<<2)|OP_MODE_MASK
#define OP_MODE_FS		(0x02<<2)|OP_MODE_MASK
#define OP_MODE_TX		(0x03<<2)|OP_MODE_MASK
#define OP_MODE_RX		(0x04<<2)|OP_MODE_MASK

#define		RegDataModul    0x02

//#define PACKET_MODE (0x00<<5)

#define OOK_MODUL     (0x01<<3)
#define FSK_MODUL	    (0x00<<3)


#define		RegBitrateMsb			0x03
#define		RegBitrateLsb			0x04

#define		RegFdevMsb				0x05
#define		RegFdevLsb				0x06

#define		RegFrfMsb				  0x07
#define		RegFrfMid				  0x08
#define		RegFrfLsb				  0x09

#define 	RegOsc1 				  0x0A 
#define 	RegAfcCtrl	 			0x0B 
#define 	RegListen1 				0x0D 
#define 	RegListen2 				0x0E 
#define 	RegListen3 				0x0F 
#define 	RegVersion 				0x10

// register for transmitter
#define		RegPaLevel				0x11
#define		RegPaRamp				  0x12
#define		RegOcp					  0x13

// register for receiver 
#define		RegLna					  0x18
#define		RegRxBw					  0x19
#define		RegAfcBw				  0x1A
#define		RegOokPeak				0x1B
#define		RegOokAvg				  0x1C
#define		RegOokFix				  0x1D
#define		RegAfcFei				  0x1E
#define		RegAfcMsb				  0x1F
#define		RegAfcLsb				  0x20
#define		RegFeiMsb				  0x21
#define		RegFeiLsb				  0x22
#define		RegRssiConfig			0x23
#define		RegRssiValue			0x24

//IRQ and Pin Mapping
#define 	RegDioMapping1 		0x25 
#define 	RegDioMapping2 		0x26 
#define 	RegIrqFlags1 			0x27 
#define 	RegIrqFlags2 			0x28 
#define 	RegRssiThresh 		0x29 
#define 	RegRxTimeout1 		0x2A 
#define 	RegRxTimeout2 		0x2B 

//Packet Engine
#define		RegPreambleMsb		0x2C
#define		RegPreambleLsb		0x2D

#define		RegSyncConfig			0x2E
#define		RegSyncValue1			0x2F
#define		RegSyncValue2			0x30
#define		RegSyncValue3			0x31
#define		RegSyncValue4			0x32
#define		RegSyncValue5			0x33
#define		RegSyncValue6			0x34
#define		RegSyncValue7			0x35
#define		RegSyncValue8			0x36

#define 	RegPacketConfig1 	0x37 
#define		RegPayloadLength	0x38
#define		RegNodeAdrs				0x39
#define		RegBroadcastAdrs	0x3A
#define 	RegAutoModes 			0x3B 
#define		RegFifoThresh			0x3C
#define		RegPacketConfig2	0x3D
#define 	RegAesKey1				0x3E 
#define 	RegAesKey2				0x3F 
#define 	RegAesKey3				0x40 
#define 	RegAesKey4				0x41 
#define 	RegAesKey5				0x42 
#define 	RegAesKey6				0x43 
#define 	RegAesKey7				0x44 
#define 	RegAesKey8				0x45 
#define 	RegAesKey9				0x46 
#define 	RegAesKey10				0x47 
#define 	RegAesKey11				0x48 
#define 	RegAesKey12				0x49 
#define 	RegAesKey13				0x4A 
#define 	RegAesKey14				0x4B 
#define 	RegAesKey15				0x4C 
#define 	RegAesKey16				0x4D 

//temperature sensor
#define 	RegTemp1 				  0x4E 
#define 	RegTemp2 			  	0x4F 
// Test register
#define 	RegTestLna 				0x58 
#define 	RegTestPa1 				0x5A 
#define 	RegTestPa2 				0x5C 
#define 	RegTestDagc 			0x6F 
#define 	RegTestAfc 				0x71 
//------------
#define		FifoNotEmpty	   (0x01<<6)
#define		FifoLevel	       (0x01<<5)
//------------

uint8_t spiRead(uint8_t addr);//read register 
void    spiWrite(uint8_t addr,uint8_t val);// write value to the register
void    spi_MultiRead(uint8_t addr,uint8_t *p,uint8_t len);// read multiple uint8_t ,the first address is addr
void    spi_MultiWrite(uint8_t addr,uint8_t *p,int len);// write multiple uint8_t ,the first address is addr

void    spi_readFIFO(uint8_t *p,uint8_t len); // read fifo
void    spi_writeFIFO(uint8_t *p,uint8_t len);// write fifo
// 
void    SwtichState(uint8_t mode); // switch operation state
void    ManualReset(void); // Reset RF module
void    spi_init(void); // init spi and reset pin 
void    RxInit(void); // Receiver init 
void    TxInit(void); // transmitter init 
//------------
void    TX_FSK_Config(void);// fsk config
void    RX_FSK_Config(void);// fsk config

//------------
void    MX_GPIO_Init(void);
void    bsp_InitRfm69hc(void);

int16_t SendMessage(uint8_t *p, uint8_t len); // send message in the packet mode
int16_t GetMessage(uint8_t *p,uint8_t len);// get message in the packet mode 
#endif



