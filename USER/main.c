#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"
#include "key.h"
#include "P25QXX.h"
#include "switch.h"
#include "24cxx.h"
#include "adc.h"
#include "exti.h"
#include "OLED_I2C.h"
#include "ad5761r.h"
#include "qspi.h"
#include "LTC2631.h"
#include "ex_p25qxx.h"
#include "power.h"
#include "iic.h"

//#define   READ_PAGE_SIZE        256  //单次读字节数                 		

#define   BOARD_ID              (GPIOE->IDR&0x003C)>>2    //BOARD Address  PE5 ~ PE2 
u8 	McuAddress,BoardID;

int main(void)
{
	int i;
	Cache_Enable();                 
	HAL_Init();				        		  
//	Stm32_Clock_Init(160,5,2,4);    //设置时钟,   Fsys = 400Mhz 
	Stm32_Clock_Init(80,5,4,2);       //设置时钟,   Fsys = 100Mhz    qspi=200
//	Stm32_Clock_Init(80,5,4,1);     //设置时钟,   Fsys = 100Mhz    qspi=400
	delay_init(100);						    
	uart_init(115200);						  
	IO_Init();
	
	LED_Init();								      
	KEY_Init();								      
	P24CXX_Init();                 
	SWITCH_Init(); 
//=======================I2C======================
//  I2C1_Init(); 	
//====================EX_FLASH====================
  W25QXX_Init();	
//	W25QXX_Read(Data_Buffer_1,0,257);//可以读
//======================OLED======================	
	OLED_GPIO_Configuration();  
	OLED_Init();	
	OLED_CLS(); 
	OLED_ShowStr(25,3,"AVT TEST BOARD",1);
	OLED_ShowStr(16,5,"www.puyasemi.com",1);
	delay_ms(1000);
	OLED_CLS(); 	
//====================LTC2631=====================	
	LTC2631_GPIO_Configuration();
	LTC2631_Init();
//=====================AD5761=====================	
	AD5761R_Init();
	cVTINFO_init();                	
//================================================		
//	Para_Init();	                  

//   SPI.Type =0X10;
	P25QXX_Init();                  
  EXTI_Init();                    //更新为按键中断

  while(1)
	{	
	  MCU_GLED_Toggle;				
	  Key_Check();	
//=================	VTP NO ERASER===============//		
    if(STATE.BeginCycling == 1)
		{  
			  //test
			  for(i=1;i<=cFLASHSTEP.count;i++)
				{
					MCUUART.DataLenFromUART = cFLASHSTEP.STTP[i][2]  +4 ;
					memcpy(&MCUUART.BufFromUART[0],cFLASHSTEP.STTP[i],MCUUART.DataLenFromUART);
					MCUUART.BufFromUART[3] =0; //清除指令中的step
				  CmdHandler();						
				}
        STATE.BeginCycling =0;				
		}
//=================	VTE NO ERASER==================//	
		else if(STATE.BeginCycling ==2)
		{			
			  cFLASHSTEP.count=0;
			  for(i=0;i<MAX_STEP_NUM;i++)
			  {
					memset(&cFLASHSTEP.STTP[i][0],0,32);				
				}
        STATE.BeginCycling =0;				
		}	
//===============VTP VTE block ERASER==============//
    else if(STATE.BeginCycling ==3)
		{
        V_Out_int(3600);			
				STATE.BeginCycling =0;			
		}	
//================VTP VTE chip ERASER==============//
    else if(STATE.BeginCycling ==4)
		{				
				STATE.BeginCycling =0;					
		}			
//=======================UART======================//	
		if(USART_RX_STA&0x8000)
		{	
				if(GetCheckSum(MCUUART.BufFromUART,MCUUART.DataLenFromUART-1) != MCUUART.BufFromUART[MCUUART.DataLenFromUART-1]) //check sum error
				{
						SendErrorCodeToPC(ERROR_CHECKSUM);
						return 0;
				}	
				else
				{
						SendErrorCodeToPC(OK); 
				}
			 switch(MCUUART.BufFromUART[0])
			 {	
				 case COMMON_CMD:		
					  CmdHandler();	
				    break;					 
				 case SPI_CMD:
					 if((MCUUART.BufFromUART[1]&0xf0)!=0xf0)
					 {
					    CmdHandler();	
					 }
					 else
					 {
					    FLASHCMDHandler();
					 }
            break;	
				 default:
					  break;				 
			 }
			 USART_RX_STA=0;
		}		
	}	     
}





