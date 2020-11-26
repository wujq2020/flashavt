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


#define MAX_NUM             60   //��ѹ�ֶ����֧��MAX_NUM
#define MAX_STEP_NUM        60   //FLASH֧�������
#define READ_PAGE_SIZE      256  //���ζ��ֽ��� 
#define WRITE_PAGE_SIZE     256  //����д�ֽ��� 


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

typedef struct _bUART     //UART �շ�
{
	u8 BufFromUART[USART_REC_LEN];
  u8 DataLenFromUART;
//  u8 DataLenToUART;
	u8 BufToUART[USART_REC_LEN];
} BUART;	


typedef struct _STATEINFO //Current state information
{
//	u32	  CyclingtempCount;               //��ǰ��������X��cycling
//	u32 	CyclingCount[TOTAL_FLASH];      //�ĸ�flash��cycling���Ĵ���
//	u32   ErrorAddress[TOTAL_FLASH];      //�ĸ�flash�ĳ����ַ
//	u8    CyclingStatus[TOTAL_FLASH];     //�ĸ�flash�ĳ�������  0x00-Pass  0x01-ErrCheck  0x02-ErrCheckFF  0x04-ErrWrite  0x08-Unknow	
//	u8    ErrorData[TOTAL_FLASH];	        //�ĸ�flash�ĳ������� 
//	u8    CorrectData[TOTAL_FLASH];	      //�ĸ�flash����ȷ����
//	u8    ErrorCount[TOTAL_FLASH];        //�ĸ�flash�ĳ������
	u8 		BeginCycling;	                  //��ǰ���еĲ���  0��stop   1��cycling   
//  u8    OnlineSpi;                      //��ǰSPI����״̬��־λ
} STATEINFO;	//STATE


typedef struct _VTINFO             //vt information
{
	signed int VP_Start_Value; 
	signed int VP_Stop_Value;
	signed int VP_Rough_Step_Value;  //�ֵ�����
	signed int VP_Fine_Step_Value;   //ϸ������
	unsigned char VP_Era_Pro_flg;       //vp������¼��־��0Ϊ��������01Ϊ����  
	
	signed int VE_Start_Value;
	signed int VE_Stop_Value;
	signed int VE_Rough_Step_Value;
	signed int VE_Fine_Step_Value;
	unsigned char VE_Era_Pro_flg;       //ve������¼��־��0Ϊ��������01Ϊ����
	
	float Value;//VTE��VTP�Ľ��
	
	u8 VT_otp_Flag;   //VT���������־ 0=��normal  �� 1=��otp
	u32 ErrorAdd;
	u8 ErrorData;
	u8 RightData;
	
//	unsigned char VT_CHECK_PATTERN;        //  0x00Ϊ������ 0x01Ϊ������ 0x02Ϊ�ͻ�patten
} VTINFO;	  //VT

typedef struct _VOL_TRIM_INFO    
{
	signed int m_Trim_Vol_Star[MAX_NUM];
	signed int m_Trim_Vol_End[MAX_NUM];
	signed int m_Trim_Vol_Step[MAX_NUM];
	u32 m_Trim_Vol_Time[MAX_NUM];
	u32 count;
	u8  V_C_Step;//��ѹ�仯����Ӧ�ĵط��͸�һ�������ź�0702
} VOL_TRIM_INFO;	  



typedef struct _ICINFO    //IC information
{
	u8 		Type;             //IC�ͺ�
	u16   ChipID;           //ManufactureID&DeviceID
  u8 		CommInterface;    //ͨ�Žӿ�
  u8 		PageSize;         //ҳ��С
  u8 		EraseInst;        //��ָ��
	u8 		ProgramInst;      //дָ��
  u8		ReadInst;         //��ָ��


//д��ָ�����
	u32 WrEr_INSTRUCTION_X_LINES;    
	u32 WrEr_ADDRESS_X_LINES;         
	u32 WRITE_DATA_X_LINES;           
//	u32 P25X_Write_Inst;

//��ָ�����  //Ĭ��Ϊ0X3B��ȡ
	u32 Read_ALTERNATE_X_LINES;      //������ģʽ
	u32 Read_Dummy;
	u32 Read_INSTRUCTION_X_LINES;	
	u32	Read_ADDRESS_X_LINES;
	u32	Read_DATA_X_LINES;

	u8    CmdTestMode[3];   //�����ͺ��ȶ����ɿ�������
	u8    Polling_flag;     //01-delay; 00-polling
	
	u8    Tpp;              //page program time
	u8    Tpe;              //page erase time
  u8    Tse; 	            //sector erase time
	u8    Tbe32k;           //Block erase time for 32K bytes
	u8    Tbe64k;           //Block erase time for 64K bytes
	u8    Tce;              //Chip erase time

	
	signed int     m_Eraser_Value;         //������ѹ 	
	signed int     m_Program_Value;        //��¼��ѹ  
	signed int     m_Read_Value;           //��ȡ��ѹ
	signed int     m_Default_Value;        //���е�ѹ	
	
	signed int     m_Eraser_Speed;         //����Ƶ�� 	
	signed int     m_Program_Speed;        //��¼Ƶ��  
	signed int     m_Read_Speed;           //��ȡƵ��
	signed int     m_Default_Speed;        //����Ƶ��
	
	unsigned int   m_Eraser_Operate_Start_addr;	   //������ʼ��ַ
	unsigned int   m_Eraser_Operate_End_addr;	     //����������ַ
	unsigned int   m_Eraser_Operate_Length;	       //�������� m_Eraser_Operate_End_addr - m_Eraser_Operate_Start_addr +1
	
	unsigned int   m_Program_Operate_Start_addr;	 //��¼��ʼ��ַ
	unsigned int   m_Program_Operate_End_addr;	   //��¼������ַ
	unsigned int   m_Program_Operate_Length;	     //��������
	
	unsigned int   m_Read_Operate_Start_addr;	     //����ʼ��ַ
	unsigned int   m_Read_Operate_End_addr;	       //��������ַ
	unsigned int   m_Read_Operate_Length;	       //��������
	
//	unsigned char  m_Customer_Pattern_Enanle; //00��ʹ�� 01 ʹ�ܿͻ�
//	unsigned char  m_Internal_Pattern_Select; //�ڲ�����ѡ�� 00-������aa55, 01-������55aa, 02-������ff00 03-������00ff 
  unsigned char  m_Pattern_Select;          //00-����xxxx, 01-�����, 02-�ڲ��Զ��� 03-���flash 

} ICINFO;	  //SPI


typedef struct _FLASHSTEP    
{
	u8  STTP[MAX_STEP_NUM+1][32]; //�洢step��Ϣ
  u32  count;	
} FLASHSTEP;	


extern BUART	   MCUUART;     //UART 
extern ICINFO 	 SPI;         //SPI flash config
//extern OPINFO    OPERATE;     //Operatiom config
extern STATEINFO STATE;       //Current state
extern VTINFO cVTINFO;
extern VOL_TRIM_INFO cVOL_TRIM_INFO;
extern FLASHSTEP cFLASHSTEP;

//extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef UART1_Handler; //UART���

#define RXBUFFERSIZE   1 //�����С
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL��USART����Buffer

void SendDatatoUART(unsigned char *buf,unsigned char Len);
//void McuAckPC(u8 CurrentAddress);
void SetCfg_1(void);
//void AVTStatusCheck(void);
//void Disable_EXTI(void); 
//void EXTI_Init(void);  
void DisNumVal_printf(char ch[],u8 x,u8 y,u32 value);

//����봮���жϽ��գ��벻Ҫע�����º궨��
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
