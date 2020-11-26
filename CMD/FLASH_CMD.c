#include "usart.h"
#include "delay.h"
#include "P25QXX.h"
#include "24cxx.h"
#include "qspi.h"
#include<stdio.h>
#include<math.h>
#include<string.h>
#include "OLED_I2C.h"
#include "LTC2631.h"

//***********************************************************************************************************************************//
//###################################################################################################################################//
//***********************************************************************************************************************************//
void FLASHCMDHandler()
{
	u32  i =0;
	u32  m_Value =0;
	u8   bReadByte = 0;
	u8   Error_Code = OK ;
	switch(MCUUART.BufFromUART[1])
	{
		case FLASH_Execute_Step: 
			  for(i=1;i<=cFLASHSTEP.count;i++)
				{
					MCUUART.DataLenFromUART = cFLASHSTEP.STTP[i][2]  +4 ;
					memcpy(&MCUUART.BufFromUART[0],cFLASHSTEP.STTP[i],MCUUART.DataLenFromUART);
					MCUUART.BufFromUART[3] =0;   //清除指令中的step
				  CmdHandler();						
				}
			break;
		 case FLASH_Clear_Config: 
				cFLASHSTEP.count=0;
			  for(i=0;i<MAX_STEP_NUM;i++)
			  {
					 memset(&cFLASHSTEP.STTP[i][0],0,32);				
				}
			break;
		 case FLASH_VTE_TEST: 

		  SPI.ReadInst = MCUUART.BufFromUART[3];
		  SPI_Read_Inst_Parse();
		 	SPI_Process_Set(2);	

		  switch(MCUUART.BufFromUART[4])
			{
					case 0x00://扫描
							if(P25QXX_VT_Test(FTMODE_VTE,0,0) == 0x00)
//						  if(P25QXX_OTP_VT_Test(FTMODE_VTE,0,0) == 0x00)
							{
									m_Value = dectohex((unsigned int)fabs(cVTINFO.Value));	 
									MCUUART.BufToUART[4] = (unsigned int)m_Value >> 8;				
									MCUUART.BufToUART[5] = (unsigned char)m_Value;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[8] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[9] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[10] = cVTINFO.ErrorData;
									MCUUART.BufToUART[11] = cVTINFO.RightData;
									bReadByte = 8;
							}
							else
              {
									m_Value = dectohex((unsigned int)fabs(cVTINFO.Value));	 
									MCUUART.BufToUART[4] = (unsigned int)m_Value >> 8;				
									MCUUART.BufToUART[5] = (unsigned char)m_Value;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[8] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[9] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[10] = cVTINFO.ErrorData;
									MCUUART.BufToUART[11] = cVTINFO.RightData;
									bReadByte = 8;
//									Error_Code =ERROR_READ;
//									bReadByte = 0;
							}
					break;
					case 0x01://点测
						  m_Value = 0 - hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);
					    if(P25QXX_VT_Test(FTMODE_VTE,0x01,m_Value) != 0x00)
							{
								Error_Code =ERROR_READ;
							}
					break;					
			}
		 	SPI_Process_Set(3);	
	 
			MCUUART.BufToUART[0] = MCUUART.BufFromUART[0];								
			MCUUART.BufToUART[1] = MCUUART.BufFromUART[1];			
			MCUUART.BufToUART[2] = Error_Code;									
			MCUUART.BufToUART[3] = bReadByte;		    					

			MCUUART.BufToUART[bReadByte + 4] = GetCheckSum(MCUUART.BufToUART,bReadByte +4 );		//check sum
			SendDatatoUART(MCUUART.BufToUART, bReadByte +4+1);
			break;
		case FLASH_VTP_TEST: 

			SPI.ReadInst = MCUUART.BufFromUART[3];
		  SPI_Read_Inst_Parse();
			SPI_Process_Set(2);	
	
		switch(MCUUART.BufFromUART[4])
			{
					case 0x00://扫描
							if(P25QXX_VT_Test(FTMODE_VTP,0,0) == 0x00)
							{
									m_Value = dectohex((unsigned int)fabs(cVTINFO.Value));	 
									MCUUART.BufToUART[4] = (unsigned int)m_Value >> 8;				
									MCUUART.BufToUART[5] = (unsigned char)m_Value;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[8] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[9] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[10] = cVTINFO.ErrorData;
									MCUUART.BufToUART[11] = cVTINFO.RightData;
									bReadByte = 8;
							}
							else
              {
								  m_Value = dectohex((unsigned int)fabs(cVTINFO.Value));	 
									MCUUART.BufToUART[4] = (unsigned int)m_Value >> 8;				
									MCUUART.BufToUART[5] = (unsigned char)m_Value;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[8] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[9] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[10] = cVTINFO.ErrorData;
									MCUUART.BufToUART[11] = cVTINFO.RightData;
									bReadByte = 8;
//									Error_Code =ERROR_READ;
//									bReadByte = 0;
							}
					break;
					case 0x01://点测
						  m_Value =  hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);
					    if(P25QXX_VT_Test(FTMODE_VTP,0x01,m_Value) != 0x00)
							{
								Error_Code =ERROR_READ;
							}
					break;					
			}	 
		 	SPI_Process_Set(3);	
	
			MCUUART.BufToUART[0] = MCUUART.BufFromUART[0];								
			MCUUART.BufToUART[1] = MCUUART.BufFromUART[1];			
			MCUUART.BufToUART[2] = Error_Code;									
			MCUUART.BufToUART[3] = bReadByte;		    					

			MCUUART.BufToUART[bReadByte + 4] = GetCheckSum(MCUUART.BufToUART,bReadByte +4 );		//check sum
			SendDatatoUART(MCUUART.BufToUART, bReadByte +4+1);
			break;
			case FLASH_OTP_VTE_TEST: 

		  SPI.ReadInst = MCUUART.BufFromUART[3];
		  SPI_Read_Inst_Parse();
		 	SPI_Process_Set(2);	

		  switch(MCUUART.BufFromUART[4])
			{
					case 0x00://扫描
						  if(P25QXX_OTP_VT_Test(FTMODE_VTE,0,0) == 0x00)
							{
									m_Value = dectohex((unsigned int)fabs(cVTINFO.Value));	 
									MCUUART.BufToUART[4] = (unsigned int)m_Value >> 8;				
									MCUUART.BufToUART[5] = (unsigned char)m_Value;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[8] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[9] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[10] = cVTINFO.ErrorData;
									MCUUART.BufToUART[11] = cVTINFO.RightData;
									bReadByte = 8;
							}
							else
              {
									m_Value = dectohex((unsigned int)fabs(cVTINFO.Value));	 
									MCUUART.BufToUART[4] = (unsigned int)m_Value >> 8;				
									MCUUART.BufToUART[5] = (unsigned char)m_Value;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[8] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[9] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[10] = cVTINFO.ErrorData;
									MCUUART.BufToUART[11] = cVTINFO.RightData;
									bReadByte = 8;
//									Error_Code =ERROR_READ;
//									bReadByte = 0;
							}
					break;
					case 0x01://点测
						  m_Value = 0 - hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);
					    if(P25QXX_VT_Test(FTMODE_VTE,0x01,m_Value) != 0x00)
							{
								Error_Code =ERROR_READ;
							}
					break;					
			}
		 	SPI_Process_Set(3);	
	 
			MCUUART.BufToUART[0] = MCUUART.BufFromUART[0];								
			MCUUART.BufToUART[1] = MCUUART.BufFromUART[1];			
			MCUUART.BufToUART[2] = Error_Code;									
			MCUUART.BufToUART[3] = bReadByte;		    					

			MCUUART.BufToUART[bReadByte + 4] = GetCheckSum(MCUUART.BufToUART,bReadByte +4 );		//check sum
			SendDatatoUART(MCUUART.BufToUART, bReadByte +4+1);
			break;
		case FLASH_OTP_VTP_TEST: 

			SPI.ReadInst = MCUUART.BufFromUART[3];
		  SPI_Read_Inst_Parse();
			SPI_Process_Set(2);	
	
		  switch(MCUUART.BufFromUART[4])
			{
					case 0x00://扫描
							if(P25QXX_OTP_VT_Test(FTMODE_VTP,0,0) == 0x00)
							{
									m_Value = dectohex((unsigned int)fabs(cVTINFO.Value));	 
									MCUUART.BufToUART[4] = (unsigned int)m_Value >> 8;				
									MCUUART.BufToUART[5] = (unsigned char)m_Value;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[8] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[9] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[10] = cVTINFO.ErrorData;
									MCUUART.BufToUART[11] = cVTINFO.RightData;
									bReadByte = 8;
							}
							else
              {
								  m_Value = dectohex((unsigned int)fabs(cVTINFO.Value));	 
									MCUUART.BufToUART[4] = (unsigned int)m_Value >> 8;				
									MCUUART.BufToUART[5] = (unsigned char)m_Value;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[8] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[9] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[10] = cVTINFO.ErrorData;
									MCUUART.BufToUART[11] = cVTINFO.RightData;
									bReadByte = 8;
//									Error_Code =ERROR_READ;
//									bReadByte = 0;
							}
					break;
					case 0x01://点测
						  m_Value =  hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);
					    if(P25QXX_VT_Test(FTMODE_VTP,0x01,m_Value) != 0x00)
							{
								Error_Code =ERROR_READ;
							}
					break;					
			}	 
		 	SPI_Process_Set(3);	
	
			MCUUART.BufToUART[0] = MCUUART.BufFromUART[0];								
			MCUUART.BufToUART[1] = MCUUART.BufFromUART[1];			
			MCUUART.BufToUART[2] = Error_Code;									
			MCUUART.BufToUART[3] = bReadByte;		    					

			MCUUART.BufToUART[bReadByte + 4] = GetCheckSum(MCUUART.BufToUART,bReadByte +4 );		//check sum
			SendDatatoUART(MCUUART.BufToUART, bReadByte +4+1);
			break;
		case FLASH_Contimue_TEST: 
			bReadByte = 0;				
			if(MCUUART.BufFromUART[3])// 非0就执行 擦动作
			{
				 if(MCUUART.BufFromUART[4]!=0x81|| \
					  MCUUART.BufFromUART[4]!=0x20|| \
				    MCUUART.BufFromUART[4]!=0x52|| \
					  MCUUART.BufFromUART[4]!=0xd8|| \
				    MCUUART.BufFromUART[4]!=0x60|| \
					  MCUUART.BufFromUART[4]!=0xc7)
				   {//指令错误
					   Error_Code =ERROR_PARAMETERS;
					 }
					 else
					 {
						 SPI_Process_Set(0);					
						 SPI.EraseInst = MCUUART.BufFromUART[4];
						 SPI_Era_Inst_Parse();
						 P25QXX_Enter_TestMode();
			       SetContinueMode();	
						 P25QXX_Erase(SPI.m_Eraser_Operate_Start_addr,SPI.m_Eraser_Operate_Length);
					 }						 
			}
			else//写
			{
				 if(MCUUART.BufFromUART[4]!=0x02|| \
					  MCUUART.BufFromUART[4]!=0xA2|| \
				    MCUUART.BufFromUART[4]!=0x32)
				   {//指令错误
					   Error_Code =ERROR_PARAMETERS;
					 }
					 else
					 {
						SPI_Process_Set(1);					
						SPI.ProgramInst = MCUUART.BufFromUART[4];		       //写指令	
						SPI_Pro_Inst_Parse();
						P25QXX_Enter_TestMode();
						SetContinueMode();	
						P25QXX_Write(SPI.m_Program_Operate_Start_addr,SPI.m_Program_Operate_Length);	
					 }						 
			}
	    
			MCUUART.BufToUART[0] = MCUUART.BufFromUART[0];								
			MCUUART.BufToUART[1] = MCUUART.BufFromUART[1];			
			MCUUART.BufToUART[2] = Error_Code;									
			MCUUART.BufToUART[3] = bReadByte;		    					

			MCUUART.BufToUART[bReadByte + 4] = GetCheckSum(MCUUART.BufToUART,bReadByte +4 );		//check sum
			SendDatatoUART(MCUUART.BufToUART, bReadByte +4+1);
			break;
		 default:
			break;	
	}
		
}

