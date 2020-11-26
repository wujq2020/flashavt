#include "P25QXX.h"
#include "qspi.h"
#include "delay.h"
#include "usart.h"  
#include "switch.h"
#include "led.h"
#include "24cxx.h"
#include "OLED_I2C.h"
#include "ad5761r.h"
#include<stdio.h>
#include<math.h>
#include "LTC2631.h"
#include "adc.h"
#include "power.h"
#include "ex_p25qxx.h"

//一个PAGE 256-BYTE
//16个PAGE为4K-BYTE，为1个Sector
//16个Sector为64K-BYTE，为1个Block
//P25Q256  （256 bit / 8 = 32 BYTE  ）
//例:容量为32M字节,共有512个Block,8192个Sector 


//通用参数
u16 P25QXX_TYPE;	    
u8  P25QXX_QPI_MODE=0;		  //QPI模式标志:0,SPI模式;1,QPI模式.
u32 FlashSize;              //容量 单位BYTE
u32 PageSize =256;
u8  Flag_4Byte_3Byte_Address =0;                           // 0:3-Byte Address    1:4-Byte Address
u32 QSPI_ADDRESS_XX_BITS     =QSPI_ADDRESS_24_BITS;        // 读写的ADDRESS长度 Q128及以下:XX=24    256及以上:XX=32
u8  CheckMode;

u16 ReadPageSize = READ_PAGE_SIZE;
u16 WritePageSize = WRITE_PAGE_SIZE;

////写擦指令参数
//u32 WrEr_INSTRUCTION_X_LINES =QSPI_INSTRUCTION_1_LINE;     //  QPI:X=4    Other:X=1
//u32 WrEr_ADDRESS_X_LINES     =QSPI_ADDRESS_1_LINE;         //  QPI:X=4    Other:X=1
//u32 WRITE_DATA_X_LINES       =QSPI_DATA_1_LINE;            //PP/QPI:X=1   DPP:X=2   QPP:X=23
//u32 P25X_Write_Inst          =P25X_PageProgram;

////读指令参数  //默认为0X3B读取
////u32	P25X_Read_Inst           =P25X_FastReadDual;	
//u32 Read_Dummy               =8;
//u32 Read_INSTRUCTION_X_LINES =QSPI_INSTRUCTION_1_LINE;	
//u32	Read_ADDRESS_X_LINES     =QSPI_ADDRESS_1_LINE;
//u32	Read_DATA_X_LINES        =QSPI_DATA_2_LINES;	

//型号信息
//u8 	CHIP_TYPE_BUF[32];
										
u8 P25QXX_QPI_MODE_CHK(void)       
{
	//检查当前在线SPI-flash是否处于QPI模式
	u8 ChkQpiFlag;
  ChkQpiFlag= 0x01 & P25QXX_QPI_MODE ;		
	return ChkQpiFlag;
}	
//u16 P25QXX_ReadID(void)
//{
//	u8 temp[2];
//	u16 deviceid;
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_ManufactDeviceID,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_4_LINES);//QPI,读id,地址为0,4线传输数据_24位地址_4线传输地址_4线传输指令,无空周期,2个字节数据
//	else 
//		QSPI_Send_CMD(P25X_ManufactDeviceID,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);			//SPI,读id,地址为0,单线传输数据_24位地址_单线传输地址_单线传输指令,无空周期,2个字节数据
//	QSPI_Receive(temp,2);
//	deviceid=(temp[0]<<8)|temp[1];
//	return deviceid;
//} 

u16 P25QXX_ReadID(u8* Uid)
{
//	u8 temp[2];
	u16 deviceid;
	if(P25QXX_QPI_MODE_CHK())
		QSPI_Send_CMD(P25X_ManufactDeviceID,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_4_LINES);//QPI,读id,地址为0,4线传输数据_24位地址_4线传输地址_4线传输指令,无空周期,2个字节数据
	else 
		QSPI_Send_CMD(P25X_ManufactDeviceID,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);			//SPI,读id,地址为0,单线传输数据_24位地址_单线传输地址_单线传输指令,无空周期,2个字节数据
	QSPI_Receive(Uid,2);
	deviceid=(Uid[0]<<8)|Uid[1];
	return deviceid;
} 

