
#ifndef __LTC2631_H
#define __LTC2631_H 

#include "sys.h"

#define  LTC2631_SCL_Port          GPIOB
#define  LTC2631_SCL_Pin           GPIO_PIN_6
#define  LTC2631_SDA_Port          GPIOB
#define  LTC2631_SDA_Pin           GPIO_PIN_7

#define LTC2631_SCL_Set1           HAL_GPIO_WritePin(LTC2631_SCL_Port, LTC2631_SCL_Pin,GPIO_PIN_SET);   
#define LTC2631_SCL_Set0           HAL_GPIO_WritePin(LTC2631_SCL_Port, LTC2631_SCL_Pin,GPIO_PIN_RESET);
#define LTC2631_SDA_Set1           HAL_GPIO_WritePin(LTC2631_SDA_Port, LTC2631_SDA_Pin,GPIO_PIN_SET);    
#define LTC2631_SDA_Set0           HAL_GPIO_WritePin(LTC2631_SDA_Port, LTC2631_SDA_Pin,GPIO_PIN_RESET);


#define LTC2631_SDA_READ()         HAL_GPIO_ReadPin(LTC2631_SDA_Port,LTC2631_SDA_Pin)
#define LtcSpeed   12              //设置LTC2631通讯速度 为60时约是400khz 0702
void LTC2631_GPIO_Configuration(void);
void LTC2631_Init(void);
void V_Out(unsigned char V_H,unsigned char V_L);
void V_Out_int(int V);
void V_Trim(void);


void LTC2631delay(void);//0702
#endif

