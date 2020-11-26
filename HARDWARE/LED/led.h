#ifndef _LED_H
#define _LED_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
/************************************************
//LED��������	   
//��������:2019/06/25
//�汾��V1.0			
************************************************/									  
////////////////////////////////////////////////////////////////////////////////// 	

#define CHIP1_RLED_ON    				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
#define CHIP1_RLED_OFF   				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
#define CHIP1_GLED_ON    				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);	
#define CHIP1_GLED_OFF  			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);	

#define MCU_RLED_ON             HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0,GPIO_PIN_RESET);
#define MCU_RLED_OFF            HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0,GPIO_PIN_SET);
#define MCU_RLED_Toggle         HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_0)

#define MCU_GLED_ON             HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_RESET);
#define MCU_GLED_OFF            HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_SET);
#define MCU_GLED_Toggle         HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1)

#define Relay_off               HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_RESET);
#define Relay_on                HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_SET);

#define power_on                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
#define power_off               HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);


#define K1_ON                   HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_SET);
#define K1_OFF                  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_RESET);

#define K2_ON                   HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_SET);
#define K2_OFF                  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_RESET);

#define K3_ON                   HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,GPIO_PIN_SET);
#define K3_OFF                  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,GPIO_PIN_RESET);

#define CS_CS                   {K2_OFF;K3_OFF;}
#define CS_DAC                  {K2_ON;K3_OFF;}   //����CS��ѹ���ӵ�AD5761
#define CS_ADC_POS                  {K1_OFF;K3_ON;K2_OFF;}   //�ɼ�CS��ѹ
#define CS_ADC_NEG                  {K1_ON;K3_ON;K2_OFF;}   //�ɼ�CS��ѹ

#define AD_POS                  K1_ON;          //CS�����ѹ
#define AD_NEG                  K1_OFF;         //CS�����ѹ

#define BEEP_ON 	    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
#define BEEP_OFF  		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	

void LED_Init(void); //LED��ʼ������
void IO_Init(void);  //����IO�ڳ�ʼ��
#endif
