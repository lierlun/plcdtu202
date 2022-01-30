#ifndef __ILI9341__H
#define __ILI9341__H
#include "bsp.h"

#define RCC_ALL_LCD 	   (RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD)

#define GPIO_PORT_RESET   GPIOC
#define GPIO_PIN_RESET	  GPIO_Pin_10

#define GPIO_PORT_CS      GPIOC
#define GPIO_PIN_CS	      GPIO_Pin_11

#define GPIO_PORT_RS      GPIOC
#define GPIO_PIN_RS	      GPIO_Pin_12

#define GPIO_PORT_WR      GPIOD
#define GPIO_PIN_WR	      GPIO_Pin_2

#define GPIO_PORT_RD      GPIOB
#define GPIO_PIN_RD	      GPIO_Pin_12

#define GPIO_PORT_DBL     GPIOC
#define GPIO_PIN_DB0	    GPIO_Pin_0
#define GPIO_PIN_DB1	    GPIO_Pin_1
#define GPIO_PIN_DB2	    GPIO_Pin_2
#define GPIO_PIN_DB3	    GPIO_Pin_3
#define GPIO_PIN_DB4	    GPIO_Pin_4
#define GPIO_PIN_DB5	    GPIO_Pin_5
#define GPIO_PIN_DB6	    GPIO_Pin_6
#define GPIO_PIN_DB7	    GPIO_Pin_7

#define GPIO_PORT_DBH     GPIOB
#define GPIO_PIN_DB8	    GPIO_Pin_0
#define GPIO_PIN_DB9	    GPIO_Pin_1
#define GPIO_PIN_DB10	    GPIO_Pin_2
#define GPIO_PIN_DB11	    GPIO_Pin_3
#define GPIO_PIN_DB12	    GPIO_Pin_4
#define GPIO_PIN_DB13	    GPIO_Pin_5
#define GPIO_PIN_DB14	    GPIO_Pin_6
#define GPIO_PIN_DB15	    GPIO_Pin_7


//RESET
#define RESET_0()		      GPIO_PORT_RESET->BRR  = GPIO_PIN_RESET
#define RESET_1()		      GPIO_PORT_RESET->BSRR = GPIO_PIN_RESET
//CS
#define CS_0()		        GPIO_PORT_CS->BRR  = GPIO_PIN_CS
#define CS_1()		        GPIO_PORT_CS->BSRR = GPIO_PIN_CS
//RS
#define RS_0()		        GPIO_PORT_RS->BRR  = GPIO_PIN_RS
#define RS_1()		        GPIO_PORT_RS->BSRR = GPIO_PIN_RS
//WR
#define WR_0()		        GPIO_PORT_WR->BRR  = GPIO_PIN_WR
#define WR_1()		        GPIO_PORT_WR->BSRR = GPIO_PIN_WR
//RD
#define RD_0()		        GPIO_PORT_RD->BRR  = GPIO_PIN_RD
#define RD_1()		        GPIO_PORT_RD->BSRR = GPIO_PIN_RD


//»­±ÊÑÕÉ«
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //×ØÉ«
#define BRRED 			     0XFC07 //×ØºìÉ«
#define GRAY  			     0X8430 //»ÒÉ«

extern void     bsp_InitLcd(void);
extern void     ILI9341_SetDirection(uint8_t _ucDir);
extern void     ILI9341_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
extern uint16_t ILI9341_GetPixel(uint16_t x,uint16_t y);
extern void     ILI9341_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usColor);
extern void     ILI9341_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint16_t _usColor);
extern void     ILI9341_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
extern void     ILI9341_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
extern void     ILI9341_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
extern void     ILI9341_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, const uint16_t *_pColor);
extern void     ILI9341_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
#endif
