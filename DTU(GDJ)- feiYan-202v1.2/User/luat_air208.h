#ifndef __LUAT_AIR208_H
#define __LUAT_AIR208_H
#include "bsp.h"

#define RCC_ALL_AIR208 	  (RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC)

//×´Ì¬Ö¸Ê¾
#define GPIO_PORT_STA   GPIOB
#define GPIO_PIN_STA 	  GPIO_Pin_5

#define GPIO_PORT_CTL     GPIOB  //on/off
#define GPIO_PIN_CTL	    GPIO_Pin_6

//µçÔ´¿ª¹Ø
#define GPIO_PORT_POWER   GPIOA
#define GPIO_PIN_POWER  	GPIO_Pin_8

//³¢ÊÔÖØ·¢´ÎÊı
#define TRY_CNT              2

//´®¿Ú¶¨Òå
//208½Ó¿Ú
#define UART208                   COM2
#define UART208_RXBUF             g_RxBuf2
#define UART208_tUART             g_tUart2

//208Ïà¹ØIO
//#define AIR208_STA         1 //(GPIO_ReadInputData(GPIO_PORT_STA)&GPIO_PIN_STA)//ÒòÎª724Ã»ÓĞ×´Ì¬·µ»Ø£¬ËùÒÔÖ±½ÓÖÃ1
#define AIR208_STA         (GPIO_ReadInputData(GPIO_PORT_STA)&GPIO_PIN_STA)//208
//Ä£¿é¿ª¹Ø¿ØÖÆ
#define AIR208_CLT_ON      (GPIO_PORT_CTL->BRR = GPIO_PIN_CTL)
#define AIR208_CLT_OFF     (GPIO_PORT_CTL->BSRR = GPIO_PIN_CTL)
//Ä£¿éµçÔ´¿ª¹Ø¿ØÖÆ
#define AIR208_POWER_ON    (GPIO_PORT_POWER->BSRR = GPIO_PIN_POWER)
#define AIR208_POWER_OFF   (GPIO_PORT_POWER->BRR = GPIO_PIN_POWER)

typedef struct//ĞÂÔö¼Ó£¬ÒòÎªdtuÃ»ÓĞds1302£,ËùÒÔ½á¹¹Ìå·Åµ½´Ë´¦
{
	uint8_t  Year;
	uint8_t  Mon;
	uint8_t  Day;	
	uint8_t  Hour;		
	uint8_t  Min;				
	uint8_t  Sec;					
	uint8_t  Week;	
	uint8_t  DayBak;
}_RTC;


extern void    AIR208_Init(void);
extern void    AIR208_Task(void);
extern uint8_t TransparentTask(void);
extern void    TimingSendFrame(void);
extern void    NetSendTask(void);
extern uint8_t AIR208_Send(uint8_t *pBuf,uint16_t len,uint8_t addr);
extern void		 Parse_208(const char *JSON);
extern void    FyIMEI(uint8_t *pAim, uint8_t array[]);
extern char*   FeiyanIMEI(char *sIMEI, char *dst);
#endif
