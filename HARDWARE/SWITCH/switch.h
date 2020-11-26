#ifndef _switch_H
#define _switch_H
#include "sys.h"


void 		SWITCH_Init(void);
void 		Set_WP(u8 Dir);

#define CS_ON_WP 					 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET);  
#define CS_OFF_WP          HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET); 
#define INPUT              0x00
#define OUTPUT             0x01


#endif

