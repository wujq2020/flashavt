#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"
#include "led.h"
#include "data.h"
#include <string.h>
//////////////////////////////////////////////////////////////////////////////////	 
////////////////////////////////////////////////////////////////////////////////// 	
#define FIRMWARE_VERSION    0x2100  
#define EXTEND_VERSION      0x0101 
#define USART_REC_LEN  		  300  	 
#define EN_USART1_RX 			  1		   


#define MAX_NUM             60   //电压分段最多支持MAX_NUM
#define MAX_STEP_NUM        60   //FLASH支持最大步骤
#define READ_PAGE_SIZE      256  //单次读字节数 
#define WRITE_PAGE_SIZE     256  //单次写字节数 


//define Error type	
#define		OK					        0x00
#define		ERROR_CHECKSUM		  0x01
#define		ERROR_PARAMETERS	  0x02
#define		ERROR_READCMP		    0x03
#define		ERROR_WRITEPOLLING	0x04
#define		ERROR_WRITERESULT   0x05
#define		ERROR_ERASE 		    0x07 
#define		ERROR_ERAL			    0x08
#define		ERROR_PASSWORDS		  0x09
#define		ERROR_READ		      0x0a

//define CMD type	
#define		COMMON_CMD			    0x00
#define		IIC_CMD				      0x01
#define		SPI_CMD				      0x02
#define		FLASH_CMD				    0x03
/************************************************************************************************/
//define COMMON_CMD ID
#define		SET_FLASH_TYPE			0x01
//#define		SET_FLASH_BPS			  0x02
#define		NOP					        0x03
#define		POR					        0x04	
#define		DELAY_MS 			      0x05
#define		READ_ID 				    0x06
#define		READ_POWER 		      0x07
#define		POWER_ON			      0x08
#define		POWER_OFF			      0x09
#define		Firmware_ver		    0x0a

#define		Set_Voltage_Trim_patten  	     0x14
#define		Voltage_Trim    	             0x15
#define	  Common_Set_Voltage_Trim_step   0x18
#define	  VT_Test_Wait_IRQHandler				 0x19

//define SPI_CMD ID
#define		SPI_CS 				      0x01
#define		SPI_SO				      0x02
#define		SPI_WP				      0x03
#define		SPI_SI				      0x04
#define		SPI_SK 				      0x05
#define		SPI_HOLD			      0x06
#define		SPI_ShiftOutNBit	  0x07
#define		SPI_ShiftInNBit 	  0x08
#define   SPI_Set_Mode        0x09
#define		SPI_Enter_TM        0x0a
#define		SPI_Exit_TM         0x0b
#define		SPI_Set_SPEED       0x0c
#define		SPI_Set_Voltage     0x0d
#define   SPI_Set_Address     0x0e
#define   SPI_Write_length    0x0f
#define   SPI_Read_length     0x10
#define   SPI_Set_Pattern     0x11
#define   SPI_Set_Cmd         0x12
#define   SPI_Polling         0x13
#define   SPI_Read_Verify     0x14
#define   SPI_Set_Back_Data   0x15
#define   SPI_TestMode        0x16
#define   SPI_IoMode          0x17

//define FLASH_CMD ID
#define		FLASH_Execute_Step  0xF0
#define   FLASH_Clear_Config  0xF2
#define   FLASH_VTE_TEST      0xF3
#define   FLASH_VTP_TEST      0xF4
#define   FLASH_OTP_VTE_TEST      0xF5
#define   FLASH_OTP_VTP_TEST      0xF6
#define   FLASH_Contimue_TEST      0xF7


extern u16 ReadPageSize;	
extern u16 WritePageSize;	

typedef struct _bUART     //UART 收发
{
	u8 BufFromUART[USART_REC_LEN];
  u8 DataLenFromUART;
//  u8 DataLenToUART;
	u8 BufToUART[USART_REC_LEN];
} BUART;	


typedef struct _STATEINFO //Current state information
{
//	u32	  CyclingtempCount;               //当前正在做第X次cycling
//	u32 	CyclingCount[TOTAL_FLASH];      //四个flash被cycling过的次数
//	u32   ErrorAddress[TOTAL_FLASH];      //四个flash的出错地址
//	u8    CyclingStatus[TOTAL_FLASH];     //四个flash的出错类型  0x00-Pass  0x01-ErrCheck  0x02-ErrCheckFF  0x04-ErrWrite  0x08-Unknow	
//	u8    ErrorData[TOTAL_FLASH];	        //四个flash的出错数据 
//	u8    CorrectData[TOTAL_FLASH];	      //四个flash的正确数据
//	u8    ErrorCount[TOTAL_FLASH];        //四个flash的出错次数
	u8 		BeginCycling;	                  //当前进行的操作  0：stop   1：cycling   
//  u8    OnlineSpi;                      //当前SPI在线状态标志位
} STATEINFO;	//STATE


typedef struct _VTINFO             //vt information
{
	signed int VP_Start_Value; 
	signed int VP_Stop_Value;
	signed int VP_Rough_Step_Value;  //粗调步长
	signed int VP_Fine_Step_Value;   //细调步长
	unsigned char VP_Era_Pro_flg;       //vp擦除烧录标志，0为不擦除，01为擦除  
	
	signed int VE_Start_Value;
	signed int VE_Stop_Value;
	signed int VE_Rough_Step_Value;
	signed int VE_Fine_Step_Value;
	unsigned char VE_Era_Pro_flg;       //ve擦除烧录标志，0为不擦除，01为擦除
	
	float Value;//VTE和VTP的结果
	
	u8 VT_otp_Flag;   //VT测试区域标志 0=》normal  ； 1=》otp
	u32 ErrorAdd;
	u8 ErrorData;
	u8 RightData;
	
//	unsigned char VT_CHECK_PATTERN;        //  0x00为正棋盘 0x01为反棋盘 0x02为客户patten
} VTINFO;	  //VT

