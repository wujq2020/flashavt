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
void SPI_Process_Set(u8 ProcessNum)
{
	switch(ProcessNum)
		{
			  case 0: //era
           V_Out_int(SPI.m_Eraser_Value);
			     QSPI_Init_Customize(SPI.m_Eraser_Speed);   
            break;
        case 1: //pro
           V_Out_int(SPI.m_Program_Value);
			     QSPI_Init_Customize(SPI.m_Program_Speed);   
            break;
        case 2://read
            V_Out_int(SPI.m_Read_Value);
			      QSPI_Init_Customize(SPI.m_Read_Speed);
            break;
        case 3://default
            V_Out_int(SPI.m_Default_Value);
			      QSPI_Init_Customize(SPI.m_Default_Speed);
            break;
        default://default
            V_Out_int(SPI.m_Default_Value);
			      QSPI_Init_Customize(SPI.m_Default_Speed);   
            break;
    }
			delay_ms(100);		
}
void SPI_Era_Inst_Parse()
{
	switch(SPI.EraseInst)
	{			
			case 0x81:  //QPI	
			case 0x20:  //QPI
			case 0x52:  //QPI
			case 0xd8:  //QPI
			case 0x60:  //QPI
			case 0xc7:  //QPI
				  if(P25QXX_QPI_MODE_CHK()) //QPI
					{
							SPI.WrEr_INSTRUCTION_X_LINES = QSPI_INSTRUCTION_4_LINES;
							SPI.WrEr_ADDRESS_X_LINES= QSPI_ADDRESS_4_LINES;	
							SPI.WRITE_DATA_X_LINES = QSPI_DATA_4_LINES;							
					}	
					else
					{
							SPI.WrEr_INSTRUCTION_X_LINES = QSPI_INSTRUCTION_1_LINE;
							SPI.WrEr_ADDRESS_X_LINES= QSPI_ADDRESS_1_LINE;	
							SPI.WRITE_DATA_X_LINES = QSPI_DATA_1_LINE;	
					}	
			break;							
	}
}

void SPI_Pro_Inst_Parse()
{
	switch(SPI.ProgramInst)
	{
			case 0x02:  //QPI
				  if(P25QXX_QPI_MODE_CHK()) //QPI    
					{
							SPI.WrEr_INSTRUCTION_X_LINES = QSPI_INSTRUCTION_4_LINES;
							SPI.WrEr_ADDRESS_X_LINES= QSPI_ADDRESS_4_LINES;	
							SPI.WRITE_DATA_X_LINES = QSPI_DATA_4_LINES;							
					}	
					else
					{
							SPI.WrEr_INSTRUCTION_X_LINES = QSPI_INSTRUCTION_1_LINE;
							SPI.WrEr_ADDRESS_X_LINES= QSPI_ADDRESS_1_LINE;	
							SPI.WRITE_DATA_X_LINES = QSPI_DATA_1_LINE;	
					}	
			break;					
			case 0xa2:
					SPI.WrEr_INSTRUCTION_X_LINES = QSPI_INSTRUCTION_1_LINE;
					SPI.WrEr_ADDRESS_X_LINES= QSPI_ADDRESS_1_LINE;	
					SPI.WRITE_DATA_X_LINES = QSPI_DATA_2_LINES;
			break;
			case 0x32:					
					SPI.WrEr_INSTRUCTION_X_LINES = QSPI_INSTRUCTION_1_LINE;
					SPI.WrEr_ADDRESS_X_LINES= QSPI_ADDRESS_1_LINE;				
					SPI.WRITE_DATA_X_LINES = QSPI_DATA_4_LINES;
					P25QXX_QE_Enable();
			break;			
	}
}

