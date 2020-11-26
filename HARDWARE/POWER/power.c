#include "power.h"
#include "LTC2631.h"
#include "delay.h"
#include "usart.h"  

void P25QXX_Power_On(void)
{
	   GPIO_InitTypeDef GPIO_Initure;	
    //PC11                NCS     (Flash2 初始化)
    GPIO_Initure.Pin=GPIO_PIN_11;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用
    GPIO_Initure.Pull=GPIO_PULLUP;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //高速
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //复用为QSPI   
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //PE7,8,9,10          IO0        IO1        IO2        IO3             
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用
    GPIO_Initure.Pull=GPIO_NOPULL;  	
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //高速
		GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;      //复用为QSPI
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);	
	
	  //PB2                 CLK    
    GPIO_Initure.Pin=GPIO_PIN_2;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用
    GPIO_Initure.Pull=GPIO_NOPULL;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //高速
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //复用为QSPI
    HAL_GPIO_Init(GPIOB,&GPIO_Initure); 

//    V_Out_Ctr(SPI.m_Eraser_Value);
    V_Out_int(SPI.m_Default_Value);
    power_on;
//		delay_ms(200);
}


void P25QXX_Power_Off(void)
{
	   GPIO_InitTypeDef GPIO_Initure;	
    //PC11                NCS     (Flash2 初始化)
    GPIO_Initure.Pin=GPIO_PIN_11;
    GPIO_Initure.Mode=GPIO_MODE_INPUT;             //复用
    GPIO_Initure.Pull=GPIO_PULLDOWN;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //高速 
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //PE7,8,9,10          IO0        IO1        IO2        IO3             
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);	
	
	  //PB2                 CLK    
    GPIO_Initure.Pin=GPIO_PIN_2;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure); 
   
    power_off;	
//  	delay_ms(200);
}
