#ifndef _KEY_H
#define _KEY_H
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//KEY驱动代码	   						  
////////////////////////////////////////////////////////////////////////////////// 	

#define KEY_STOP        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)    //KEY_STOP  按键 PA11  STOP    	 B2
#define KEY_START       HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)     //KEY_START 按键 PD8   START      B4
#define KEY_CHECK       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12)    //KEY_CHECK 按键 PA12  CHECK ID   B1
#define KEY_ERRRST      HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9)     //KEY_ERRRST按键 PD9   ERR RST    B3

#define KEY_STOP_PRES 	  1 //KEY_STOP按下后返回值
#define KEY_START_PRES		2	//KEY_START按下后返回值
#define KEY_CHECK_PRES		3	//KEY_CHECK按下后返回值
#define KEY_ERRRST_PRES   4	//KEY_ERRRST按下后返回值

void KEY_Init(void);  //按键IO初始化函数
u8 KEY_Scan(u8 mode); //按键扫描函数
void Key_Check(void);  
#endif
