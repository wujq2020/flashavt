#include "LTC2631.h"
#include "delay.h"
#include "usart.h"
#include "key.h"//0702
void LTC2631delay()//0702
{
	u32  i=LtcSpeed;
	while(i--);
}

void LTC2631_GPIO_Configuration(void)
{	
		GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_GPIOB_CLK_ENABLE();														//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;								//PB10，B11
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  							//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         								//上拉
//	  GPIO_Initure.Pull=GPIO_NOPULL;
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     							//初始化PB10，B11
}

void LTC2631_SDA_DIR_IN_FLOAT()
{
		GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_GPIOB_CLK_ENABLE();														//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_7;								//PB10，B11
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  							//推挽输出
//    GPIO_Initure.Pull=GPIO_PULLUP;         								//上拉
	  GPIO_Initure.Pull=GPIO_NOPULL;
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure); 

}

void LTC2631_SDA_DIR_OUT_PP()
{
		GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_GPIOB_CLK_ENABLE();														//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_7;								//PB10，B11
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  							//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         								//上拉
//	  GPIO_Initure.Pull=GPIO_NOPULL;
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure); 

}

void LTC2631_Start(void)//0702
{
	LTC2631delay();
	LTC2631_SDA_Set1;
	LTC2631_SCL_Set1;
	LTC2631delay();
	LTC2631_SDA_Set0;
	LTC2631delay();
	LTC2631_SCL_Set0;
	LTC2631delay();
}


void LTC2631_Stop(void)
{
	LTC2631_SCL_Set0;
	LTC2631_SDA_Set0;
	LTC2631delay();
	LTC2631_SCL_Set1;
	LTC2631delay();
	LTC2631_SDA_Set1;
}

unsigned char LTC2631_TestAck(void)//0702
{
	LTC2631_SCL_Set0;
	LTC2631delay();        
	LTC2631_SCL_Set1;
	LTC2631delay();
	return 1;
}

void LTC2631_ShiftOut8Bit(unsigned char sendbyte)//0702
{
	unsigned char idx;
	
  for(idx = 0; idx < 8; idx++)
	{
		LTC2631_SCL_Set0;
    if(sendbyte & 0x80)
			{LTC2631_SDA_Set1;}
    else
			{LTC2631_SDA_Set0;}
    sendbyte <<= 1;
		LTC2631delay(); 
    LTC2631_SCL_Set1;       
    LTC2631delay();   
  }
	LTC2631_SCL_Set0;
 // return(LTC2631_TestAck());				
}
/******************************************************************/
void LTC2631_Init(void)
{
	LTC2631_Start();
	LTC2631_ShiftOut8Bit(0x20);
	LTC2631_TestAck();
	LTC2631_ShiftOut8Bit(0x60); //internal Ref
	LTC2631_TestAck();
	LTC2631_ShiftOut8Bit(0x00);
	LTC2631_TestAck();
	LTC2631_ShiftOut8Bit(0x00);
	LTC2631_TestAck();
	LTC2631_Stop();
	
	V_Out(0,0);
}

/******************************************************************/
void V_Out(unsigned char V_H,unsigned char V_L)
{
		LTC2631_Start();
		LTC2631_ShiftOut8Bit(0x20);
		LTC2631_TestAck();
		LTC2631_ShiftOut8Bit(0x30); 
		LTC2631_TestAck();
		LTC2631_ShiftOut8Bit((unsigned char)((V_H<<4)|(V_L>>4)));
		LTC2631_TestAck();
		LTC2631_ShiftOut8Bit((unsigned char)((V_L<<4)));
		LTC2631_TestAck();
		LTC2631_Stop();
}

void V_Out_int(int V)
{
		LTC2631_Start();
		LTC2631_ShiftOut8Bit(0x20);
		LTC2631_TestAck();
		LTC2631_ShiftOut8Bit(0x30); 
		LTC2631_TestAck();
		LTC2631_ShiftOut8Bit((unsigned char)(V>>4));
		LTC2631_TestAck();
		LTC2631_ShiftOut8Bit((unsigned char)(V<<4));
		LTC2631_TestAck();
		LTC2631_Stop();
}

void V_Trim()
{
	u16 i,j;
	signed long Trim_Vol_data;
	for(i=0;i<=cVOL_TRIM_INFO.count;i++)
	{
		cVOL_TRIM_INFO.m_Trim_Vol_Step[i] = ((signed long)cVOL_TRIM_INFO.m_Trim_Vol_End[i] - (signed long)cVOL_TRIM_INFO.m_Trim_Vol_Star[i])/((signed long)(cVOL_TRIM_INFO.m_Trim_Vol_Time[i]+1)); //1MS出2个数据
		Trim_Vol_data=cVOL_TRIM_INFO.m_Trim_Vol_Star[i];
		j=cVOL_TRIM_INFO.m_Trim_Vol_Time[i];
		
		V_Out_int(cVOL_TRIM_INFO.m_Trim_Vol_Star[i]);//
		if(i==cVOL_TRIM_INFO.V_C_Step)//给触发信号
		{
//			R14_KEY_L;
		}
		if((signed long)cVOL_TRIM_INFO.m_Trim_Vol_Star[i]!=(signed long)cVOL_TRIM_INFO.m_Trim_Vol_End[i])
		{
			j++;
			while(j--)
			{
				Trim_Vol_data+=cVOL_TRIM_INFO.m_Trim_Vol_Step[i];
				V_Out_int(Trim_Vol_data);
			}
		}
		else//定值
		{
			if(j>3)
				delay_us(50*(j-3));
			if((signed long)cVOL_TRIM_INFO.m_Trim_Vol_Star[i]!=(signed long)cVOL_TRIM_INFO.m_Trim_Vol_Star[i+1])
				delay_us(50);
		}
	}
//	R14_KEY_H;
	cVOL_TRIM_INFO.count=0;
}
