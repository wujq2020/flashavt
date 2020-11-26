#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "24cxx.h"
#include "p25qxx.h"
#include "OLED_I2C.h"
//////////////////////////////////////////////////////////////////////////////////	 
//�����ж�����								  
////////////////////////////////////////////////////////////////////////////////// 	
u8 EXTI_flag=1;
//u8 key_value ;
//�ⲿ�жϳ�ʼ��
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
		__HAL_RCC_GPIOA_CLK_ENABLE();                 //����GPIOAʱ��
		__HAL_RCC_GPIOD_CLK_ENABLE();                 //����GPIOBʱ��		
	
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;   //PA11   PA12
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_NOPULL;              //�ⲿ����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;  //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	
	
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;     //PD8   PD9
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_NOPULL;              //�ⲿ����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;  //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);		


    HAL_NVIC_SetPriority(EXTI9_5_IRQn,3,1);     //��ռ���ȼ�Ϊ3�������ȼ�Ϊ1
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);  

    HAL_NVIC_SetPriority(EXTI15_10_IRQn,3,0);   //��ռ���ȼ�Ϊ3�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);  
}

void Disable_EXTI(void)
{
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);          //�ر��ж�
	  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); 
}


//�жϷ�����
void EXTI15_10_IRQHandler(void)
{	
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_11) != RESET)      //GPIO_PIN_11������
	{	
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);    					//�����жϴ����ú���    KEY_STOP
	}
  else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET)
	{
//    delay_ms(20);                                       //not good...
//	  key_value = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12);  
//    if(key_value==0)
//    {
//		 while(key_value==0){key_value = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12);}				
     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);   						//�����жϴ����ú���    KEY_CHECK
//		}
	}
}
void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET)
	{
//    delay_ms(20);                                       
//	  key_value = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8);
//    if(key_value==0)	
//    {	
//     while(key_value==0){key_value = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8);}				
     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);   						//�����жϴ����ú���     KEY_START
//		}
	}
  else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET)
	{	
//    delay_ms(20);                                       
//	  key_value = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9);
//    if(key_value==0)
//		{
//     while(key_value==0){key_value = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9);}			
     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);   						//�����жϴ����ú���     KEY_ERRRST
//	  }
	}
}

//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
			case GPIO_PIN_8:                  //KEY_START

		    EXTI_flag=0;
				break;
			case GPIO_PIN_11:                  //KEY_ERRRST			  //KEY_ERRRST	��  KEY_STOP���ˣ���������
        EXTI_flag=0;				
				break;
			case GPIO_PIN_9:                 //KEY_STOP			
		    EXTI_flag=0; 

				break;
			case GPIO_PIN_12:                 //KEY_CHECK
				if(STATE.BeginCycling != 1)			
				{
				
//				  P25QXX_Check_ID(); 
			
				}					
				break;	
      default:
					break;
   } 	 
}

