#include "usart.h"
#include "delay.h"
#include "led.h"
#include "24cxx.h"
#include "P25QXX.h"
#include "adc.h"
#include "qspi.h"
#include "OLED_I2C.h"
//#include <string.h>

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//PUYA UART��������	   
//��������:2019/07/10		
////////////////////////////////////////////////////////////////////////////////// 	  
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
//#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART1->TDR=(u8)ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	

u8 aRxBuffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART1_Handler; //UART���



//�ṹ�嶨��
BUART	    MCUUART;     //UART 
ICINFO 		SPI;         //SPI flash config
//OPINFO    OPERATE;     //Operation config
STATEINFO STATE;       //Current state
VTINFO cVTINFO;
VOL_TRIM_INFO cVOL_TRIM_INFO;
FLASHSTEP cFLASHSTEP;


u8  UART_MCU_NUM=0xFF;



//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound)
{	
	//UART ��ʼ������
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  
}

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			  //ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_9;			
		GPIO_Initure.Mode=GPIO_MODE_AF_OD;		
		GPIO_Initure.Pull=GPIO_NOPULL;			     
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	

		GPIO_Initure.Pin=GPIO_PIN_10;			     
		GPIO_Initure.Mode=GPIO_MODE_AF_OD;		
		GPIO_Initure.Pull=GPIO_NOPULL;			  
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   
		
#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);				    //ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART1_IRQn,2,3);			//��ռ���ȼ�2�������ȼ�3
#endif	
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//����Ǵ���1
	{
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
		  if(USART_RX_STA&0x4000)//������ȷ��
			 {						
					MCUUART.BufFromUART[USART_RX_STA&0X3FFF]=aRxBuffer[0] ;
				  USART_RX_STA++;	
					if((USART_RX_STA&0X3FFF) >= (MCUUART.DataLenFromUART))   //�������  			
					 {
						 USART_RX_STA|=0x8000;
//					   if(MCUUART.BufFromUART[1]!=0&&MCUUART.BufFromUART[1]!=McuAddress)
//						 { 
//							 USART_RX_STA=0;
//						   UART_MCU_NUM=0xFF;
//						 }					 
					 }						 	 			 				 
			 }
			else
			 {	
					MCUUART.BufFromUART[USART_RX_STA&0X3FFF]=aRxBuffer[0] ;			
					USART_RX_STA++;
					if((USART_RX_STA&0X3FFF)==3)//���ճ�������  ����++�� ���Դ˴���3
					{					
						MCUUART.DataLenFromUART=aRxBuffer[0]+3+1; //������Res+3������(Cmd+MCUAddress+Len)+1У��λ
						USART_RX_STA|=0x4000;	
						if(MCUUART.DataLenFromUART > USART_REC_LEN)
						 {
						   USART_RX_STA=0;
						 } //���ȴ���,���¿�ʼ����
					}
			 }			
		}		
  }		
}
 
//����1�жϷ������
void USART1_IRQHandler(void)                	
{ 
	u32 timeout=0;
  u32 maxDelay=0x1FFFF;
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntEnter();    
#endif	
	MCU_GLED_ON;
	HAL_UART_IRQHandler(&UART1_Handler);	//����HAL���жϴ����ú���	
	timeout=0;
    while (HAL_UART_GetState(&UART1_Handler)!=HAL_UART_STATE_READY)//�ȴ�����
	{
        timeout++;////��ʱ����
        if(timeout>maxDelay) break;		
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART1_Handler,(u8 *)aRxBuffer, RXBUFFERSIZE)!=HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
        timeout++; //��ʱ����
        if(timeout>maxDelay) break;	
	}
  MCU_GLED_OFF;	
#if SYSTEM_SUPPORT_OS	 	//ʹ��OS
	OSIntExit();  											 
#endif
} 
#endif	


void SendDatatoUART(unsigned char *buf,unsigned char Len)
{
	unsigned char t=0;
//== delay ==//refer to real CLK	
	for(t=0;t<250;t++){;}			
		HAL_NVIC_DisableIRQ(USART1_IRQn);				//��ֹUSART1�ж�ͨ��
//   MCU_GLED_ON;	
	 HAL_UART_Transmit(&UART1_Handler,(uint8_t*)buf,Len,1000);//��buf��ֵ���͵�PC
	 while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���	
//	 MCU_GLED_OFF;	
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//ʹ��USART1�ж�ͨ��
}



void SetCfg_1(void)
{
	SPI.Type           = MCUUART.BufFromUART[4];   //[4][5]  Reserve
//	SPI.ChipID         = MCUUART.BufFromUART[6]<<8|MCUUART.BufFromUART[7];
//	SPI.CommInterface	 = MCUUART.BufFromUART[8];
//	SPI.PageSize 			 = MCUUART.BufFromUART[9];   //Write PageSize
//	SPI.EraseInst 	   = MCUUART.BufFromUART[10];
//	SPI.ProgramInst 	 = MCUUART.BufFromUART[11];
//	SPI.ReadInst			 = MCUUART.BufFromUART[12]; 
}


