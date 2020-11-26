#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ADC ��ѹ�ɼ�							  
////////////////////////////////////////////////////////////////////////////////// 	
#define VREFINT_CAL  			*(__IO uint16_t *)(0x1FF1E860)   // У׼ֵ
#define VREF  			      2000   // У׼ֵ2000mv



void ADC1_Init(void); 									//ADCͨ����ʼ��
void ADC3_Init(void); 				
u16  Get_Adc1(u32 ch); 		        			//���ĳ��ͨ��ֵ 
u16  Get_Adc3(u32 ch); 		       
u16  Get_Adc1_Average(u32 ch,u8 times); //�õ�ĳ��ͨ����������������ƽ��ֵ
u16  Get_Adc3_Average(u32 ch,u8 times);
u16  Get_Power_Voltage(void);            //��ȡ�����ѹ
u16  Get_Flash_Voltage(void);            //��ȡFLAHS�����ѹ
u16  Get_CS_Voltage(void);
u16  Get_MCU_Voltage(void);
u16  Get_GND_Voltage(void);
#endif 
