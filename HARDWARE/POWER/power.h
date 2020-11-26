#ifndef _POWER_H
#define _POWER_H
#include "sys.h"
	

#define power_on                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
#define power_off               HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);

void P25QXX_Power_On(void);
void P25QXX_Power_Off(void);
#endif