void P25QXX_RDID(u8* pUid)   
{  
	if(P25QXX_QPI_MODE_CHK())
		QSPI_Send_CMD(P25X_JedecDeviceID,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	else 
		QSPI_Send_CMD(P25X_JedecDeviceID,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
    QSPI_Receive(pUid,3);
}

//void P25QXX_Check_ID(void)           
//{
//	  //读验chipID  检查是否放错位置或放错型号。
//		delay_ms(5);
//		P25QXX_TYPE=P25QXX_ReadID();    	
//}
void SPI_Flash_Customs_Buffer_Init(u32 WriteStaAddr,u32 WriteEndAddr,u8* pBuffer)    
{
	u8 i=0;
	for(i=0;i<8;i++)
	{
		 memcpy(&Data_Buffer_Custom_8K[i*1024+WriteStaAddr],pBuffer,WriteEndAddr-WriteStaAddr+1);	
	}		
}

void SPI_Flash_Buffer_Init(u8 ckb,u8 ckbba)    
{ //初始化正反棋盘的Buffer   
	u8   PageNum;
	u16  j;
	switch((SPI.Type>>3)&0x0F)         //1M 2M 4M一页一换    8M 16M两页一换   32M 64M四页一换   128M （128*AA,128*55） 十六页一换
	{
	  case Q05:	                       	//一页一换
	  case Q10:		
	  case Q20:
	  case Q40:	
//===================40 S==================//			
	  if((SPI.Type&0x03) == 0x03)                 //40S   半页一换
		 {	
			for(PageNum=1;PageNum<=32;PageNum++)   //128个AA   128个55  一页一换 
				{
					switch(PageNum)
					{
						case 1:
						case 3:
						case 5:
						case 7:
						case 9:
						case 11:
						case 13:	
						case 15:	
	          case 17:
						case 19:
						case 21:
						case 23:
						case 25:
						case 27:
						case 29:	
						case 31:							
						 {		
							 for(j=(PageNum-1)*256;j<PageNum*256-128;j++)
								 {
										Data_Buffer_1[j] = ckb;
								 }
							 for(j=PageNum*256-128;j<PageNum*256;j++)
								 {
										Data_Buffer_1[j] = ckbba;
								 }	
						 }
						 break;
						case 2:
						case 4:
						case 6:
						case 8:
						case 10:
						case 12:
						case 14:	
						case 16:
						case 18:
						case 20:
						case 22:
						case 24:
						case 26:
						case 28:
						case 30:	
						case 32:
							{
								for(j=(PageNum-1)*256;j<PageNum*256-128;j++)
									{
										 Data_Buffer_1[j] = ckbba;
									}
								for(j=PageNum*256-128;j<PageNum*256;j++)
									{
										 Data_Buffer_1[j] = ckb;
									}							 				
							}
							break;
						 default:	
							break;	
					 }				
				}	
		 }
//===================40 H/L/U==================//		 
    else
		 {				
		for(PageNum=1;PageNum<=32;PageNum++)    
			{
	      switch(PageNum)
				{
					  case 1:
						case 3:
						case 5:
						case 7:
						case 9:
						case 11:
						case 13:	
						case 15:	
	          case 17:
						case 19:
						case 21:
						case 23:
						case 25:
						case 27:
						case 29:	
						case 31:							
					{		
						j= (PageNum-1)*256;
						while(j < PageNum * 256)
						{
							Data_Buffer_1[j]   = ckb;
							Data_Buffer_1[j+1] = ckb;
							Data_Buffer_1[j+2] = ckb;		
							Data_Buffer_1[j+3] = ckb;			
							Data_Buffer_1[j+4] = ckbba;
							Data_Buffer_1[j+5] = ckbba;		
							Data_Buffer_1[j+6] = ckbba;		
							Data_Buffer_1[j+7] = ckbba;	
							j = j + 8;         					
						}
					 }
					 break;
					  case 2:
						case 4:
						case 6:
						case 8:
						case 10:
						case 12:
						case 14:	
						case 16:
						case 18:
						case 20:
						case 22:
						case 24:
						case 26:
						case 28:
						case 30:	
						case 32:
					 {
						 j= (PageNum-1)*256;
						 while(j < PageNum * 256)
						 {				 
							 Data_Buffer_1[j]   = ckbba;
							 Data_Buffer_1[j+1] = ckbba;
							 Data_Buffer_1[j+2] = ckbba;		
							 Data_Buffer_1[j+3] = ckbba;			
							 Data_Buffer_1[j+4] = ckb;
							 Data_Buffer_1[j+5] = ckb;		
							 Data_Buffer_1[j+6] = ckb;		
							 Data_Buffer_1[j+7] = ckb;	
								j = j + 8; 
						 }					
						}
						break;
					 default:	
						break;	
				}				
			}
		}		
		  break;
	  case Q80:		            //两页一换
	  case Q16:
//===================16 S==================//	
		if((SPI.Type&0x03) == 0x03)
    {			
			for(PageNum=1;PageNum<=32;PageNum++)   //128个AA   128个55  四页一换 
				{
					switch(PageNum)
					{
						case 1:
						case 2:
						case 3:
						case 4:
						case 9:
						case 10:
						case 11:	
						case 12:	
	          case 17:
						case 18:
						case 19:
						case 20:
						case 25:
						case 26:
						case 27:	
						case 28:						
						 {		
							 for(j=(PageNum-1)*256;j<PageNum*256-128;j++)
								 {
										Data_Buffer_1[j] = ckb;
								 }
							 for(j=PageNum*256-128;j<PageNum*256;j++)
								 {
										Data_Buffer_1[j] = ckbba;
								 }	
						 }
						 break;
						case 5:
						case 6:
						case 7:
						case 8:						
						case 13:
						case 14:
						case 15:	
						case 16:							
						case 21:
						case 22:
						case 23:
						case 24:							
						case 29:
						case 30:
						case 31:	
						case 32:	
							{
								for(j=(PageNum-1)*256;j<PageNum*256-128;j++)
									{
										 Data_Buffer_1[j] = ckbba;
									}
								for(j=PageNum*256-128;j<PageNum*256;j++)
									{
										 Data_Buffer_1[j] = ckb;
									}							 				
							}
							break;
						 default:	
							break;	
					 }				
				}
     }			
//===================16 H/L/U==================//		
    else
    {				
		for(PageNum=1;PageNum<=32;PageNum++)    
			{
	      switch(PageNum)
				{
					case 1:
					case 2:
					case 5:
					case 6:
					case 9:
					case 10:
					case 13:	
					case 14:	
          case 17:
					case 18:
					case 21:
					case 22:
					case 25:
					case 26:
					case 29:	
					case 30:						
					{		
						j= (PageNum-1)*256;
						while(j < PageNum * 256)
						{
							Data_Buffer_1[j]   = ckb;
							Data_Buffer_1[j+1] = ckb;
							Data_Buffer_1[j+2] = ckb;		
							Data_Buffer_1[j+3] = ckb;			
							Data_Buffer_1[j+4] = ckbba;
							Data_Buffer_1[j+5] = ckbba;		
							Data_Buffer_1[j+6] = ckbba;		
							Data_Buffer_1[j+7] = ckbba;	
							j = j + 8;         					
						}
					 }
					 break;
					case 3:
					case 4:
					case 7:
					case 8:
					case 11:
					case 12:
					case 15:	
					case 16:
					case 19:
					case 20:
					case 23:
					case 24:
					case 27:
					case 28:
					case 31:	
					case 32:
					 {
						 j= (PageNum-1)*256;
						 while(j < PageNum * 256)
						 {				 
							 Data_Buffer_1[j]   = ckbba;
							 Data_Buffer_1[j+1] = ckbba;
							 Data_Buffer_1[j+2] = ckbba;		
							 Data_Buffer_1[j+3] = ckbba;			
							 Data_Buffer_1[j+4] = ckb;
							 Data_Buffer_1[j+5] = ckb;		
							 Data_Buffer_1[j+6] = ckb;		
							 Data_Buffer_1[j+7] = ckb;	
								j = j + 8; 
						 }					
						}
						break;
					 default:	
						break;	
				 }				
			}
		}		
		  break;
	  case Q32:		            //四页一换
	  case Q64:			
		for(PageNum=1;PageNum<=32;PageNum++)    
			{
	      switch(PageNum)
				{
					  case 1:
						case 2:
						case 3:
						case 4:
						case 9:
						case 10:
						case 11:	
						case 12:	
	          case 17:
						case 18:
						case 19:
						case 20:
						case 25:
						case 26:
						case 27:	
						case 28:							
					{		
						j= (PageNum-1)*256;
						while(j < PageNum * 256)
						{
							Data_Buffer_1[j]   = ckb;
							Data_Buffer_1[j+1] = ckb;
							Data_Buffer_1[j+2] = ckb;		
							Data_Buffer_1[j+3] = ckb;			
							Data_Buffer_1[j+4] = ckbba;
							Data_Buffer_1[j+5] = ckbba;		
							Data_Buffer_1[j+6] = ckbba;		
							Data_Buffer_1[j+7] = ckbba;	
							j = j + 8;         					
						}
					 }
					 break;
						case 5:
						case 6:
						case 7:
						case 8:						
						case 13:
						case 14:
						case 15:	
						case 16:
							
						case 21:
						case 22:
						case 23:
						case 24:							
						case 29:
						case 30:
						case 31:	
						case 32:	
					 {
						 j= (PageNum-1)*256;
						 while(j < PageNum * 256)
						 {				 
							 Data_Buffer_1[j]   = ckbba;
							 Data_Buffer_1[j+1] = ckbba;
							 Data_Buffer_1[j+2] = ckbba;		
							 Data_Buffer_1[j+3] = ckbba;			
							 Data_Buffer_1[j+4] = ckb;
							 Data_Buffer_1[j+5] = ckb;		
							 Data_Buffer_1[j+6] = ckb;		
							 Data_Buffer_1[j+7] = ckb;	
								j = j + 8; 
						 }					
						}
						break;
					 default:	
						break;	
				 }				
			}			
		  break;
	  case Q128:	            //（128*AA,128*55） 十六页一换
		case Q256:		
		for(PageNum=1;PageNum<=16;PageNum++)      
			{
			   for(j=(PageNum-1)*256;j<PageNum*256-128;j++)
				   {
					    Data_Buffer_1[j] = ckb;
					 }
         for(j=PageNum*256-128;j<PageNum*256;j++)
				   {
					    Data_Buffer_1[j] = ckbba;
					 }					 
			}

		for(PageNum=17;PageNum<=32;PageNum++)      
			{
			   for(j=(PageNum-1)*256;j<PageNum*256-128;j++)
				   {
					    Data_Buffer_1[j] = ckbba;
					 }
         for(j=PageNum*256-128;j<PageNum*256;j++)
				   {
					    Data_Buffer_1[j] = ckb;
					 }					 
			}		
 			
		  break;		
	  default:	
		  break;				
	}
	for(j=0;j<8192;j++)     //CKBba                     
	{
		 Data_Buffer_2[j]= ~Data_Buffer_1[j];
	} 	
}

void SPI_Flash_Init(void)
{
  switch((SPI.Type>>3)&0x0F)         //初始化芯片型号及寻址范围
	{
	  case Q05:	                        
       FlashSize = 1024*1024/8 /2;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;		                  //3Byte_Address mode
		   SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;
		  break;		
	  case Q10:	                        
       FlashSize = 1024*1024/8 *1;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;	                   //3Byte_Address mode
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;
	  case Q20:	
       FlashSize = 1024*1024/8 *2;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;		                  //3Byte_Address mode
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;
	  case Q40:	
       FlashSize = 1024*1024/8 *4;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;		                  //3Byte_Address mode	
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;
	  case Q80:	
       FlashSize = 1024*1024/8 *8;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;		                  //3Byte_Address mode
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;
	  case Q16:	
       FlashSize = 1024*1024/8 *16;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;		                  //3Byte_Address mode	
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;
	  case Q32:	
       FlashSize = 1024*1024/8 *32;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;		                  //3Byte_Address mode	
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;		
	  case Q64:	
       FlashSize = 1024*1024/8 *64;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;		                  //3Byte_Address mode		
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;
	  case Q128:	
       FlashSize = 1024*1024/8 *128;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_24_BITS;
       Flag_4Byte_3Byte_Address=0;		                  //3Byte_Address mode
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;
	  case Q256:	
       FlashSize = 1024*1024/8 *256;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_32_BITS;
       Flag_4Byte_3Byte_Address=1;		                  //4Byte_Address mode			  
		   P25QXX_4ByteAddr_Enable();                       //进入4-Byte地址模式，用以访问大于128M之后的存储空间 
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;		
		  break;		
	  case Q512:	
       FlashSize = 1024*1024/8 *512;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_32_BITS;
       Flag_4Byte_3Byte_Address=1;		                  //4Byte_Address mode		
       P25QXX_4ByteAddr_Enable();		                    //进入4-Byte地址模式
		   SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;
		  break;
	  case Q1G:	
       FlashSize = 1024*1024/8 *1024;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_32_BITS;
       Flag_4Byte_3Byte_Address=1;		                  //4Byte_Address mode	
       P25QXX_4ByteAddr_Enable();		                    //进入4-Byte地址模式
		   SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;
		  break;
	  case Q2G:	
	     FlashSize = 1024*1024/8 *2048;
       QSPI_ADDRESS_XX_BITS = QSPI_ADDRESS_32_BITS;
       Flag_4Byte_3Byte_Address=1;		                  //4Byte_Address mode	
       P25QXX_4ByteAddr_Enable();		                    //进入4-Byte地址模式
		   SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;
		  break;
	  default:
       SPI.Tpp=2;  
		   SPI.Tpe=10; 
		   SPI.Tse=10;
		   SPI.Tbe32k=10;  
		   SPI.Tbe64k=10; 
		   SPI.Tce=10;			
		  break;		
	}
	
	switch((SPI.Type>>3)&0x0F)       	//	TEST MODE
	{
		case Q05:	                       
		case Q10:		
		case Q20:
		case Q40:
		case Q80:		            
		case Q16:	
		case Q32:		            
		case Q64:			
		  	if((SPI.Type&0x03) == 0x03)                 //40S和16S  
		    {			
						SPI.CmdTestMode[0]=0x96;
						SPI.CmdTestMode[1]=0x69;
						SPI.CmdTestMode[2]=0xa8;
				}
				else
				{
						SPI.CmdTestMode[0]=0xa8;
						SPI.CmdTestMode[1]=0x8a;
						SPI.CmdTestMode[2]=0x65;				
				}
		  break;
	  case Q128:	           
		case Q256:	
	    SPI.CmdTestMode[0]=0x96;
			SPI.CmdTestMode[1]=0x69;
			SPI.CmdTestMode[2]=0xa8;							
			break;		
		default:
			SPI.CmdTestMode[0]=0xa8;
			SPI.CmdTestMode[1]=0x8a;
			SPI.CmdTestMode[2]=0x65;					
			break;			
	}
	
//  LOAD_Dis_chip_type();	
}
 
void P25QXX_Init(void)              //上电初始化(1次)	需读取配置参数后执行
{	
		QSPI_Init();					     //初始化MCU的QSPI接口 - flash size/ clock speed	
		SPI_Flash_Init();  
	  SPI_Flash_Buffer_Init(0xAA,0x55);   //初始化SPI-Flash的正反棋盘Buffer
}

void P25QXX_QE_Enable(void)     		//置1 S9:QE 位
{
	u8 QE_value=0;
	u8 SR_Buffer[2]={0x00,0x02};      //S7-S0 , S15-S8	
	QE_value=P25QXX_ReadSR(2);		 		//先读出状态寄存器2的原始值  35H	
	if(((SPI.Type>>3)&0x0F) < Q32)
	 {		
//			if(((SPI.Type&0x03) == 0x03) &&(((SPI.Type>>3)&0x0F) == Q40) )                 //40S
		 	 if((SPI.Type&0x03) == 0x03)                 //40S 16s
				{	
					if((QE_value&0x02)==0)			  		//若QE==0则 置1
					{		
						P25QXX_Write_Enable();	        //40S使用 31H + 02H(QE)        31H + 8bit     会影响小容量的CR寄存器预留位
						delay_ms(5);		
						P25QXX_Write_SR(2,&SR_Buffer[1]);//写状态寄存器2  
						delay_ms(5);						
					}		
				}			 
        else
        {	
					if((QE_value&0x02)==0)			  		//若QE==0则 置1
					{
						P25QXX_Write_Enable();          //8M等小容量 01H+00H+02H(QE)	     01H + 16bit
						delay_ms(5);		
						P25QXX_Write_SR(1,&SR_Buffer[0]);//写状态寄存器2 
						delay_ms(5);				
					}	
        }
   }
	else
	 {
			if((QE_value&0x02)==0)			  		//若QE==0则 置1
			{		
				P25QXX_Write_Enable();	        //32M等大容量 31H + 02H(QE)        31H + 8bit     会影响小容量的CR寄存器预留位
				delay_ms(5);		
				P25QXX_Write_SR(2,&SR_Buffer[1]);//写状态寄存器2  
				delay_ms(5);	
//        QE_value=P25QXX_ReadSR(2);				
			}			 
   }
}

void P25QXX_QE_Disable(void)     		//清0 S9:QE 位
{
	u8 QE_value=0;
	u8 SR_Buffer[2]={0x00,0x00};      //S7-S0 , S15-S8
	QE_value=P25QXX_ReadSR(2);		 		//先读出状态寄存器2的原始值  35H	
	if( ((SPI.Type>>3)&0x0F) < Q32 )
	 {		
			if(QE_value&0x02)              		//若QE==1则 清0
			{		
				P25QXX_Write_Enable();	     		//8M等小容量 01H+00H+00H(QE)	     01H + 16bit
				delay_ms(5);		
				P25QXX_Write_SR(1,&SR_Buffer[0]);//写状态寄存器2  	
				delay_ms(5);
			}	
	 }
	else
	 {
			if(QE_value&0x02)              		//若QE==1则 清0
			{	
				P25QXX_Write_Enable();	     		//32M等大容量 31H + 00H(QE)        31H + 8bit       会影响小容量的CR寄存器预留位
				delay_ms(5);		
				P25QXX_Write_SR(2,&SR_Buffer[1]);//写状态寄存器2  31H + 00H(32M)   
				delay_ms(5);		
			}	
   }
}

//P25QXX进入QSPI模式 
void P25QXX_Qspi_Enable(void)
{
  P25QXX_QE_Enable();	   // QE = 1
  delay_ms(5);		
	QSPI_Send_CMD(P25X_EnterQPIMode,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//写command指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据

	P25QXX_QPI_MODE |= 0x01;
  delay_ms(5);	
}

//P25QXX退出QSPI模式 
void P25QXX_Qspi_Disable(void)
{ 
	QSPI_Send_CMD(P25X_ExitQPIMode,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//写command指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	P25QXX_QPI_MODE &= ~(0x01);	
	delay_ms(5);	
}
u8 P25QXX_ReadSR(u8 regno)   
{  
	u8 byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=P25X_ReadStatusReg1;    //读状态寄存器1指令 05H
            break;
        case 2:
            command=P25X_ReadStatusReg2;    //读状态寄存器2指令 35H
            break;
        case 3:
            command=P25X_ReadStatusReg3;    //读状态寄存器3指令 15H
            break;
        default:
            command=P25X_ReadStatusReg1;    
            break;
    }   
	  if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);	//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
	  else QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
	  QSPI_Receive(&byte,1);		
	  return byte;   
}  

//写P25QXX状态寄存器
void P25QXX_Write_SR(u8 regno,u8 *pSRbuffer)   
{   
    u8 command=0;
    switch(regno)
    {
        case 1:
            command=P25X_WriteStatusReg1;    //写状态寄存器1指令
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);	//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
						else QSPI_Send_CMD(command,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
						QSPI_Transmit(pSRbuffer,2);		
            break;
        case 2:
            command=P25X_WriteStatusReg2;    //写状态寄存器2指令
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);	//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
						else QSPI_Send_CMD(command,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
						QSPI_Transmit(pSRbuffer,1);					
            break;
        case 3:
            command=P25X_WriteStatusReg3;    //写状态寄存器3指令(8M  32M的指令是不一样的 实际使用时要修改配置)  
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);	//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
						else QSPI_Send_CMD(command,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
						QSPI_Transmit(pSRbuffer,1);					
            break;
        default:
            command=P25X_WriteStatusReg1;   
						if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);	//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
						else QSPI_Send_CMD(command,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
						QSPI_Transmit(pSRbuffer,2);					
            break;
    }   
         	      
}  

void P25QXX_Wait_Busy(u32 Inst)   
{ 
	if(!SPI.Polling_flag)	//Polling 	
	{	
	   while((P25QXX_ReadSR(1)&0x01)==0x01);
	}
  else
	{
	  switch(Inst)
		{
			case P25X_PageProgram:
			case P25X_DPageProgram:
			case P25X_QPageProgram:
				delay_ms(SPI.Tpp);
			  break;
		  case P25X_PageErase:
				delay_ms(SPI.Tpe);
			  break;
		  case P25X_SectorErase:
				delay_ms(SPI.Tse);
			  break;						
		  case P25X_BlockErase32:
				delay_ms(SPI.Tbe32k);
			  break;				
		  case P25X_BlockErase64:
				delay_ms(SPI.Tbe64k);
			  break;			
		  case P25X_ChipErase_C7h:
			case P25X_ChipErase_60h:
				delay_ms(SPI.Tce);
			  break;
      case P25X_TestMode_Odd_Even_Page_Chip_Program:
			case P25X_TestMode_Odd_Even_Page_Plane_Program:
				delay_ms(SPI.Tpp);
			  break;			
      default:
				delay_ms(10);
			  break;
		}
	}		
} 
//P25QXX写使能	
//将S1寄存器的WEL置位   
void P25QXX_Write_Enable(void)   
{
	if(P25QXX_QPI_MODE_CHK())
		QSPI_Send_CMD(P25X_WriteEnable,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);	//QPI,写使能指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	else 
		QSPI_Send_CMD(P25X_WriteEnable,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写使能指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
}
//P25QXX写禁止	
//将WEL清零  
//void P25QXX_Write_Disable(void)   
//{  
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_WriteDisable,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
//	else 
//		QSPI_Send_CMD(P25X_WriteDisable,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
//} 
//void P25QXX_Volatile_SR_Write_Enable(void)   
//{  
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_VWREN,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
//	else 
//		QSPI_Send_CMD(P25X_VWREN,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
//}

//void P25QXX_Global_Block_Lock(void)   
//{  
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_GBLK,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
//	else 
//		QSPI_Send_CMD(P25X_GBLK,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
//}
//void P25QXX_Global_Block_Unlock(void)   
//{  
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_GBULK,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
//	else 
//		QSPI_Send_CMD(P25X_GBULK,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
//}

//void P25QXX_Reset_Enable(void)   
//{  
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_ResetEnable,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
//	else 
//		QSPI_Send_CMD(P25X_ResetEnable,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
//}

//void P25QXX_Reset(void)   
//{  
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_Reset,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
//	else 
//		QSPI_Send_CMD(P25X_Reset,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
//}
void P25QXX_Deep_PowerDown(void)   
{  
	if(P25QXX_QPI_MODE_CHK())
		QSPI_Send_CMD(P25X_PowerDown,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	else 
		QSPI_Send_CMD(P25X_PowerDown,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
}
//void P25QXX_Suspend(void)   
//{  
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_PES_75h,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
//	else 
//		QSPI_Send_CMD(P25X_PES_75h,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
//}
//void P25QXX_Resume(void)   
//{  
//	if(P25QXX_QPI_MODE_CHK())
//		QSPI_Send_CMD(P25X_PER_7Ah,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
//	else 
//		QSPI_Send_CMD(P25X_PER_7Ah,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
//}

//P25QXX_SendCMD1 命令支持SPI和QPI（无地址，无dummy，无数据）
void P25QXX_SendCMD1(u32 cmd)   
{  
	if(P25QXX_QPI_MODE_CHK())
		QSPI_Send_CMD(cmd,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);
	else 
		QSPI_Send_CMD(cmd,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				
}


void P25QXX_Erase_Page_All(u32 Dst_Addr) 
{
	Dst_Addr*=PageSize;
	P25QXX_Write_Enable();                  
  QSPI_Send_CMD(P25X_PageErase,Dst_Addr,0,SPI.WrEr_INSTRUCTION_X_LINES,SPI.WrEr_ADDRESS_X_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_NONE); 
	P25QXX_Wait_Busy(P25X_PageErase);   				    
}

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms
void P25QXX_Erase_Sector_All(u32 Dst_Addr)   
{  
 	Dst_Addr*=4096;
  P25QXX_Write_Enable();                  //SET WEL 	
  QSPI_Send_CMD(P25X_SectorErase,Dst_Addr,0,SPI.WrEr_INSTRUCTION_X_LINES,SPI.WrEr_ADDRESS_X_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_NONE);
	P25QXX_Wait_Busy(P25X_SectorErase);   				          //等待擦除完成  
}

//擦除64K Block
void P25QXX_Erase_Block_All(u32 Dst_Addr)
{
  Dst_Addr*=65536;
  P25QXX_Write_Enable();                  //SET WEL 	 
 	QSPI_Send_CMD(P25X_BlockErase64,Dst_Addr,0,SPI.WrEr_INSTRUCTION_X_LINES,SPI.WrEr_ADDRESS_X_LINES,QSPI_ADDRESS_XX_BITS,QSPI_DATA_NONE); 	
	P25QXX_Wait_Busy(P25X_BlockErase64);   				    //等待擦除完成
}

//擦除Chip 
//等待时间长
void P25QXX_Erase_Chip_All(u8 Inst)
{ 
  P25QXX_Write_Enable();              //SET WEL 	 	
	QSPI_Send_CMD(Inst,0,0,SPI.WrEr_INSTRUCTION_X_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE); 
	P25QXX_Wait_Busy(Inst);   				    //等待擦除完成
}
  
void P25QXX_Erase(u32 WriteAddr,u32 NumByteToWrite)   //ERASE需要地址的初始化，不放到开始阶段配置参数。
{
	u32 secpos_str,secpos_end;
	u32 pagepos_str,pagepos_end;
	u32 blcpos_str,blcpos_end;  
 	u16 i;  
	switch(SPI.EraseInst)
	 {
		case P25X_PageErase:
	     pagepos_str = WriteAddr/PageSize;
	     pagepos_end =(WriteAddr+NumByteToWrite-1)/PageSize;				
			 for(i=pagepos_str;i<=pagepos_end;i++)
		    {
			    P25QXX_Erase_Page_All(i);     //Page擦除
		    }			
			 break;
		case P25X_SectorErase:
 	     secpos_str  = WriteAddr/4096; //扇区起始地址
       secpos_end  =(WriteAddr+NumByteToWrite-1)/4096; //扇区结束始地址		   		
			 for(i=secpos_str;i<=secpos_end;i++)
		    {
			    P25QXX_Erase_Sector_All(i);   //Sector擦除
		    }
			 break;
		case P25X_BlockErase64:
	     blcpos_str  = WriteAddr/65536;
	     blcpos_end  =(WriteAddr+NumByteToWrite-1)/65536;			
			 for(i=blcpos_str;i<=blcpos_end;i++)
		    {
			    P25QXX_Erase_Block_All(i);    //Block擦除
		    }			
			 break;
		case P25X_ChipErase_C7h:
			 P25QXX_Erase_Chip_All(P25X_ChipErase_C7h);     //Chip擦除
			 break;
		case P25X_ChipErase_60h:
			 P25QXX_Erase_Chip_All(P25X_ChipErase_60h);     //Chip擦除
			 break;	
		default:			
			 break;			
	 }	
}

//SPI在一页(0~65535)内写入少于PageSize个字节的数据
//在指定地址开始写入最大PageSize字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)
//NumByteToWrite:要写入的字节数(最大PageSize),该数不应该超过该页的剩余字节数!
void P25QXX_Write_Page_All(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	P25QXX_Write_Enable();					//写使能
  QSPI_Send_CMD(SPI.ProgramInst,WriteAddr,0,SPI.WrEr_INSTRUCTION_X_LINES,SPI.WrEr_ADDRESS_X_LINES,QSPI_ADDRESS_XX_BITS,SPI.WRITE_DATA_X_LINES);	//PP,DPP,QPP,QPI
	QSPI_Transmit(pBuffer,NumByteToWrite);	 
	P25QXX_Wait_Busy(SPI.ProgramInst);					    //等待写入结束
} 
//第一次写入整page剩余的字节，之后每次写WritePageSize个字节（WritePageSize必须被256整除）
//void P25QXX_Write(u8* pBuffer_a,u8* pBuffer_ba,u32 WriteAddr,u32 NumByteToWrite)  
void P25QXX_Write(u32 WriteAddr,u32 NumByteToWrite) 
{ 
	u16 pageremain;	   
	u8 * pBuffer_str_value; 	
	u8 * pBuffer;	
	
  switch(SPI.m_Pattern_Select)
	{
		case PATTERN_CKB:
			pBuffer = Data_Buffer_1;
		  pBuffer_str_value = pBuffer;
	    pBuffer=pBuffer_str_value + WriteAddr%8192;
			break;
		case PATTERN_RANDOM:
			pBuffer = Data_Buffer_Random_8K;
		  pBuffer_str_value = pBuffer;
	    pBuffer=pBuffer_str_value + WriteAddr%8192;
			break;
		case PATTERN_CUSTOM:
			pBuffer = Data_Buffer_Custom_8K;
		  pBuffer_str_value = pBuffer;
	    pBuffer=pBuffer_str_value + WriteAddr%8192;
			break;
		case PATTERN_FLASH:
			pBuffer = Data_Buffer_Flash_8K;
			pBuffer_str_value = pBuffer;
	    pBuffer=pBuffer_str_value;
			break;
		default:
			pBuffer = Data_Buffer_1;
			pBuffer_str_value = pBuffer;
	    pBuffer=pBuffer_str_value + WriteAddr%8192;
		  break;
	}
	
	if(WriteAddr%PageSize ==0)               //写地址在页的起始地址
	{
		pageremain=WritePageSize;
	}		
	else
	{
		pageremain=PageSize-WriteAddr%PageSize;//第一次把整夜剩余的自己写进去
	}
 
	if(NumByteToWrite<=pageremain)           //不大于PageSize(default:256)个字节
		pageremain=NumByteToWrite;      
	while(1)
	{	 
//    Uart_Check();	
    if(SPI.m_Pattern_Select ==PATTERN_FLASH)
		{
			  W25QXX_Read(pBuffer,Flash_Star_Addr+WriteAddr,pageremain);   //外挂Flash读取	
		}			
		P25QXX_Write_Page_All(pBuffer,WriteAddr,pageremain);            		
		if(NumByteToWrite==pageremain)break;   //写入结束
	 	else                   
		{		
		  WriteAddr+=pageremain;	             //跳起始地址
			NumByteToWrite-=pageremain;			     //减去已经写入了的字节数				

			switch(SPI.m_Pattern_Select)
			{
				case PATTERN_CKB:
				case PATTERN_RANDOM:
				case PATTERN_CUSTOM:
					pBuffer=pBuffer_str_value + WriteAddr%8192;
					break;
				case PATTERN_FLASH:
					pBuffer=pBuffer_str_value;
					break;
				default:
					pBuffer=pBuffer_str_value + WriteAddr%8192;
					break;
			}			
			if(NumByteToWrite>WritePageSize)
				pageremain=WritePageSize;          //一次可以写入PageSize(default:256)个字节
			else 
				pageremain=NumByteToWrite; 	       //不够256个字节了
		}
	} 
}
void P25QXX_TestMode_Write_All_chip(u8* pVerfBuf_a,u8* pVerfBuf_b)     
{
	u8 i;
	u32 ADDR1=0x000000;
		
	switch((SPI.Type>>3)&0x0F)          //1M 2M 4M一页一换    8M 16M两页一换   32M 64M四页一换   128M （128*AA,128*55） 十六页一换
	{
	  case Q05:	                       	//一页一换
	  case Q10:		
	  case Q20:
	  case Q40:			
					QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Chip_Program,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 	  //偶 even
					QSPI_Transmit(pVerfBuf_a,256);
					P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Chip_Program);

					QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Chip_Program,0X100,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);	//奇 odd
					QSPI_Transmit(pVerfBuf_b,256);
	        P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Chip_Program);
			break;
	  case Q80:		            //两页一换 
	  case Q16:					
		 	if((SPI.Type&0x03) == 0x03)                 //16S 
		    {			
					QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Chip_Program,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 	  
					QSPI_Transmit(pVerfBuf_a,256);
					P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Chip_Program);

					QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Chip_Program,0X400,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);	
					QSPI_Transmit(pVerfBuf_b,256);
					P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Chip_Program);
				}
				else
				{
					QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Chip_Program,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 	  
					QSPI_Transmit(pVerfBuf_a,256);
					P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Chip_Program);

					QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Chip_Program,0X100,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);	
					QSPI_Transmit(pVerfBuf_a,256);
					P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Chip_Program);

					QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Chip_Program,0X200,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 	  
					QSPI_Transmit(pVerfBuf_b,256);
					P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Chip_Program);

					QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Chip_Program,0X300,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE);
					QSPI_Transmit(pVerfBuf_b,256);
					P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Chip_Program);
				}					      	
		  break;
	  case Q32:		            //四页一换
	  case Q64:			
	
					for ( i = 0; i < 16; i++)
					{
							if ((i < 4) || ((i > 7) && (i < 12)))
							{
									if (i > 4)
									{
											ADDR1 = 0x001000 + ((i % 8)) * 256;
									}
									QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Plane_Program,ADDR1,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 	  //偶 even
									QSPI_Transmit(pVerfBuf_a,256);
					        P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Plane_Program);
									
									if (i < 4)
									{ ADDR1 = ADDR1 + 256; }
							}
							else
							{
									if (i > 7)
									{
											ADDR1 = 0x001000 + ((i % 8)) * 256;
									}
									QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Plane_Program,ADDR1,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 	  //偶 even
									QSPI_Transmit(pVerfBuf_b,256);
					        P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Plane_Program);

									if (i < 8)
									{
											ADDR1 = ADDR1 + 256;
									}
							}
					}	
		  break;
	  case Q128:	            //（128*AA,128*55） 十六页一换
		case Q256:				
					for (i = 0; i < 64; i++)
					{
							if (i > 31)
							{
									ADDR1 = (0X10000 + (i % 32) * 256);

							}
							if ((i < 16) || ((i < 48) && (i > 31)))
							{
									QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Plane_Program,ADDR1,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 	  //偶 even
									QSPI_Transmit(pVerfBuf_a,256);
									P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Plane_Program);
							}
							else
							{
									QSPI_Send_CMD(P25X_TestMode_Odd_Even_Page_Plane_Program,ADDR1,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 	  //偶 even
									QSPI_Transmit(pVerfBuf_b,256);
									P25QXX_Wait_Busy(P25X_TestMode_Odd_Even_Page_Plane_Program);
							}
							if (i < 32)
							{
									ADDR1 = ADDR1 + 256;
							}	
					}

				break;		
			default:	
				break;					
		 }
}

