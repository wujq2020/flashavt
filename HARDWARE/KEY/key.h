#ifndef _KEY_H
#define _KEY_H
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//KEY��������	   						  
////////////////////////////////////////////////////////////////////////////////// 	

#define KEY_STOP        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)    //KEY_STOP  ���� PA11  STOP    	 B2
#define KEY_START       HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)     //KEY_START ���� PD8   START      B4
#define KEY_CHECK       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12)    //KEY_CHECK ���� PA12  CHECK ID   B1
#define KEY_ERRRST      HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9)     //KEY_ERRRST���� PD9   ERR RST    B3

#define KEY_STOP_PRES 	  1 //KEY_STOP���º󷵻�ֵ
#define KEY_START_PRES		2	//KEY_START���º󷵻�ֵ
#define KEY_CHECK_PRES		3	//KEY_CHECK���º󷵻�ֵ
#define KEY_ERRRST_PRES   4	//KEY_ERRRST���º󷵻�ֵ

void KEY_Init(void);  //����IO��ʼ������
u8 KEY_Scan(u8 mode); //����ɨ�躯��
void Key_Check(void);  
#endif
