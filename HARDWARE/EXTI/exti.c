#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "24cxx.h"
#include "p25qxx.h"
#include "OLED_I2C.h"
//////////////////////////////////////////////////////////////////////////////////	 
//按键中断配置								  
////////////////////////////////////////////////////////////////////////////////// 	
u8 EXTI_flag=1;
//u8 key_value ;
//外部中断初始化
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
		__HAL_RCC_GPIOA_CLK_ENABLE();                 //开启GPIOA时钟
		__HAL_RCC_GPIOD_CLK_ENABLE();                 //开启GPIOB时钟		
	
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;   //PA11   PA12
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_NOPULL;              //外部上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;  //中速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	
	
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;     //PD8   PD9
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_NOPULL;              //外部上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;  //中速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);		


    HAL_NVIC_SetPriority(EXTI9_5_IRQn,3,1);     //抢占优先级为3，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);  

    HAL_NVIC_SetPriority(EXTI15_10_IRQn,3,0);   //抢占优先级为3，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);  
}

void Disable_EXTI(void)
{
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);          //关闭中断
	  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); 
}


//中断服务函数
void EXTI15_10_IRQHandler(void)
{	
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_11) != RESET)      //GPIO_PIN_11被按下
	{	
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);    					//调用中断处理公用函数    KEY_STOP
	}
  else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET)
	{
//    delay_ms(20);                                       //not good...
//	  key_value = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12);  
//    if(key_value==0)
//    {
//		 while(key_value==0){key_value = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12);}				
     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);   						//调用中断处理公用函数    KEY_CHECK
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
     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);   						//调用中断处理公用函数     KEY_START
//		}
	}
  else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET)
	{	
//    delay_ms(20);                                       
//	  key_value = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9);
//    if(key_value==0)
//		{
//     while(key_value==0){key_value = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9);}			
     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);   						//调用中断处理公用函数     KEY_ERRRST
//	  }
	}
}

//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
			case GPIO_PIN_8:                  //KEY_START

		    EXTI_flag=0;
				break;
			case GPIO_PIN_11:                  //KEY_ERRRST			  //KEY_ERRRST	和  KEY_STOP反了！！！！！
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

