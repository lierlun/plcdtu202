#ifndef _BSP_DOUT_H
#define _BSP_DOUT_H

#define RCC_ALL_DOUT 	    (RCC_APB2Periph_GPIOC)

#define GPIO_PORT_DOUT1   GPIOC
#define GPIO_PIN_DOUT1	  GPIO_Pin_12

extern void    bsp_InitDout(void);
extern void    bsp_DoutOn(uint8_t _no);
extern void    bsp_DoutOff(uint8_t _no);
extern void    bsp_DoutToggle(uint8_t _no);
extern uint8_t bsp_IsDoutOn(uint8_t _no);
#endif
