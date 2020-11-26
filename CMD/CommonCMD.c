#include "usart.h"
#include "delay.h"
#include "P25QXX.h"
#include "24cxx.h"
#include "qspi.h"
#include<stdio.h>
#include<math.h>
#include "OLED_I2C.h"
#include "LTC2631.h"
#include "adc.h"
#include "power.h"
#include "exti.h"

u8 VTTestWaiteKeyFlag=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////
void SysDelay(void)
{

}

void timedelay(void)
{
	char i;
	for(i = 0; i< 3; i++)
	SysDelay();
}

void CommonCMDHandler(unsigned char bCmdID)//MCUUART.BufFromUART[1]
{
	u32  bReadByte =0;
	u8   Error_Code = OK ;
	u16  m_PowerVol =0;
	u32  count;
	
	switch(bCmdID)		
	{
		case	SET_FLASH_TYPE:					
//			OLED_CLS();
			SetCfg_1();
//		  P24CXX_WriteLenByte(INIT_CONFIG_1_ADD,&MCUUART.BufFromUART[4],10);
			QSPI_Init();					    
			SPI_Flash_Init();          
			SPI_Flash_Buffer_Init(0xAA,0x55);
		  bReadByte = 0;	
			break;
//		case	SET_FLASH_BPS:	
//      bReadByte = 0;				
//			break;
		case	NOP:	
      bReadByte = 0;		
			break;
		case	POR:									
			P25QXX_Power_Off();
			delay_ms(500);
	    P25QXX_Power_On();
			bReadByte = 0;	
			break;	
		case	DELAY_MS:			
			delay_ms( MCUUART.BufFromUART[4]);
			bReadByte = 0;	
			break;
		case	READ_ID:
			delay_ms(5);
//			P25QXX_TYPE=P25QXX_ReadID();
////			P25QXX_Check_ID();           //读验chipID  检查是否放错位置或放错型号。
//		  MCUUART.BufToUART[4] = P25QXX_TYPE >> 8;				
//			MCUUART.BufToUART[5] = (unsigned char)P25QXX_TYPE;			
		  P25QXX_ReadID(&MCUUART.BufToUART[4]);
		  bReadByte = 2; 
			break;
		case	READ_POWER:
	    m_PowerVol =Get_Flash_Voltage();
		  m_PowerVol = dectohex((unsigned int)m_PowerVol);
	    MCUUART.BufToUART[4] = m_PowerVol >> 8;				
			MCUUART.BufToUART[5] = (unsigned char)m_PowerVol;		
		  bReadByte = 2;		
			break;
		case	POWER_ON:			
			P25QXX_Power_On();		
			bReadByte = 0;	
			break;
		case	POWER_OFF:	
      P25QXX_Power_Off();		
			bReadByte = 0;	
			break;
		case	Firmware_ver:					     
			MCUUART.BufToUART[4] = FIRMWARE_VERSION >> 8;				
			MCUUART.BufToUART[5] = (unsigned char)FIRMWARE_VERSION;	
		  bReadByte = 2;							
			break;
    case	Set_Voltage_Trim_patten:
			bReadByte = 0;	  
		  count = MCUUART.BufFromUART[4];
		  cVOL_TRIM_INFO.m_Trim_Vol_Star[count]= hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]); 
		  cVOL_TRIM_INFO.m_Trim_Vol_End[count]= hextodec(MCUUART.BufFromUART[7], MCUUART.BufFromUART[8]); 
      cVOL_TRIM_INFO.m_Trim_Vol_Time[count]=  MCUUART.BufFromUART[9];
		
		  if(count> cVOL_TRIM_INFO.count)
			{
				 cVOL_TRIM_INFO.count = count;			
			}
			
			break;
		case	Voltage_Trim:
			if(VTTestWaiteKeyFlag)
				while(EXTI_flag);
			EXTI_flag=1;
			bReadByte = 0;
		  if(cVOL_TRIM_INFO.count ==0)
			{
				Error_Code =ERROR_PARAMETERS;		
			}
			else
			{
	     	V_Trim();
			}
			break;
			
			case	Common_Set_Voltage_Trim_step://add0702
				cVOL_TRIM_INFO.V_C_Step=MCUUART.BufFromUART[4];
			break;
			case	VT_Test_Wait_IRQHandler://add0702
				VTTestWaiteKeyFlag=MCUUART.BufFromUART[4];
			break;		
	  default:
			break;
	}
	
		MCUUART.BufToUART[0] = MCUUART.BufFromUART[0];								
		MCUUART.BufToUART[1] = MCUUART.BufFromUART[1];			
		MCUUART.BufToUART[2] = Error_Code;									
		MCUUART.BufToUART[3] = bReadByte;		    					

		MCUUART.BufToUART[bReadByte + 4] = GetCheckSum(MCUUART.BufToUART,bReadByte +4 );		//check sum
		SendDatatoUART(MCUUART.BufToUART, bReadByte +4+1);
}