typedef struct _VOL_TRIM_INFO    
{
	signed int m_Trim_Vol_Star[MAX_NUM];
	signed int m_Trim_Vol_End[MAX_NUM];
	signed int m_Trim_Vol_Step[MAX_NUM];
	u32 m_Trim_Vol_Time[MAX_NUM];
	u32 count;
	u8  V_C_Step;//电压变化到对应的地方就给一个触发信号0702
} VOL_TRIM_INFO;	  



typedef struct _ICINFO    //IC information
{
	u8 		Type;             //IC型号
	u16   ChipID;           //ManufactureID&DeviceID
  u8 		CommInterface;    //通信接口
  u8 		PageSize;         //页大小
  u8 		EraseInst;        //擦指令
	u8 		ProgramInst;      //写指令
  u8		ReadInst;         //读指令


//写擦指令参数
	u32 WrEr_INSTRUCTION_X_LINES;    
	u32 WrEr_ADDRESS_X_LINES;         
	u32 WRITE_DATA_X_LINES;           
//	u32 P25X_Write_Inst;

//读指令参数  //默认为0X3B读取
	u32 Read_ALTERNATE_X_LINES;      //交替字模式
	u32 Read_Dummy;
	u32 Read_INSTRUCTION_X_LINES;	
	u32	Read_ADDRESS_X_LINES;
	u32	Read_DATA_X_LINES;

	u8    CmdTestMode[3];   //根据型号稳定，可开放配置
	u8    Polling_flag;     //01-delay; 00-polling
	
	u8    Tpp;              //page program time
	u8    Tpe;              //page erase time
  u8    Tse; 	            //sector erase time
	u8    Tbe32k;           //Block erase time for 32K bytes
	u8    Tbe64k;           //Block erase time for 64K bytes
	u8    Tce;              //Chip erase time

	
	signed int     m_Eraser_Value;         //擦除电压 	
	signed int     m_Program_Value;        //烧录电压  
	signed int     m_Read_Value;           //读取电压
	signed int     m_Default_Value;        //空闲电压	
	
	signed int     m_Eraser_Speed;         //擦除频率 	
	signed int     m_Program_Speed;        //烧录频率  
	signed int     m_Read_Speed;           //读取频率
	signed int     m_Default_Speed;        //空闲频率
	
	unsigned int   m_Eraser_Operate_Start_addr;	   //擦除起始地址
	unsigned int   m_Eraser_Operate_End_addr;	     //擦除结束地址
	unsigned int   m_Eraser_Operate_Length;	       //擦除长度 m_Eraser_Operate_End_addr - m_Eraser_Operate_Start_addr +1
	
	unsigned int   m_Program_Operate_Start_addr;	 //烧录起始地址
	unsigned int   m_Program_Operate_End_addr;	   //烧录结束地址
	unsigned int   m_Program_Operate_Length;	     //擦除长度
	
	unsigned int   m_Read_Operate_Start_addr;	     //读起始地址
	unsigned int   m_Read_Operate_End_addr;	       //读结束地址
	unsigned int   m_Read_Operate_Length;	       //擦除长度
	
//	unsigned char  m_Customer_Pattern_Enanle; //00不使能 01 使能客户
//	unsigned char  m_Internal_Pattern_Select; //内部数据选择 00-正棋盘aa55, 01-反棋盘55aa, 02-正棋盘ff00 03-反棋盘00ff 
  unsigned char  m_Pattern_Select;          //00-棋盘xxxx, 01-随机数, 02-内部自定义 03-外挂flash 

} ICINFO;	  //SPI


typedef struct _FLASHSTEP    
{
	u8  STTP[MAX_STEP_NUM+1][32]; //存储step信息
  u32  count;	
} FLASHSTEP;	


extern BUART	   MCUUART;     //UART 
extern ICINFO 	 SPI;         //SPI flash config
//extern OPINFO    OPERATE;     //Operatiom config
extern STATEINFO STATE;       //Current state
extern VTINFO cVTINFO;
extern VOL_TRIM_INFO cVOL_TRIM_INFO;
extern FLASHSTEP cFLASHSTEP;

//extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef UART1_Handler; //UART句柄

#define RXBUFFERSIZE   1 //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL库USART接收Buffer

void SendDatatoUART(unsigned char *buf,unsigned char Len);
//void McuAckPC(u8 CurrentAddress);
void SetCfg_1(void);
//void AVTStatusCheck(void);
//void Disable_EXTI(void); 
//void EXTI_Init(void);  
void DisNumVal_printf(char ch[],u8 x,u8 y,u32 value);

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);

//AVT ADD
unsigned char GetCheckSum(unsigned char *buf, unsigned char  bLen);
void SendErrorCodeToPC(unsigned char bCode);
void CmdHandler(void);
void FLASHCMDHandler(void);
void CommonCMDHandler(unsigned char bCmdID);
void SPICMDHandler(unsigned char bCmdID);
void SPI_Process_Set(u8 ProcessNum);
void SPI_Era_Inst_Parse(void);
void SPI_Pro_Inst_Parse(void);
void SPI_Read_Inst_Parse(void);
unsigned int dectohex(unsigned int dec);
unsigned int hextodec(unsigned char hex_h,unsigned char hex_l);

#endif
