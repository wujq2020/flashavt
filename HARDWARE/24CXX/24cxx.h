#ifndef _24Cxx_H
#define _24Cxx_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//24CXX��������	   
//��������:2019/07/24									  
//////////////////////////////////////////////////////////////////////////////////
//====IIC����====
//IO��������
#define SDA_IN()  {GPIOD->MODER&=~(3<<(13*2));GPIOD->MODER|=0<<13*2;}	//PD13����ģʽ
#define SDA_OUT() {GPIOD->MODER&=~(3<<(13*2));GPIOD->MODER|=1<<13*2;} //PD13���ģʽ
//IO����
#define pc24IIC_SCL(n)  (n?HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET)) //SCL
#define pc24IIC_SDA(n)  (n?HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET)) //SDA
#define READ_SDA    HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_13)               //����SDA

//pc24IIC���в�������
void pc24IIC_Init(void);                 //��ʼ��IIC		 
void pc24IIC_Start(void);				         //����IIC��ʼ�ź�
void pc24IIC_Stop(void);	  			       //����IICֹͣ�ź�
void pc24IIC_Send_Byte(u8 txd);			     //IIC����һ���ֽ�
u8 pc24IIC_Read_Byte(unsigned char ack); //IIC��ȡһ���ֽ�
u8 pc24IIC_Wait_Ack(void); 				       //IIC�ȴ�ACK�ź�
void pc24IIC_Ack(void);					         //IIC����ACK�ź�
void pc24IIC_NAck(void);				         //IIC������ACK�ź�
void EE_Delay(void);                 //IICƵ�ʿ���

//====EEPROM����====
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
#define EE_PAGE_SIZE 64       //24C256 ���64

#define RST_ERRINFO 0x01      //���������Ϣ
#define RST_STATUS  0x02      //���״̬��Ϣ
#define RST_CONFIG  0x03      //���������Ϣ
#define SELF_CHECK  0x04      //EE�Լ�

//#define EE_VTINFO_START_ADDR  0x7000	//VT�洢EE��Ϣ��ַ

#define EE_VTINFO_START_ADDR  0x6000		

u8   P24CXX_ReadOneByte(u16 ReadAddr);							              		//ָ����ַ��ȡһ���ֽ�
void P24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		    				//ָ����ַд��һ���ֽ�
void P24CXX_WriteLenByte(u16 WriteAddr,u8* pBuffer,u16 NumByteToWrite);				//ָ����ַ��ʼд��ָ�����ȵ�����
u32  P24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					          			//ָ����ַ��ʼ��ȡָ����������
void P24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	  			//��ָ����ַ��ʼд��ָ�����ȵ�����
void P24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	  		  //��ָ����ַ��ʼ����ָ�����ȵ�����
void P24CXX_WriteCompletionPolling(void);                             //��ѯ
void P24CXX_PageWrite(u16 WriteAddr,u8* pBuffer,u16 NumByteToWrite);   //ҳд

u8   P24CXX_Check(void);     //�������
//void P24CXX_Operate(u8 mode);  
void P24CXX_Init(void);      //��ʼ��IIC   

void Merge_data(u8 *P_Buffer,u32 *P_Merge_Buffer);
void Split_data(u8 *P_Buffer,u32 *P_Split_Buffer);
//void Save_VTINFO_inf(void);
//void Read_VTINFO_inf(void);

#endif