//读取SPI FLASH  只读不验
//在指定地址开始读取指定长度的数据
//pBuffer: 数据存储区
//pVerfBuf:没有用到
//ReadAddr:开始读取的地址(最大32bit)
//NumByteToRead:要读取的字节数(最大32bit)

void P25QXX_Read_No_Verify(u8* pBuffer,u32 ReadAddr,u32 NumByteToRead)   
{ 
//	 u16 i;	
	u32 pageremain;
	u8 * pBuffer_str_value; 
		
	pBuffer_str_value = pBuffer;	

  if(ReadAddr%PageSize ==0)
	{
		pageremain=ReadPageSize;
	}		
	else
	{
		pageremain=PageSize-ReadAddr%PageSize;
	}
			
	if(NumByteToRead<=pageremain)
		pageremain=NumByteToRead;  //不大于ReadPageSize(default:256)个字节
  memset(datatemp, 0, sizeof(datatemp));	
	
	while(1)
	{	

    QSPI_Send_CMD_alternateByte(SPI.ReadInst,ReadAddr,0,SPI.Read_ALTERNATE_X_LINES,SPI.Read_Dummy,SPI.Read_INSTRUCTION_X_LINES,SPI.Read_ADDRESS_X_LINES,QSPI_ADDRESS_XX_BITS,SPI.Read_DATA_X_LINES);		
//                QSPI_Send_CMD(SPI.ReadInst,ReadAddr,SPI.Read_Dummy,SPI.Read_INSTRUCTION_X_LINES,SPI.Read_ADDRESS_X_LINES,QSPI_ADDRESS_XX_BITS,SPI.Read_DATA_X_LINES);
	  QSPI_Receive(pBuffer,pageremain);           				  //读出所有数据 				
		if(NumByteToRead==pageremain)                        //判断是否读完
		{	  		  	
			 break;//读出结束
		}
	 	else               
		{			 					
			ReadAddr += pageremain;	       //跳起始地址
			pBuffer  += pageremain;	
			if(pBuffer - pBuffer_str_value >=8192)  //最大读出8k
			{
			   pBuffer = pBuffer_str_value;   //还原buffer
			}				
  //	  pagenum++;                     //页计数++
			NumByteToRead-=pageremain;		 //减去已经读出了的字节数
			if(NumByteToRead>ReadPageSize)pageremain=ReadPageSize;   //一次读出ReadPageSize(default:256)个字节
			else pageremain=NumByteToRead; //不够256个字节了
		}
	} 
} 

