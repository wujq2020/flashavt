#ifndef _EXTI_H
#define _EXTI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//按键中断						  
////////////////////////////////////////////////////////////////////////////////// 	

void EXTI_Init(void); //外部中断初始化
void Disable_EXTI(void);
extern u8 EXTI_flag;

#endif
