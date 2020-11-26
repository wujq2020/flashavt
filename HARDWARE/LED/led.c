#include "led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_GPIOE_CLK_ENABLE();														
		__HAL_RCC_GPIOA_CLK_ENABLE();														
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1;								
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  							
    GPIO_Initure.Pull=GPIO_PULLUP;         								
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);     							
	
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0,GPIO_PIN_SET);	    //PE0    (R) 
	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_SET);	    //PE1    (G)
	
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;			           
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  							
    GPIO_Initure.Pull=GPIO_PULLUP;         								
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);     							
	
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);	    //PA6   (R)    
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);	    //PA7	 	(G)
}

void IO_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();	
	  __HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();														
		__HAL_RCC_GPIOE_CLK_ENABLE();														
	
	  //BOARD Address  PE5 ~ PE2 
	  GPIO_Initure.Pin= GPIO_PIN_2 |GPIO_PIN_3 |GPIO_PIN_4 |GPIO_PIN_5;			
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  									
    GPIO_Initure.Pull=GPIO_PULLDOWN;         								
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_LOW;  				
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	  //Beep  PA1 
	  GPIO_Initure.Pin= GPIO_PIN_1;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  									
    GPIO_Initure.Pull=GPIO_PULLDOWN;         								
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_LOW;  				
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	
	
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	 
		
		//TX0304 OE  PB0
	  GPIO_Initure.Pin=GPIO_PIN_0;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  									
    GPIO_Initure.Pull=GPIO_NOPULL;         								
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	    
	
		//¼ÌµçÆ÷1 2 3
	  GPIO_Initure.Pin=GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  									
    GPIO_Initure.Pull=GPIO_NOPULL;         								
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15,GPIO_PIN_RESET);
	
	  //POR_CTR PB9
	  GPIO_Initure.Pin=GPIO_PIN_9;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  									
    GPIO_Initure.Pull=GPIO_NOPULL;         								
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
		
		 //V_FLASH_AD-PC4-ADC12_PIN4
    GPIO_Initure.Pin=GPIO_PIN_4;            
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);	
	
	 //V_CS_AD-PA4-ADC12_PIN18
    GPIO_Initure.Pin=GPIO_PIN_4;  
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;		
//    GPIO_Initure.Mode=GPIO_MODE_INPUT;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	
	
	 //V_MCU_AD-PA3-ADC12_PIN15
    GPIO_Initure.Pin=GPIO_PIN_3;            
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	
		
		//V_GND_AD-PA2-ADC12_PIN14
    GPIO_Initure.Pin=GPIO_PIN_2;            
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}