void SPI_Read_Inst_Parse()
{
	switch(SPI.ReadInst)
	{
			case 0x0b:  //QPI	   
					if(P25QXX_QPI_MODE_CHK()) //QPI
					{
							SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_NONE;
							SPI.Read_Dummy              = 8;
							SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_4_LINES;
							SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_4_LINES;
							SPI.Read_DATA_X_LINES       = QSPI_DATA_4_LINES;				
					}	
					else
					{			
							SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_NONE;							
							SPI.Read_Dummy              = 8;
							SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_1_LINE;
							SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_1_LINE;
							SPI.Read_DATA_X_LINES       = QSPI_DATA_1_LINE;
					}
			break;	
			case 0x03:
					SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_NONE;
					SPI.Read_Dummy              = 0;                        
					SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_1_LINE;
					SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_1_LINE;
					SPI.Read_DATA_X_LINES       = QSPI_DATA_1_LINE;
			break;
			case 0x3b:
					SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_NONE;
					SPI.Read_Dummy              = 8;
					SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_1_LINE;
					SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_1_LINE;
					SPI.Read_DATA_X_LINES       = QSPI_DATA_2_LINES;
			break;
			case 0xbb:
					SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_2_LINES;
					SPI.Read_Dummy              = 0;
					SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_1_LINE;
					SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_2_LINES;
					SPI.Read_DATA_X_LINES       = QSPI_DATA_2_LINES;	
			break;					
			case 0x6b:
					SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_NONE;
					SPI.Read_Dummy              = 8;
					SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_1_LINE;
					SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_1_LINE;
					SPI.Read_DATA_X_LINES       = QSPI_DATA_4_LINES;	
					P25QXX_QE_Enable();
			break;					
			case 0xeb:  //QPI
					if(P25QXX_QPI_MODE_CHK()) //QPI
					{
							SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_4_LINES;
							SPI.Read_Dummy              = 6;
							SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_4_LINES;
							SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_4_LINES;
							SPI.Read_DATA_X_LINES       = QSPI_DATA_4_LINES;						
					}	
					else
					{		
							SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_4_LINES;
							SPI.Read_Dummy              = 4;
							SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_1_LINE;
							SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_4_LINES;
							SPI.Read_DATA_X_LINES       = QSPI_DATA_4_LINES;	
							P25QXX_QE_Enable();	
					}
			break;
      case 0xd7:
					SPI.Read_ALTERNATE_X_LINES  = QSPI_ALTERNATE_BYTES_NONE;
					SPI.Read_Dummy              = 0;
					SPI.Read_INSTRUCTION_X_LINES= QSPI_INSTRUCTION_1_LINE;
					SPI.Read_ADDRESS_X_LINES    = QSPI_ADDRESS_1_LINE;
					SPI.Read_DATA_X_LINES       = QSPI_DATA_1_LINE;	
			break;									
	}
}


