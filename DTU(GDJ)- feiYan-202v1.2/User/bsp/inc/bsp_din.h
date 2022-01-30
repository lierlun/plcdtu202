#ifndef __BSP_DIN_H
#define __BSP_DIN_H

#define RCC_ALL_DIN 	  (RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB||RCC_APB2Periph_GPIOC)

#define GPIO_PORT_DIN1  GPIOB        //�ɻɹ�1��Ĭ�����3.3V
#define GPIO_PIN_DIN1	  GPIO_Pin_3

#define GPIO_PORT_DIN2  GPIOB        //�ɻɹ�2��Ĭ�����3.3V
#define GPIO_PIN_DIN2	  GPIO_Pin_4

#define GPIO_PORT_DIN3  GPIOA        //�ɻɹ�3��Ĭ�����3.3V
#define GPIO_PIN_DIN3	  GPIO_Pin_11

#define GPIO_PORT_DIN4  GPIOA        //�ɻɹ�4��Ĭ�����3.3V
#define GPIO_PIN_DIN4	  GPIO_Pin_12

#define GPIO_PORT_DIN5  GPIOC        //����1��Ĭ�����0V
#define GPIO_PIN_DIN5	  GPIO_Pin_8

#define GPIO_PORT_DIN6  GPIOC        //����2��Ĭ�����0V
#define GPIO_PIN_DIN6	  GPIO_Pin_9

#define GPIO_PORT_DIN7  GPIOC        //�����Ӧ��Ĭ�����3.3V
#define GPIO_PIN_DIN7	  GPIO_Pin_7

extern void    bsp_InitDin(void);
extern uint8_t bsp_ReadDin(uint8_t _no);
extern uint8_t bsp_ReadDinAll(void);
#endif
