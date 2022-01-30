#ifndef __BSP_DIN_H
#define __BSP_DIN_H

#define RCC_ALL_DIN 	  (RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB||RCC_APB2Periph_GPIOC)

#define GPIO_PORT_DIN1  GPIOB        //干簧管1，默认输出3.3V
#define GPIO_PIN_DIN1	  GPIO_Pin_3

#define GPIO_PORT_DIN2  GPIOB        //干簧管2，默认输出3.3V
#define GPIO_PIN_DIN2	  GPIO_Pin_4

#define GPIO_PORT_DIN3  GPIOA        //干簧管3，默认输出3.3V
#define GPIO_PIN_DIN3	  GPIO_Pin_11

#define GPIO_PORT_DIN4  GPIOA        //干簧管4，默认输出3.3V
#define GPIO_PIN_DIN4	  GPIO_Pin_12

#define GPIO_PORT_DIN5  GPIOC        //红外1，默认输出0V
#define GPIO_PIN_DIN5	  GPIO_Pin_8

#define GPIO_PORT_DIN6  GPIOC        //红外2，默认输出0V
#define GPIO_PIN_DIN6	  GPIO_Pin_9

#define GPIO_PORT_DIN7  GPIOC        //人体感应，默认输出3.3V
#define GPIO_PIN_DIN7	  GPIO_Pin_7

extern void    bsp_InitDin(void);
extern uint8_t bsp_ReadDin(uint8_t _no);
extern uint8_t bsp_ReadDinAll(void);
#endif
