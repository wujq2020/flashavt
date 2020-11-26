#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ADC 电压采集							  
////////////////////////////////////////////////////////////////////////////////// 	
#define VREFINT_CAL  			*(__IO uint16_t *)(0x1FF1E860)   // 校准值
#define VREF  			      2000   // 校准值2000mv



void ADC1_Init(void); 									//ADC通道初始化
void ADC3_Init(void); 				
u16  Get_Adc1(u32 ch); 		        			//获得某个通道值 
u16  Get_Adc3(u32 ch); 		       
u16  Get_Adc1_Average(u32 ch,u8 times); //得到某个通道给定次数采样的平均值
u16  Get_Adc3_Average(u32 ch,u8 times);
u16  Get_Power_Voltage(void);            //获取供电电压
u16  Get_Flash_Voltage(void);            //获取FLAHS供电电压
u16  Get_CS_Voltage(void);
u16  Get_MCU_Voltage(void);
u16  Get_GND_Voltage(void);
#endif 
