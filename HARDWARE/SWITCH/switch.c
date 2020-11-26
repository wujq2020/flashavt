#include "switch.h"
#include "delay.h"
#include "usart.h"

void SWITCH_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;                                         
	  __HAL_RCC_GPIOD_CLK_ENABLE();                                           

  	GPIO_Initure.Pin=GPIO_PIN_15;       																		
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;                                 
    GPIO_Initure.Pull=GPIO_PULLUP;                                          
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;                             
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);	
	  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);                   
}


void Set_WP(u8 Dir)
{
    GPIO_InitTypeDef GPIO_Initure;
		if(Dir==INPUT)
		{		
			GPIO_Initure.Pin=GPIO_PIN_9;
			GPIO_Initure.Mode=GPIO_MODE_INPUT;             //复用
			GPIO_Initure.Pull=GPIO_NOPULL;              
			GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;     //影响过冲
			HAL_GPIO_Init(GPIOE,&GPIO_Initure);	  
		}
		else if(Dir==OUTPUT)
		{
			//PE9                IO2                     
			GPIO_Initure.Pin=GPIO_PIN_9;
			GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用
			GPIO_Initure.Pull=GPIO_NOPULL;              
			GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;     //影响过冲
			GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;      //复用为QSPI
			HAL_GPIO_Init(GPIOE,&GPIO_Initure);	  	
		}
}
