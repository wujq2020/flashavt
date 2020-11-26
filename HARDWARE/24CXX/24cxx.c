#include "24cxx.h"
#include "delay.h"
#include "led.h"
#include "usart.h" 
#include "P25QXX.h"	
//====IIC驱动====
//IIC初始化
void pc24IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOD_CLK_ENABLE();   //使能GPIOD时钟  
    //PD12,13初始化设置
    GPIO_Initure.Pin=GPIO_PIN_12|GPIO_PIN_13;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;    //快速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);  
    pc24IIC_SDA(1);
    pc24IIC_SCL(1);  
}
/*********************************************************/
//	; Send START, defined as high-to-low SDA with SCL high.
//	; Return with SCL, SDA low.
/*********************************************************/
void pc24IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	pc24IIC_SDA(1);	  	  
	pc24IIC_SCL(1);
	EE_Delay();
 	pc24IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	EE_Delay();
	pc24IIC_SCL(0);//钳住I2C总线，准备发送或接收数据 
}	  
/*********************************************************/
//; Send EE24xx_Stop, defined as low-to-high SDA with EE24xx_SCL high.
//; EE24xx_SCL expected low on entry. Return with SCL, SDA high.
/*********************************************************/
void pc24IIC_Stop(void)
{	
	SDA_OUT();//sda线输出
	pc24IIC_SCL(0);
	pc24IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
	EE_Delay();
	pc24IIC_SCL(1); 
	EE_Delay();
	pc24IIC_SDA(1);//发送I2C总线结束信号
	EE_Delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 pc24IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	pc24IIC_SDA(1);    //1us?
	pc24IIC_SCL(1);    //1us?
	EE_Delay();	
	while(READ_SDA)
	{
		ucErrTime++;     
		if(ucErrTime>65530)    ///??????????????65530???????  超时
		{
			pc24IIC_Stop();
			return 1;
		}
	}
	pc24IIC_SCL(0);//时钟输出0 	   
	return 0;                           //有响应
} 
/********************************************************/
//	; Clock out an acknowledge bit (low).
//	; SCL expected low on entry. Return with SCL, SDA low.
/********************************************************/
void pc24IIC_Ack(void)
{	
	pc24IIC_SCL(0);
	SDA_OUT();
	pc24IIC_SDA(0);
	EE_Delay();	
	pc24IIC_SCL(1);
	EE_Delay();	
	pc24IIC_SCL(0);
}
/***************************************************/
//	; Clock out a negative acknowledge bit (high).
//	; SCL expected low on entry. Return with SCL low, SDA high.
/****************************************************/	    
void pc24IIC_NAck(void)
{	
	pc24IIC_SCL(0);
	SDA_OUT();
	pc24IIC_SDA(1);
	EE_Delay();	
	pc24IIC_SCL(1);
	EE_Delay();	
	pc24IIC_SCL(0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void pc24IIC_Send_Byte(u8 txd)
{ 	
    u8 t;   
	SDA_OUT(); 	    
    pc24IIC_SCL(0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        pc24IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	  
	    EE_Delay();	  
			pc24IIC_SCL(1);
	    EE_Delay();	  
			pc24IIC_SCL(0);	
	    EE_Delay();	
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 pc24IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	  {
        pc24IIC_SCL(0); 
       	EE_Delay();	
		 pc24IIC_SCL(1);
        receive<<=1;
				EE_Delay();	
        if(READ_SDA)receive++;   
		   	EE_Delay();	 
    }					 
    if (!ack)
        pc24IIC_NAck();//发送nACK
    else
        pc24IIC_Ack(); //发送ACK   
    return receive;
}



//====EEPROM驱动====
//初始化
void P24CXX_Init(void)
{	
	pc24IIC_Init();//pc24IIC初始化	
}

//在P24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 P24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  pc24IIC_Start();  
	if(EE_TYPE>P24C16)
	{
		pc24IIC_Send_Byte(0XA0);	        //发送写命令
		pc24IIC_Wait_Ack();
		pc24IIC_Send_Byte(ReadAddr>>8);   //发送高地址	    
	}else pc24IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	pc24IIC_Wait_Ack(); 
  pc24IIC_Send_Byte(ReadAddr%256);    //发送低地址
	pc24IIC_Wait_Ack();	    
	pc24IIC_Start();  	 	   
	pc24IIC_Send_Byte(0XA1);            //进入接收模式			   
	pc24IIC_Wait_Ack();	 
  temp=pc24IIC_Read_Byte(0);		   
  pc24IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在P24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void P24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{		
  pc24IIC_Start();  
	if(EE_TYPE>P24C16)
	{
		pc24IIC_Send_Byte(0XA0);	         //发送写命令
		pc24IIC_Wait_Ack();
		pc24IIC_Send_Byte(WriteAddr>>8);   //发送高地址	  
	}else pc24IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	pc24IIC_Wait_Ack();	   
  pc24IIC_Send_Byte(WriteAddr%256);    //发送低地址
	pc24IIC_Wait_Ack(); 	 										  		   
	pc24IIC_Send_Byte(DataToWrite);      //发送字节							   
	pc24IIC_Wait_Ack();  		    	   
  pc24IIC_Stop();//产生一个停止条件 
  P24CXX_WriteCompletionPolling();
}
//在P24CXX里面的指定地址开始写入长度为NumByteToWrite的数据
//该函数用于连续写入多个数据，采用页写模式，速度较快.
//WriteAddr      :开始写入的地址  
//pBuffer        :数据数组首地址
//NumByteToWrite :写入数据的个数
void P24CXX_WriteLenByte(u16 WriteAddr,u8* pBuffer,u16 NumByteToWrite)
{  	
	u8 pageremain;
	pageremain = EE_PAGE_SIZE - WriteAddr%EE_PAGE_SIZE;	
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;  //不大于ReadPageSize(default:256)个字节	
	while(1)
	{
	  P24CXX_PageWrite(WriteAddr,pBuffer,pageremain);
		
		if(NumByteToWrite==pageremain)    												 //判断是否写完
		{	  		  	
			 break;//读出结束
		}
	 	else                                                       //NumByteToWrite>pageremain                    
		{			 					
			WriteAddr += pageremain;	       												 //跳起始地址
			pBuffer   += pageremain;            										 //跳buffer			
			NumByteToWrite-=pageremain;		   												 //减去已经写入的字节数
			if(NumByteToWrite>EE_PAGE_SIZE)pageremain=EE_PAGE_SIZE;  //一次写入EE_PAGE_SIZE(64)个BYTE
			else pageremain=NumByteToWrite;                          //不够EE_PAGE_SIZE个字节了
		}	
	}									    
}

//在P24CXX指定地址写入页数据
//WriteAddr  		 :开始写入的地址    
//pBuffer		     :数据数组首地址
//NumByteToWrite :写入数据的个数
void P24CXX_PageWrite(u16 WriteAddr,u8* pBuffer,u16 NumByteToWrite)
{		
	u16 i;
  pc24IIC_Start();  
	if(EE_TYPE>P24C16)
	{
		pc24IIC_Send_Byte(0XA0);	        //发送写命令
		pc24IIC_Wait_Ack();
		pc24IIC_Send_Byte(WriteAddr>>8);  //发送高地址	  
	}else pc24IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	pc24IIC_Wait_Ack();	   
  pc24IIC_Send_Byte(WriteAddr%256);   //发送低地址
	pc24IIC_Wait_Ack(); 	
	for(i=0;i<NumByteToWrite;i++)   //页写
	{	
	  pc24IIC_Send_Byte(pBuffer[i]);    //发送字节							   
	  pc24IIC_Wait_Ack();  
  }		
  pc24IIC_Stop();//产生一个停止条件 
  P24CXX_WriteCompletionPolling();
}


//在P24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 P24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=P24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查P24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 P24CXX_Check(void)
{
	u8 temp;
	temp=P24CXX_ReadOneByte(255);//避免每次开机都写P24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		P24CXX_WriteOneByte(255,0X55);
	    temp=P24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//void P24CXX_Operate(u8 mode)
//{
//	u16 i;
//	u16 startadd,endadd,datasize;
//	switch(mode)
//	{
//		case RST_ERRINFO:                                         //清日志
//			 MCU_RLED_ON;		
//		
//		   startadd = (u16)CHIP1_STATUS_ADD;
//		   endadd   = (u16)STATUS_END_ADD;
//       datasize = endadd - startadd + 1;
//		
//			 for(i=0;i<datasize;i++)   //小于4096  
//			 {
//				 datatemp[i]=0x00;	
//			 }	

//			 P24CXX_WriteLenByte(CHIP1_STATUS_ADD,datatemp,datasize);	
//			 delay_ms(500);
//			 MCU_RLED_OFF;	 		
//		  break;
//		case RST_STATUS:					
//		  break;
//		case RST_CONFIG:					
//		  break;		
//		case SELF_CHECK:
//		  break;		
//		default:
//			break;	
//	}	
//}

//在P24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void P24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=P24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在P24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void P24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		P24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

void P24CXX_WriteCompletionPolling(void)
{
	u8 AckTest;
	u8 k = 0;
	 do{		      
		   pc24IIC_Start(); 
		   pc24IIC_Send_Byte(0XA0);	    //发送写命令
		   AckTest = pc24IIC_Wait_Ack(); 
		   k++;
		   if(k>=50) AckTest=0;	 
	   }while(AckTest);//If write complete,chip will return 0(ACK); 	
}

void 	EE_Delay(void)
{
  delay_us(4);     //IIC速度设置
}

void Merge_data(u8 *P_Buffer,u32 *P_Merge_Buffer)//组合
{	
	*P_Merge_Buffer = 0;
	*P_Merge_Buffer = ((u32)(*(P_Buffer))<<24) | ((u32)(*(P_Buffer+1))<<16) | ((u32)(*(P_Buffer+2))<<8) | (u32)(*(P_Buffer+3));
}

void Split_data(u8 *P_Buffer,u32 *P_Split_Buffer)//拆分
{	
	(*P_Buffer)=(u8)(*P_Split_Buffer>>24); 
	(*(P_Buffer+1))=(u8)(*P_Split_Buffer>>16); 
	(*(P_Buffer+2))=(u8)(*P_Split_Buffer>>8); 
	(*(P_Buffer+3))=(u8)(*P_Split_Buffer); 			
}


//void Save_VTINFO_inf(void)
//{	
//	u8 count_d[sizeof(cVTINFO)]={0};
//	
//	Split_data(count_d,(u32 *)&cVTINFO.m_Eraser_Value);
//	
//	Split_data(&count_d[4],(u32 *)&cVTINFO.m_VP_Value);
//	Split_data(&count_d[8],(u32 *)&cVTINFO.VP_Start_Value);
//	Split_data(&count_d[12],(u32 *)&cVTINFO.VP_Stop_Value);
//	Split_data(&count_d[16],(u32 *)&cVTINFO.VP_Rough_Step_Value);
//	Split_data(&count_d[20],(u32 *)&cVTINFO.VP_Fine_Step_Value);
//	count_d[24] = cVTINFO.VP_Era_Pro_flg;
//	
//	Split_data(&count_d[25],(u32 *)&cVTINFO.m_VE_Value);
//	Split_data(&count_d[29],(u32 *)&cVTINFO.VE_Start_Value);
//	Split_data(&count_d[33],(u32 *)&cVTINFO.VE_Stop_Value);
//	Split_data(&count_d[37],(u32 *)&cVTINFO.VE_Rough_Step_Value);
//	Split_data(&count_d[41],(u32 *)&cVTINFO.VE_Fine_Step_Value);
//	count_d[45] = cVTINFO.VE_Era_Pro_flg;
//	count_d[46] = cVTINFO.VT_CHECK_PATTERN;
//		
//	P24CXX_WriteLenByte(EE_VTINFO_START_ADDR,count_d,sizeof(cVTINFO));
//	Dis_VT_Voltage();	
//}

//void Read_VTINFO_inf(void)
//{	
//	u8 count_d[sizeof(cVTINFO)]={0};
//	u8 i;
//	u8 sum_d =0;
//	P24CXX_Read(EE_VTINFO_START_ADDR,count_d,sizeof(cVTINFO));
//	
//	for(i=0;i<sizeof(cVTINFO);i++)
//	{
//		  sum_d += (count_d[i] -0xff);	
//	}
//	
//	if(sum_d == 0)
//	{
//		  cVTINFO_init();  
//	}
//	else
//	{
//			Merge_data(count_d,(u32 *)&cVTINFO.m_Eraser_Value);
//			Merge_data(&count_d[4],(u32 *)&cVTINFO.m_VP_Value);
//			Merge_data(&count_d[8],(u32 *)&cVTINFO.VP_Start_Value);
//			Merge_data(&count_d[12],(u32 *)&cVTINFO.VP_Stop_Value);
//			Merge_data(&count_d[16],(u32 *)&cVTINFO.VP_Rough_Step_Value);
//			Merge_data(&count_d[20],(u32 *)&cVTINFO.VP_Fine_Step_Value);
//			cVTINFO.VP_Era_Pro_flg =count_d[24] ;

//			Merge_data(&count_d[25],(u32 *)&cVTINFO.m_VE_Value);
//			Merge_data(&count_d[29],(u32 *)&cVTINFO.VE_Start_Value);
//			Merge_data(&count_d[33],(u32 *)&cVTINFO.VE_Stop_Value);
//			Merge_data(&count_d[37],(u32 *)&cVTINFO.VE_Rough_Step_Value);
//			Merge_data(&count_d[41],(u32 *)&cVTINFO.VE_Fine_Step_Value);
//			cVTINFO.VE_Era_Pro_flg =count_d[45] ;	
//	   	cVTINFO.VT_CHECK_PATTERN =count_d[46] ;	
//	}
//  Dis_VT_Voltage();	
//}