//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//pVerfBuf:校验数据
//ReadAddr:开始读取的地址(最大32bit)
//NumByteToRead:要读取的字节数(最大32bit)
//unsigned char P25QXX_Read_Verify(u8* pBuffer,u8* pVerfBuf_a,u8* pVerfBuf_ba,u32 ReadAddr,u32 NumByteToRead,u8  Check_Mode)   
unsigned char P25QXX_Read_Verify(u8* pBuffer,u32 ReadAddr,u32 NumByteToRead,u8  Check_Mode) 
{
	u32 pageremain;
  u16 i;	
	u8 * pBuffer_str_value; 
	u8 * pVerfBuf;
	
//	pVerfBuf = 	pVerfBuf_a;	
	pBuffer_str_value = pBuffer;	
//	pageremain=ReadPageSize-ReadAddr%ReadPageSize;          //单页剩余的字节数	
	if(ReadAddr%PageSize ==0)
	{
		pageremain=ReadPageSize;
	}		
	else
	{
		pageremain=PageSize-ReadAddr%PageSize;
	}
  pBuffer+=ReadAddr%ReadPageSize;	                        //页偏移量
	if(NumByteToRead<=pageremain)
		pageremain=NumByteToRead;                             //不大于ReadPageSize(default:256)个字节
  memset(datatemp, 0, sizeof(datatemp));
	
	if(SPI.m_Pattern_Select ==PATTERN_FLASH)
	{
			if(W25QXX_ReadID()!=W25Q128)         
			{
				return 0x01;//外挂Flash读取ID	不正确，直接返回错误
			}
	}	
		
	switch(SPI.m_Pattern_Select)
	{
		case PATTERN_CKB:
			pVerfBuf = Data_Buffer_1;		
			break;
		case PATTERN_RANDOM:
			pVerfBuf = Data_Buffer_Random_8K;	 
			break;
		case PATTERN_CUSTOM:
			pVerfBuf = Data_Buffer_Custom_8K;
			break;
		case PATTERN_FLASH:
			pVerfBuf = Data_Buffer_Flash_8K;
			break;
		default:
			pVerfBuf = Data_Buffer_1;
		  break;
	}

	while(1)
	{	
		if(SPI.m_Pattern_Select ==PATTERN_FLASH)
		{
		    W25QXX_Read(Data_Buffer_Flash_8K,Flash_Star_Addr+ReadAddr,pageremain);   //外挂Flash读取	
		}			
    QSPI_Send_CMD_alternateByte(SPI.ReadInst,ReadAddr,0,SPI.Read_ALTERNATE_X_LINES,SPI.Read_Dummy,SPI.Read_INSTRUCTION_X_LINES,SPI.Read_ADDRESS_X_LINES,QSPI_ADDRESS_XX_BITS,SPI.Read_DATA_X_LINES);		
//			QSPI_Send_CMD(SPI.ReadInst,ReadAddr,SPI.Read_Dummy,SPI.Read_INSTRUCTION_X_LINES,SPI.Read_ADDRESS_X_LINES,QSPI_ADDRESS_XX_BITS,SPI.Read_DATA_X_LINES);			
    QSPI_Receive(pBuffer,pageremain);           				  //读出所有数据 
    //=====================================数据校验=====================================//	
		switch(Check_Mode)                           
		{
			case CHECK_PATTERN:    //正反棋盘				
					 for(i=0;i<pageremain;i++)                                        
				   {
						 if(SPI.m_Pattern_Select ==PATTERN_FLASH)
						{   
	              if(pBuffer[i] != pVerfBuf[i]) 							
								{		
										cVTINFO.ErrorAdd=ReadAddr+i;
										cVTINFO.ErrorData=pBuffer[i];
										cVTINFO.RightData=pVerfBuf[i];
										return 0x01;	
								}						
						}	
            else
						{	
//               if(pBuffer[i] != pVerfBuf[ReadAddr%8192+i])   						
//								{											
//										cVTINFO.ErrorAdd=ReadAddr+i; //出错 !!
//										cVTINFO.ErrorData=pBuffer[i];
//										cVTINFO.RightData=pVerfBuf[ReadAddr%8192+i];
//										return 0x01;	
//								}							
                if((ReadAddr%8192+i) < 8192)  //支持任意长度读取
								{
     								if(pBuffer[i] != pVerfBuf[ReadAddr%8192+i])   						
										{											
												cVTINFO.ErrorAdd=ReadAddr+i; //出错 !!
												cVTINFO.ErrorData=pBuffer[i];
												cVTINFO.RightData=pVerfBuf[ReadAddr%8192+i];
												return 0x01;	
										}					
								}	
                else//超出8k区间
								{									
//										if(pBuffer[i] != pVerfBuf[(8192-ReadAddr)%8192+i]) 
                    if(pBuffer[i] != pVerfBuf[(ReadAddr%8192+i)%8192])  									
										{											
												cVTINFO.ErrorAdd=ReadAddr+i; //出错 !!
												cVTINFO.ErrorData=pBuffer[i];
												cVTINFO.RightData=pVerfBuf[(8192-ReadAddr)%8192+i];
												return 0x01;	
										}
							  }
						}
					 }											
				break;
			case CHECK_FF:		    break;				 				
			case CHECK_00:		    break;				 				
			case CHECK_CUSTOMER:	break;								 							
			case READ_ONLY:				break;		
			default:						  break;
				
		}				
		
		if(NumByteToRead==pageremain)    //判断是否读完
		{	  		  	
			 break;//读出结束
		}
	 	else //NumByteToRead>pageremain                    
		{			 					
			ReadAddr += pageremain;	       //跳起始地址
			pBuffer = pBuffer_str_value;   //还原buffer	(和可靠性不一样)		

			NumByteToRead-=pageremain;		 //减去已经读出了的字节数
			if(NumByteToRead>ReadPageSize)pageremain=ReadPageSize;   //一次读出ReadPageSize(default:256)个字节
			else pageremain=NumByteToRead; //不够256个字节了
		}
	}
  return 0x00;	
}	


