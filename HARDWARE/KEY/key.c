#include "key.h"
#include "delay.h"
#include "P25QXX.h"
#include "LTC2631.h"
#include "OLED_I2C.h"
#include "usart.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    	
		__HAL_RCC_GPIOA_CLK_ENABLE();          
		__HAL_RCC_GPIOD_CLK_ENABLE();           
	
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12;           
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      
    GPIO_Initure.Pull=GPIO_NOPULL;          
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;     
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	
	
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;               
    GPIO_Initure.Mode=GPIO_MODE_INPUT;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_MEDIUM;     
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);		
	
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY_STOP>KEY_START>KEY_CHECK>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up=1;     //�����ɿ���־
    if(mode==1)key_up=1;    //֧������
    if(key_up&&(KEY_STOP==0||KEY_START==0||KEY_CHECK==0||KEY_ERRRST==0))
    {
        delay_ms(10);
        key_up=0;
        if(KEY_STOP==0)        return KEY_STOP_PRES;
        else if(KEY_START==0)  return KEY_START_PRES;
        else if(KEY_CHECK==0)  return KEY_CHECK_PRES;
        else if(KEY_ERRRST==0) return KEY_ERRRST_PRES;          
    }
		else if(KEY_STOP==1&&KEY_START==1&&KEY_CHECK==1&&KEY_ERRRST==1)
		{	
		    key_up=1;   //�ɿ� key_up��ԭ
		}
    return 0;   //�ް�������
}

void Key_Check(void)                //�ϵ�˳����
{
	u8  key;	
	key = KEY_Scan(0);
	
	if(key==KEY_START_PRES)           //VTP_NO_ERA       
	{
//		OLED_CLS1(4,5,0,132,0x00);
		OLED_CLS1(VTP_Result_ADDR_y,VTE_Result_ADDR_y,OLED_Min_Column,OLED_Max_Column,0x00);
		STATE.BeginCycling =1; 
	}		
	else if(key==KEY_ERRRST_PRES)     //VTE_NO_ERA 
	{
//		OLED_CLS1(4,5,0,132,0x00);	
		OLED_CLS1(VTP_Result_ADDR_y,VTE_Result_ADDR_y,OLED_Min_Column,OLED_Max_Column,0x00);
    STATE.BeginCycling =2;			
	}
	else if(key==KEY_STOP_PRES)       //VTP_VTE_NO_ERA        
	{
//		OLED_CLS1(4,5,0,132,0x00);
		OLED_CLS1(VTP_Result_ADDR_y,VTE_Result_ADDR_y,OLED_Min_Column,OLED_Max_Column,0x00);
    STATE.BeginCycling =3; 		
	}	
	else if(key==KEY_CHECK_PRES)		  //VTP_VTE_ERA 
	{
//    OLED_CLS1(4,5,0,132,0x00);
		OLED_CLS1(VTP_Result_ADDR_y,VTE_Result_ADDR_y,OLED_Min_Column,OLED_Max_Column,0x00);
    STATE.BeginCycling =4; 	
	}		
}