void DisNumVal_printf(char ch[],u8 x,u8 y,u32 value)
{
	u8 count_d[4],i;
	count_d[3] = value%10;
	count_d[2] = (value/10)%10;
	count_d[1] = (value/100)%10;
	count_d[0] = (value/1000)%10;
		
	for(i=0;i<4;i++)
	{
		count_d[i] += 0x30; 
	}
	OLED_ShowStr(x,y,ch,1);
	OLED_ShowStrlen(x+6*strlen(ch),y,count_d,4,1); 
}

unsigned int dectohex(unsigned int dec)
{
	u8 count_d[4];
	unsigned int temp=0;
	count_d[3] = dec%10;
	count_d[2] = (dec/10)%10;
	count_d[1] = (dec/100)%10;
	count_d[0] = (dec/1000)%10;
	
	temp = count_d[0] *16*16*16 + count_d[1] *16*16 +count_d[2] *16 +count_d[3];
	return temp;
}

unsigned int hextodec(unsigned char hex_h,unsigned char hex_l)
{
	u8 count_d[4];
	unsigned int temp1;
	unsigned int temp;
	temp1 = (hex_h<<8) + hex_l;
	count_d[3] = temp1%16;
	count_d[2] = (temp1/16)%16;
	count_d[1] = (temp1/256)%16;
	count_d[0] = (temp1/4096)%16;
	
	temp = count_d[0] *10*10*10 + count_d[1] *10*10 +count_d[2] *10 +count_d[3];
	return temp;
}

unsigned char GetCheckSum(unsigned char *buf, unsigned char  bLen)
{
	unsigned char Checksum, i;
	Checksum = buf[0];
	for(i = 1; i < bLen; i++)
	{
		Checksum = Checksum^buf[i];
	}
	return Checksum;
}


void SendErrorCodeToPC(unsigned char bCode)
{
	unsigned char TempBuf[5];
	TempBuf[0] = MCUUART.BufFromUART[0];
	TempBuf[1] = MCUUART.BufFromUART[1];
	TempBuf[2] = bCode;
	TempBuf[3] = 0;
	TempBuf[4] = GetCheckSum(TempBuf,4);
 	SendDatatoUART(TempBuf, 5);	
}
void CmdHandler(void)
{
//	if(GetCheckSum(MCUUART.BufFromUART,MCUUART.DataLenFromUART-1) != MCUUART.BufFromUART[MCUUART.DataLenFromUART-1]) //check sum error
//	{
//			SendErrorCodeToPC(ERROR_CHECKSUM);
//			return;
//	}	
//	else
//	{
//	  	SendErrorCodeToPC(OK); 
//	}
	
	//�����CRC����,ֱ�ӷ���ACK1
//	SendErrorCodeToPC(OK); 
//	memset(MCUUART.BufToUART, 0, sizeof(MCUUART.BufToUART));
	if(MCUUART.BufFromUART[2]!=0 && MCUUART.BufFromUART[3]!=0) //��step��Ϣ
	{ 
		cFLASHSTEP.count++;
	  if(cFLASHSTEP.count >= MAX_STEP_NUM+1)
		{
				SendErrorCodeToPC(ERROR_PARAMETERS);	                 //�������step	
				return;
		}  
		memcpy(cFLASHSTEP.STTP[cFLASHSTEP.count],&MCUUART.BufFromUART[0],MCUUART.DataLenFromUART);

		MCUUART.BufToUART[0] = MCUUART.BufFromUART[0];								
		MCUUART.BufToUART[1] = MCUUART.BufFromUART[1];			
		MCUUART.BufToUART[2] = OK;									
		MCUUART.BufToUART[3] = 1;		    					
    MCUUART.BufToUART[4] = cFLASHSTEP.count;
		MCUUART.BufToUART[1 + 4] = GetCheckSum(MCUUART.BufToUART,1 +4 );		//check sum
		SendDatatoUART(MCUUART.BufToUART, 1 +4+1);
		return;
	}
	
	
	switch(MCUUART.BufFromUART[0])										
	{
		case COMMON_CMD:		//buf[0] = 0x00
			CommonCMDHandler(MCUUART.BufFromUART[1]); 
	    	break;
		case IIC_CMD:  		  //buf[0] = 0x01
//				IICCMDHandler(MCUUART.BufFromUART[1]);
			break;
		case SPI_CMD:				//buf[0] = 0x02
			SPICMDHandler(MCUUART.BufFromUART[1]);
			break;
		
		default:
			 break;
  }                      		
	MCUUART.DataLenFromUART =0;
//	memset(MCUUART.BufFromUART,0,32);	
}



 

 