void P25QXX_4ByteAddr_Enable(void)      //使能4字节地址模式 
{ 
 if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(P25X_Enable4ByteAddr,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);	
 else QSPI_Send_CMD(P25X_Enable4ByteAddr,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);
 delay_ms(5);
}	

void P25QXX_4ByteAddr_Disable(void)     //禁止4字节地址模式 
{
 if(P25QXX_QPI_MODE_CHK())QSPI_Send_CMD(P25X_Exit4ByteAddr,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);	
 else QSPI_Send_CMD(P25X_Exit4ByteAddr,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);	
 delay_ms(5);
}	

void P25QXX_Enter_TestMode(void)
{		
	//ENTER TEST MODE
	QSPI_Send_CMD(SPI.CmdTestMode[0],0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
	QSPI_Send_CMD(SPI.CmdTestMode[1],0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);						
	QSPI_Send_CMD(SPI.CmdTestMode[2],0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);	
	delay_ms(10);		

	Set_WP(INPUT);           //原WP脚浮空输入
	CS_ON_WP;                //原CS脚接到原WP的脚位上
	delay_ms(100);	
}

void P25QXX_Exit_TestMode(void)
{
	u8 CmdRstBuffer[2]={0x66,0x99};
	//EXIT TEST MODE
	QSPI_Send_CMD(CmdRstBuffer[0],0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
	QSPI_Send_CMD(CmdRstBuffer[1],0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);						
  delay_ms(1);	
	
	CS_OFF_WP;
	Set_WP(OUTPUT); 
}

void P25QXX_Set_FTMOD(u8 m_ftmod)    
{
	u32 CP1_Address,CP2_Address;
	
	switch((SPI.Type>>3)&0x0F)          
	{
		case Q05:	                       	
		case Q10:		
		case Q20:
		case Q40:
		case Q80:	
	 	case Q32:
		case Q128:
		case Q256:			
				CP1_Address=0x001003;
				QSPI_Send_CMD(P25X_TestMode_Set_Reg_Data,CP1_Address,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE);	
				QSPI_Transmit(&m_ftmod,1);
				delay_ms(10);
		   break;
		case Q16:				
	      CP1_Address=0x001003;
		    CP2_Address=0x101003;
				QSPI_Send_CMD(P25X_TestMode_Set_Reg_Data,CP1_Address,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE);	
				QSPI_Transmit(&m_ftmod,1);
				delay_ms(10);
		    QSPI_Send_CMD(P25X_TestMode_Set_Reg_Data,CP2_Address,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE);	
				QSPI_Transmit(&m_ftmod,1);
				delay_ms(10);
		   break;	
		case Q64: 			
	      CP1_Address=0x001003;
		    CP2_Address=0x401003;
				QSPI_Send_CMD(P25X_TestMode_Set_Reg_Data,CP1_Address,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE);	
				QSPI_Transmit(&m_ftmod,1);
				delay_ms(10);
		    QSPI_Send_CMD(P25X_TestMode_Set_Reg_Data,CP2_Address,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE);	
				QSPI_Transmit(&m_ftmod,1);
				delay_ms(10);
		   break;		
			default:	
			break;			
	}
}

void cVTINFO_init(void)    
{
	cVTINFO.VP_Start_Value        = 0;    	
	cVTINFO.VP_Stop_Value         = 1000;
	cVTINFO.VP_Rough_Step_Value   = 100;
	cVTINFO.VP_Fine_Step_Value    = -20;
	cVTINFO.VP_Era_Pro_flg        = 0x00;//不擦除
	
	cVTINFO.VE_Start_Value        = 0;  
	cVTINFO.VE_Stop_Value         = -1000;
	cVTINFO.VE_Rough_Step_Value   = -100;
	cVTINFO.VE_Fine_Step_Value    = 20;
	cVTINFO.VE_Era_Pro_flg        = 0x00;//不擦除
	
	cVTINFO.VT_otp_Flag           = 0x00;  //VT 在normal区
}

u8 P25QXX_VT_Verify(u8 test_type)    
{
  u8 Rough_Adj_Fail_Flag=0x00;
	u8 Fine_Adj_Ok_Flag=0x01;
	u32 Star_add=0;
	u32 Length=FlashSize;
	signed int Start_Value=0;
	signed int Rough_Step_Value=0;
	signed int Fine_Step_Value=0;
	signed int Stop_Value=0;
	
	Star_add = SPI.m_Read_Operate_Start_addr;
	Length = SPI.m_Read_Operate_Length;
	
	if(test_type == FTMODE_VTE)
	{
			Start_Value = cVTINFO.VE_Start_Value;
			Rough_Step_Value = cVTINFO.VE_Rough_Step_Value;
			Fine_Step_Value = cVTINFO.VE_Fine_Step_Value;
			Stop_Value = cVTINFO.VE_Stop_Value;
	}
	else
	{
			Start_Value = cVTINFO.VP_Start_Value;
			Rough_Step_Value = cVTINFO.VP_Rough_Step_Value;
			Fine_Step_Value = cVTINFO.VP_Fine_Step_Value;
			Stop_Value = cVTINFO.VP_Stop_Value;
	}
  
	cVTINFO.Value = Start_Value;		
	while(!Rough_Adj_Fail_Flag)//粗调
	{	
			AD5761R_SetVoltage_DIRECT(cVTINFO.Value);
			delay_ms(50);
		
//			Rough_Adj_Fail_Flag = P25QXX_Read_Verify(datatemp,Data_Buffer_1,Data_Buffer_2,Star_add,Length,CHECK_PATTERN);
      Rough_Adj_Fail_Flag = P25QXX_Read_Verify(datatemp,Star_add,Length,CHECK_PATTERN);		
			if(Rough_Adj_Fail_Flag)  break;

			if(test_type == FTMODE_VTE)
			{
				 if(cVTINFO.Value<= Stop_Value)
					 return 0x01;		//vte粗调到达最大量程						 
			}
			else
			{
				 if(cVTINFO.Value>= Stop_Value)
					 return 0x01;	  //vtp粗调到达最大量程						 
			}
			cVTINFO.Value += Rough_Step_Value;
	}
		
	while(Fine_Adj_Ok_Flag)//细调
	{
			if(test_type == FTMODE_VTE)
			{
				 if(cVTINFO.Value>= Start_Value)
					 return 0x01;		//vte细调到达最小量程				
			}
			else
			{
				 if(cVTINFO.Value<= Start_Value)
					 return 0x01;		//vtp细调到达最小量程	
			}
			cVTINFO.Value+= Fine_Step_Value;		
			
			AD5761R_SetVoltage_DIRECT(cVTINFO.Value);
			delay_ms(50);
			Fine_Adj_Ok_Flag = P25QXX_Read_Verify(datatemp,Star_add,Length,CHECK_PATTERN);
//			Fine_Adj_Ok_Flag = P25QXX_Read_Verify(datatemp,Data_Buffer_1,Data_Buffer_2,Star_add,Length,CHECK_PATTERN);
	}
	return 0x00;
}

u8 P25QXX_VT_Point_Verify(u8 tMode,signed int Point_Value)    
{
  u8 Fail_Flag=0x01;
	u32 Star_add=0;
	u32 Length=FlashSize;
	
	Star_add = SPI.m_Read_Operate_Start_addr;
	Length = SPI.m_Read_Operate_Length;
	
	if(tMode == FTMODE_VTE)
	{

	}
	else
	{

	} 
		AD5761R_SetVoltage_DIRECT(Point_Value);
		delay_ms(50);
	
//		Fail_Flag = P25QXX_Read_Verify(datatemp,Data_Buffer_1,Data_Buffer_2,Star_add,Length,CHECK_PATTERN);	
    Fail_Flag = P25QXX_Read_Verify(datatemp,Star_add,Length,CHECK_PATTERN);			
		return Fail_Flag;
}

u8 P25QXX_VT_Test(u8 test_type,u8 test_mode,signed int point_value)     
{
	u8 temp=0;
	AD5761R_Init();
//ENTER TEST MODE
	P25QXX_Enter_TestMode();
	AD5761R_SetVoltage_DIRECT(0);
	
//	Relay_on;
	CS_DAC;
	delay_ms(100);
	
// 之前应该切换cs管脚，接DAC输出
	P25QXX_Set_FTMOD(test_type);
	
//VTE_Verify	
	SPI_Process_Set(2);
	if(test_mode==0)
	{
	   temp=P25QXX_VT_Verify(test_type);
	}
	else
	{
		 temp=P25QXX_VT_Point_Verify(test_type,point_value);	
	}
	SPI_Process_Set(3);
//FTMOD_Normal	
	P25QXX_Set_FTMOD(FTMODE_NORMAL);	
//OUT TEST MODE
	P25QXX_Exit_TestMode();
//	Relay_off;
	CS_CS;
	AD5761R_SetVoltage_DIRECT(0);
	return temp;
}	

u8 P25QXX_OTP_VT_Test(u8 test_type,u8 test_mode,signed int point_value)    
{
	u8 temp=0;
	AD5761R_Init();

	//read OTP DATA
	P25QXX_Enter_TestMode(); 
	AD5761R_SetVoltage_DIRECT(0);
//	Relay_on;
	CS_DAC;
	delay_ms(100);
	
	QSPI_Send_CMD(P25X_TestMode_OTP_Read,SPI.m_Read_Operate_Start_addr,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_XX_BITS,QSPI_DATA_1_LINE); 					
//	QSPI_Receive(Data_Buffer_1,SPI.m_Read_Operate_Length); 
	QSPI_Receive(Data_Buffer_1 + SPI.m_Read_Operate_Start_addr,SPI.m_Read_Operate_Length);  	
	
	P25QXX_Set_FTMOD(test_type);
	
//VTE_Verify	
	SPI_Process_Set(2);
	if(test_mode==0)
	{
	   temp=P25QXX_VT_Verify(test_type);
	}
	else
	{
		 temp=P25QXX_VT_Point_Verify(test_type,point_value);	
	}
	SPI_Process_Set(3);
//FTMOD_Normal	
	P25QXX_Set_FTMOD(FTMODE_NORMAL);	
//OUT TEST MODE
	P25QXX_Exit_TestMode();
//	P25QXX_Power_Off();		
//	Relay_off;
	CS_CS;
	AD5761R_SetVoltage_DIRECT(0);
	return temp;
}

void SetContinueMode()//支持40H/S 11H 32S 32L
{
	u32 ConAddr[2];
	u8 ConTemp[2];
	u8 DataLen,i;
	switch((SPI.Type>>3)&0x0F)
	{
		case Q05:
		case Q10:
		case Q20:
		case Q40:
			if((SPI.Type&0x03) == 0x03)//40S
		    {	
					ConAddr[0]=0x100D;//EC
					ConTemp [0]= 0x80;
					ConAddr[1]=0x100F;//PC
					ConTemp[1] = 0x80;
					DataLen=2;
				}
				else
				{
					ConAddr[0]=0x1000D;
					ConTemp[0] = 0x90;
					DataLen=1;
				}
			break;
		case Q80:
		case Q16:
//			break;
		case Q32:
		case Q64:
				if((SPI.Type&0x03) == 0x03)//32S
		    {			
					ConAddr[0]=0x100D;//EC
					ConTemp [0]= 0x80;
					ConAddr[1]=0x100F;//PC
					ConTemp[1] = 0x80;
					DataLen=2;
				}
				else//H L U
				{
					ConAddr[0]=0x100D;
					ConTemp[0] = 0x90;
					DataLen=1;					
				}
				break;						
		case Q128:
		case Q256:
				ConAddr[0]=0x100D;
				ConTemp[0] = 0x90;
				DataLen=1;	
				break;
			default:
				break;
	}

	for(i=0;i<DataLen;i++)
	{
		QSPI_Send_CMD(0xD0,ConAddr[i],0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE);	
		QSPI_Transmit(&ConTemp[i],1);
		delay_us(5);
	}
	
}