void SPICMDHandler(unsigned char bCmdID)
{
	u32  bReadByte =0;
	u32  iAddrStar =0;
	u8   DummyClk=0;
	u16  count = 0 ;
	u8   Error_Code = OK ;
	u32  clk_pre;	
	u32 Start_addr=0,End_addr=0;
		
	double clk_f;
	switch(bCmdID)
	{
		case SPI_CS: 
			break;
		case SPI_SO:
			break;
		case SPI_WP:
			break;
		case SPI_SI:
			break;
		case SPI_SK:
			break; 		
		case SPI_HOLD:
			break; 		
		case SPI_ShiftOutNBit:		
			break; 			
		case SPI_ShiftInNBit:			  //以上预留
			break; 		
		case	SPI_Set_Mode:
      bReadByte = 0;
		  if(QSPI_Init_Mode(MCUUART.BufFromUART[4]))
		   {   Error_Code = ERROR_PARAMETERS; }  
	    break;			
		case	SPI_Enter_TM:
      bReadByte = 0;
		  P25QXX_Enter_TestMode();
	    break;			
		case	SPI_Exit_TM:
			bReadByte = 0;
      P25QXX_Exit_TestMode();  
	    break;			
    case	SPI_Set_SPEED:
		  clk_f = hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);
      clk_f =2000/clk_f;
      clk_pre =(u32)	(clk_f +0.5);
			switch(MCUUART.BufFromUART[4])  //200M/clk_pre 即为实际频率
			{
					case 0x00:		 
							 SPI.m_Eraser_Speed = clk_pre-1;  
							 break;
					case 0x01:
							 SPI.m_Program_Speed = clk_pre-1;
							 break;
					case 0x02:
							 SPI.m_Read_Speed =    clk_pre-1;
							 break;
					case 0x03:
							 SPI.m_Default_Speed = clk_pre-1;
							 break;
			}			
      MCUUART.BufToUART[4] = clk_pre;				
	 	  bReadByte = 1;
			break;	 	 
		case	SPI_Set_Voltage:	
      switch(MCUUART.BufFromUART[4])
			{
					case 0x00:		 
					     SPI.m_Eraser_Value = hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);  
					     break;
					case 0x01:
					     SPI.m_Program_Value = hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);
					     break;
					case 0x02:
					     SPI.m_Read_Value = hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);
					     break;
					case 0x03:
					     SPI.m_Default_Value = hextodec(MCUUART.BufFromUART[5], MCUUART.BufFromUART[6]);
					     break;
			}
			bReadByte = 0;	
			break;		
		case	SPI_Set_Address:
      Merge_data(&MCUUART.BufFromUART[5],&iAddrStar);					
			Start_addr = iAddrStar;		
		  Merge_data(&MCUUART.BufFromUART[9],&iAddrStar);	
      if(iAddrStar>=FlashSize) 	iAddrStar=FlashSize-1;	
			End_addr = iAddrStar;		
		  if(Start_addr>End_addr) 
			{				
					Error_Code =ERROR_PARAMETERS;	
					bReadByte = 0;	
					break;
			}				
      switch(MCUUART.BufFromUART[4])
			{
					case 0x00:		 
					     SPI.m_Eraser_Operate_Start_addr = Start_addr;
					     SPI.m_Eraser_Operate_End_addr = End_addr;
					     SPI.m_Eraser_Operate_Length	=	 End_addr - Start_addr+1;
					     break;
					case 0x01:
					     SPI.m_Program_Operate_Start_addr = Start_addr;
					     SPI.m_Program_Operate_End_addr = End_addr;
				       SPI.m_Program_Operate_Length	=	 End_addr - Start_addr+1;
					     break;
					case 0x02:
				       SPI.m_Read_Operate_Start_addr = Start_addr;
					     SPI.m_Read_Operate_End_addr = End_addr;
               SPI.m_Read_Operate_Length	=	 End_addr - Start_addr+1;
					     break;
			}
			bReadByte = 0;	
			break;	
		case	SPI_Polling:
			if( MCUUART.BufFromUART[4]==0)
			{	
					SPI.Polling_flag = 0;
			}
			else
			{
					SPI.Polling_flag = 1;
			}
			bReadByte = 0;	
			break;				
		case	SPI_Write_length:				
			count = MCUUART.BufFromUART[4]*256+MCUUART.BufFromUART[5];
			if((256%count) ||(count==0))//单次写的字节数不能被256整除，判错
			{
					Error_Code =ERROR_PARAMETERS;	
			}
			else
			{
					WritePageSize = count;			
			}
			bReadByte = 0;	
			break;			
		case	SPI_Read_length:
			ReadPageSize = MCUUART.BufFromUART[4]*256+MCUUART.BufFromUART[5];
			if(ReadPageSize==0)//单次写的字节数不能被256整除，判错
			{
					Error_Code =ERROR_PARAMETERS;	
			}
			bReadByte = 0;	
			break;		
		case	SPI_Set_Pattern:  
			switch(MCUUART.BufFromUART[4])
			{
					case 0x00:	//内部棋盘数据	
							 SPI.m_Pattern_Select =PATTERN_CKB;	
							 SPI_Flash_Buffer_Init(MCUUART.BufFromUART[5],MCUUART.BufFromUART[6]);							
							 break;
					case 0x01:  //内部随机数
							 SPI.m_Pattern_Select =PATTERN_RANDOM;	
							 break;
					case 0x02:  //内部自定义
							 SPI.m_Pattern_Select =PATTERN_CUSTOM;
							 Merge_data(&MCUUART.BufFromUART[5],&iAddrStar);					
							 Start_addr = iAddrStar;

							 Merge_data(&MCUUART.BufFromUART[9],&iAddrStar);	
							 End_addr = iAddrStar;	

							 if( (Start_addr>=End_addr) ||	(Start_addr>=1024) ||	(End_addr>=1024))
							 {
									Error_Code =ERROR_PARAMETERS;
									break;								 
							 }								 
							 SPI_Flash_Customs_Buffer_Init(Start_addr,End_addr,&MCUUART.BufFromUART[13]);					     
							 break;
					case 0x03:  //外部flash
							 SPI.m_Pattern_Select =PATTERN_FLASH;	
							 Merge_data(&MCUUART.BufFromUART[5],&iAddrStar);					
							 Flash_Star_Addr = iAddrStar;

							 Merge_data(&MCUUART.BufFromUART[9],&iAddrStar);	
							 Flash_End_Addr = iAddrStar;
							 if( Flash_Star_Addr>=Flash_End_Addr ) 
							 {
									Error_Code =ERROR_PARAMETERS;
									break;								 
							 }						     
							 break;
			}
			bReadByte = 0;	
			break;
		case	SPI_Set_Back_Data:
			memcpy(Data_Back_Buffer_1K,&MCUUART.BufFromUART[4],MCUUART.BufFromUART[2]-1);	
			bReadByte = 0;	
			break;
		case	SPI_TestMode:
			bReadByte = 0;
			switch(MCUUART.BufFromUART[4])
			{
					case 0x00: //NORMAL 				
							 break;	
					case 0x01: //testmode 				
							 break;						
					default:
					break;	
			}				
			break;
		case	SPI_IoMode://00-SPI, 01-QPI
			bReadByte = 0;
			switch(MCUUART.BufFromUART[4])
			{
					case 0x00: //硬件spi  						 
						if(P25QXX_QPI_MODE != 0)    //退出QPI
						{
								P25QXX_Qspi_Disable();
						}					 									
						break;	
					case 0x01:                      //QSPI
						P25QXX_Qspi_Enable();		      //使能QSPI模式						
						P25QXX_Write_Enable();		    //写使能  => 设置dummy
						delay_ms(5);
						QSPI_Send_CMD(P25X_SetReadParam,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES); 		//QPI,设置读参数指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
						DummyClk=3<<4;					      //设置P4&P5=11,8个dummy clocks,104M
						QSPI_Transmit(&DummyClk,1);		//发送1个字节						
						break;
					case 0x02: //模拟spi 				
						break;						
					default:
						 break;	
			}
			break;
		case	SPI_Set_Cmd:
			bReadByte = 0;
      switch(MCUUART.BufFromUART[4])
			{
					case 0x0b:  //QPI
					case 0x03:
					case 0x3b:						
					case 0xbb:									
					case 0x6b:						
					case 0xeb:  //QPI
					  SPI_Process_Set(2);
						SPI.ReadInst =	MCUUART.BufFromUART[4];						
					  SPI_Read_Inst_Parse();				
						P25QXX_Read_No_Verify(datatemp,SPI.m_Read_Operate_Start_addr,SPI.m_Read_Operate_Length); 										
            if(SPI.m_Read_Operate_Length>128)
						{
							 bReadByte = 128;
						}
						else
						{
						   bReadByte = SPI.m_Read_Operate_Length;
						}
						memcpy(&MCUUART.BufToUART[4],datatemp,bReadByte);
					  break;	
					case 0xD7:  //TEST MODE			
					  SPI_Process_Set(2);
						SPI.ReadInst =	MCUUART.BufFromUART[4];						
					  SPI_Read_Inst_Parse();				
					  P25QXX_Enter_TestMode();
						P25QXX_Read_No_Verify(datatemp,SPI.m_Read_Operate_Start_addr,SPI.m_Read_Operate_Length); 	
					  P25QXX_Exit_TestMode();								
            if(SPI.m_Read_Operate_Length>128)
						{
							 bReadByte = 128;
						}
						else
						{
						   bReadByte = SPI.m_Read_Operate_Length;
						}
						memcpy(&MCUUART.BufToUART[4],datatemp,bReadByte);
					  break;	        	
					case 0x81:  //QPI	
					case 0x20:  //QPI
					case 0x52:  //QPI
					case 0xd8:  //QPI
					case 0x60:  //QPI
					case 0xc7:  //QPI
					 SPI_Process_Set(0);					
					 SPI.EraseInst = MCUUART.BufFromUART[4];
					 SPI_Era_Inst_Parse();
					 P25QXX_Erase(SPI.m_Eraser_Operate_Start_addr,SPI.m_Eraser_Operate_Length);
					 bReadByte = 0;	
					 break;		
					case 0x02://QPI
					case 0xa2:
					case 0x32:	
						SPI_Process_Set(1);					
						SPI.ProgramInst = MCUUART.BufFromUART[4];		       //写指令	
						SPI_Pro_Inst_Parse();
						P25QXX_Write(SPI.m_Program_Operate_Start_addr,SPI.m_Program_Operate_Length);	
						bReadByte = 0;	
						break;						
					case 0x75://QPI  
					case 0xb0://QPI				
					case 0x7a://QPI  
					case 0x30://QPI
						SPI_Process_Set(3);					
					  P25QXX_SendCMD1(MCUUART.BufFromUART[4]); 
						break;
		      //protection
					case 0x06://QPI		
					case 0x04://QPI					
					case 0x50://QPI	
						SPI_Process_Set(3);					
					  P25QXX_SendCMD1(MCUUART.BufFromUART[4]);
						break;					
					case 0x36://QPI	
					case 0x39://QPI
						SPI_Process_Set(3);
            Merge_data(&MCUUART.BufFromUART[5],&iAddrStar);					
			      Start_addr = iAddrStar;								
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(MCUUART.BufFromUART[4],Start_addr,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_NONE);	
							else QSPI_Send_CMD(MCUUART.BufFromUART[4],Start_addr,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_NONE);									
						bReadByte=0;						
    				break;
					case 0x3C://QPI						
					case 0x3D://QPI				
            SPI_Process_Set(3);
            Merge_data(&MCUUART.BufFromUART[5],&iAddrStar);					
			      Start_addr = iAddrStar;								
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(MCUUART.BufFromUART[4],Start_addr,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_4_LINES);	
							else QSPI_Send_CMD(MCUUART.BufFromUART[4],Start_addr,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);	
            QSPI_Receive(&MCUUART.BufToUART[4],1);						
						bReadByte=1;						
    				break;
					case 0x7E://QPI					
					case 0x98://QPI					
						SPI_Process_Set(3);					
					  P25QXX_SendCMD1(MCUUART.BufFromUART[4]);
						break;					
					case 0x44:					
						SPI_Process_Set(3);
            Merge_data(&MCUUART.BufFromUART[5],&iAddrStar);					
			      Start_addr = iAddrStar;								
					  QSPI_Send_CMD(MCUUART.BufFromUART[4],Start_addr,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_NONE);									
						bReadByte=0;						
    				break;
					case 0x42:
            SPI_Process_Set(3);
            Merge_data(&MCUUART.BufFromUART[5],&iAddrStar);					
			      Start_addr = iAddrStar;								
						QSPI_Send_CMD(MCUUART.BufFromUART[4],Start_addr,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);	
            QSPI_Transmit(&MCUUART.BufFromUART[9],MCUUART.BufFromUART[2]-6);						
						bReadByte=0;						
    				break;						
					case 0x48:					
            SPI_Process_Set(3);
            Merge_data(&MCUUART.BufFromUART[5],&iAddrStar);					
			      Start_addr = iAddrStar;	
						bReadByte=MCUUART.BufFromUART[9];					
						QSPI_Send_CMD(MCUUART.BufFromUART[4],Start_addr,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);	
            QSPI_Receive(&MCUUART.BufToUART[4],bReadByte);												
    				break;
					case 0x05://QPI		
						SPI_Process_Set(3);	
						MCUUART.BufToUART[4]=P25QXX_ReadSR(1);	
						bReadByte=1;				
						break;
					case 0x35://QPI					
						SPI_Process_Set(3);						
						MCUUART.BufToUART[4]=P25QXX_ReadSR(2);	
						bReadByte=1;
						break;
					case 0x15://QPI				
						SPI_Process_Set(3);						
						MCUUART.BufToUART[4]=P25QXX_ReadSR(3);	
						bReadByte=1;
						break;				
					case 0x25:					
						SPI_Process_Set(3);							
            QSPI_Send_CMD(0X25,0,8, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_24_BITS,QSPI_DATA_NONE);					
						bReadByte=0;						
    				break;
					case 0x01://QPI
						SPI_Process_Set(3);							
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(P25X_WriteStatusReg1,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);	
							else QSPI_Send_CMD(P25X_WriteStatusReg1,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				
							QSPI_Transmit(&MCUUART.BufFromUART[5],MCUUART.BufFromUART[2]-2);				
						bReadByte=0;
						break;
					case 0x31://QPI  
						SPI_Process_Set(3);						
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(P25X_WriteStatusReg2,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);
							else QSPI_Send_CMD(P25X_WriteStatusReg2,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);			
							QSPI_Transmit(&MCUUART.BufFromUART[5],1);
						bReadByte=0;
						break;
					case 0x11://QPI					
						SPI_Process_Set(3);						
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(P25X_WriteStatusReg3,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);	
						else QSPI_Send_CMD(P25X_WriteStatusReg3,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				
						QSPI_Transmit(&MCUUART.BufFromUART[5],1);
						bReadByte=0;
						break;
					case 0x66://QPI
					case 0x99://QPI	
					case 0x38:
						SPI_Process_Set(3);						
					  P25QXX_SendCMD1(MCUUART.BufFromUART[4]);					
						bReadByte=0;
  					break;
					case 0x9F://QPI
						SPI_Process_Set(3);	
						P25QXX_RDID(&MCUUART.BufToUART[4]);
						bReadByte=3;	
						break;
					case 0x90://QPI
						SPI_Process_Set(3);							
						P25QXX_ReadID(&MCUUART.BufToUART[4]);
						bReadByte=2;
						break;
					case 0x92:
						SPI_Process_Set(3);						
						QSPI_Send_CMD(0x92,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_2_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_2_LINES);		
						QSPI_Receive(&MCUUART.BufToUART[4],2);
						bReadByte=2;
						break;
					case 0x94:					
						SPI_Process_Set(3);						
						QSPI_Send_CMD(0x92,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_4_LINES);		
						QSPI_Receive(&MCUUART.BufToUART[4],2);
						bReadByte=2;
						break;
					case 0xB9://QPI
						SPI_Process_Set(3);						
					  P25QXX_SendCMD1(MCUUART.BufFromUART[4]);	
						bReadByte=0;					
						break;					
					case 0xAB://QPI					
						SPI_Process_Set(3);						
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(0xab,0,6,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);	
						else QSPI_Send_CMD(0xab,0,24, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				
						QSPI_Receive(&MCUUART.BufToUART[4],1);
						bReadByte=1;
						break;
					case 0x77:					
						SPI_Process_Set(3);						
            QSPI_Send_CMD(MCUUART.BufFromUART[4],0,24, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);				
						QSPI_Transmit(&MCUUART.BufFromUART[5],1);
						bReadByte=0;
						break;						
					case 0x5A://QPI					
						SPI_Process_Set(3);					
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(0x5a,0,8,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_24_BITS,QSPI_DATA_4_LINES);	
						else QSPI_Send_CMD(0x5a,0,8, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE);				
						QSPI_Receive(&MCUUART.BufToUART[4],84);
						bReadByte=84;
						break;
					case 0xFF://QPI					
						SPI_Process_Set(3);					
            P25QXX_SendCMD1(MCUUART.BufFromUART[4]);								
						bReadByte=0;
						break;
					case 0x4B:	
						SPI_Process_Set(3);					
						QSPI_Send_CMD(0x4b,0,32, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE);			
						QSPI_Receive(&MCUUART.BufToUART[4],16);
						bReadByte=16;
						break;						
					//QPI特有
					case 0xC0:					break;//QPI
					case 0x0C:					break;//QPI	
          //TEST MODE					
					case 0xDA:  
					case 0xD4:
						SPI_Process_Set(1);					
						SPI_Pro_Inst_Parse();
						P25QXX_Enter_TestMode();
						P25QXX_TestMode_Write_All_chip(Data_Buffer_1,Data_Buffer_2);
						P25QXX_Exit_TestMode();
						bReadByte = 0;	
						break;	
					default:     //不支持的指令
						 Error_Code =ERROR_PARAMETERS;
						 break;
			}       		   
	    break;			
		case	SPI_Read_Verify:
			bReadByte = 0;
      switch(MCUUART.BufFromUART[4])
			{
					case 0x0b:  //QPI
					case 0x03:
					case 0x3b:						
					case 0xbb:									
					case 0x6b:						
					case 0xeb:  //QPI
					  SPI_Process_Set(2);
						SPI.ReadInst =	MCUUART.BufFromUART[4];							
					  SPI_Read_Inst_Parse();	
   					bReadByte = 0;					
						if(P25QXX_Read_Verify(datatemp,SPI.m_Read_Operate_Start_addr,SPI.m_Read_Operate_Length,CHECK_PATTERN) !=OK)
						{							
									MCUUART.BufToUART[4] = cVTINFO.ErrorAdd>>24;
									MCUUART.BufToUART[5] = cVTINFO.ErrorAdd>>16;
									MCUUART.BufToUART[6] = cVTINFO.ErrorAdd>>8;
									MCUUART.BufToUART[7] = cVTINFO.ErrorAdd;
									MCUUART.BufToUART[8] = cVTINFO.ErrorData;
									MCUUART.BufToUART[9] = cVTINFO.RightData;
									bReadByte = 6;							
						}
					break;	
				 default:
					break;						
			}       		   
	    break;	 			
		 default:
			break;	
	}

		SPI_Process_Set(3);	
	
		MCUUART.BufToUART[0] = MCUUART.BufFromUART[0];								
		MCUUART.BufToUART[1] = MCUUART.BufFromUART[1];			
		MCUUART.BufToUART[2] = Error_Code;									
		MCUUART.BufToUART[3] = bReadByte;		    					

		MCUUART.BufToUART[bReadByte + 4] = GetCheckSum(MCUUART.BufToUART,bReadByte +4 );		//check sum
		SendDatatoUART(MCUUART.BufToUART, bReadByte +4+1);
}
