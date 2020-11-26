#ifndef _24Cxx_H
#define _24Cxx_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//24CXX驱动代码	   
//创建日期:2019/07/24									  
//////////////////////////////////////////////////////////////////////////////////
//====IIC驱动====
//IO方向设置
#define SDA_IN()  {GPIOD->MODER&=~(3<<(13*2));GPIOD->MODER|=0<<13*2;}	//PD13输入模式
#define SDA_OUT() {GPIOD->MODER&=~(3<<(13*2));GPIOD->MODER|=1<<13*2;} //PD13输出模式
//IO操作
#define pc24IIC_SCL(n)  (n?HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET)) //SCL
#define pc24IIC_SDA(n)  (n?HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET)) //SDA
#define READ_SDA    HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_13)               //输入SDA

//pc24IIC所有操作函数
void pc24IIC_Init(void);                 //初始化IIC		 
void pc24IIC_Start(void);				         //发送IIC开始信号
void pc24IIC_Stop(void);	  			       //发送IIC停止信号
void pc24IIC_Send_Byte(u8 txd);			     //IIC发送一个字节
u8 pc24IIC_Read_Byte(unsigned char ack); //IIC读取一个字节
u8 pc24IIC_Wait_Ack(void); 				       //IIC等待ACK信号
void pc24IIC_Ack(void);					         //IIC发送ACK信号
void pc24IIC_NAck(void);				         //IIC不发送ACK信号
void EE_Delay(void);                 //IIC频率控制

//====EEPROM驱动====
#define P24C01		127
#define P24C02		255
#define P24C04		511
#define P24C08		1023
#define P24C16		2047
#define P24C32		4095
#define P24C64	  8191
#define P24C128	  16383
#define P24C256	  32767  
#define P24C512	  65535  

#define EE_TYPE P24C256
#define EE_PAGE_SIZE 64       //24C256 最大64

#define RST_ERRINFO 0x01      //清除错误信息
#define RST_STATUS  0x02      //清除状态信息
#define RST_CONFIG  0x03      //清除配置信息
#define SELF_CHECK  0x04      //EE自检

//#define EE_VTINFO_START_ADDR  0x7000	//VT存储EE信息地址

#define EE_VTINFO_START_ADDR  0x6000		

u8   P24CXX_ReadOneByte(u16 ReadAddr);							              		//指定地址读取一个字节
void P24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		    				//指定地址写入一个字节
void P24CXX_WriteLenByte(u16 WriteAddr,u8* pBuffer,u16 NumByteToWrite);				//指定地址开始写入指定长度的数据
u32  P24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					          			//指定地址开始读取指定长度数据
void P24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	  			//从指定地址开始写入指定长度的数据
void P24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	  		  //从指定地址开始读出指定长度的数据
void P24CXX_WriteCompletionPolling(void);                             //轮询
void P24CXX_PageWrite(u16 WriteAddr,u8* pBuffer,u16 NumByteToWrite);   //页写

u8   P24CXX_Check(void);     //检查器件
//void P24CXX_Operate(u8 mode);  
void P24CXX_Init(void);      //初始化IIC   

void Merge_data(u8 *P_Buffer,u32 *P_Merge_Buffer);
void Split_data(u8 *P_Buffer,u32 *P_Split_Buffer);
//void Save_VTINFO_inf(void);
//void Read_VTINFO_inf(void);

#endif



