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
//如果使用os,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//PUYA UART驱动代码	   
//创建日期:2019/07/10		
////////////////////////////////////////////////////////////////////////////////// 	  
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
//#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR=(u8)ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

u8 aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART1_Handler; //UART句柄



//结构体定义
BUART	    MCUUART;     //UART 
ICINFO 		SPI;         //SPI flash config
//OPINFO    OPERATE;     //Operation config
STATEINFO STATE;       //Current state
VTINFO cVTINFO;
VOL_TRIM_INFO cVOL_TRIM_INFO;
FLASHSTEP cFLASHSTEP;


u8  UART_MCU_NUM=0xFF;



//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{	
	//UART 初始化设置
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			  //使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
	
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
		HAL_NVIC_EnableIRQ(USART1_IRQn);				    //使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,2,3);			//抢占优先级2，子优先级3
#endif	
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//如果是串口1
	{
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
		  if(USART_RX_STA&0x4000)//长度已确定
			 {						
					MCUUART.BufFromUART[USART_RX_STA&0X3FFF]=aRxBuffer[0] ;
				  USART_RX_STA++;	
					if((USART_RX_STA&0X3FFF) >= (MCUUART.DataLenFromUART))   //接收完成  			
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
					if((USART_RX_STA&0X3FFF)==3)//接收长度数据  上面++了 所以此处是3
					{					
						MCUUART.DataLenFromUART=aRxBuffer[0]+3+1; //共接收Res+3个数据(Cmd+MCUAddress+Len)+1校验位
						USART_RX_STA|=0x4000;	
						if(MCUUART.DataLenFromUART > USART_REC_LEN)
						 {
						   USART_RX_STA=0;
						 } //长度错误,重新开始接收
					}
			 }			
		}		
  }		
}
 
//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 
	u32 timeout=0;
  u32 maxDelay=0x1FFFF;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif	
	MCU_GLED_ON;
	HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数	
	timeout=0;
    while (HAL_UART_GetState(&UART1_Handler)!=HAL_UART_STATE_READY)//等待就绪
	{
        timeout++;////超时处理
        if(timeout>maxDelay) break;		
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&UART1_Handler,(u8 *)aRxBuffer, RXBUFFERSIZE)!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
        timeout++; //超时处理
        if(timeout>maxDelay) break;	
	}
  MCU_GLED_OFF;	
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
} 
#endif	


void SendDatatoUART(unsigned char *buf,unsigned char Len)
{
	unsigned char t=0;
//== delay ==//refer to real CLK	
	for(t=0;t<250;t++){;}			
		HAL_NVIC_DisableIRQ(USART1_IRQn);				//禁止USART1中断通道
//   MCU_GLED_ON;	
	 HAL_UART_Transmit(&UART1_Handler,(uint8_t*)buf,Len,1000);//将buf的值发送到PC
	 while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//等待发送结束	
//	 MCU_GLED_OFF;	
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
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
	
	//不检测CRC错误,直接返回ACK1
//	SendErrorCodeToPC(OK); 
//	memset(MCUUART.BufToUART, 0, sizeof(MCUUART.BufToUART));
	if(MCUUART.BufFromUART[2]!=0 && MCUUART.BufFromUART[3]!=0) //有step信息
	{ 
		cFLASHSTEP.count++;
	  if(cFLASHSTEP.count >= MAX_STEP_NUM+1)
		{
				SendErrorCodeToPC(ERROR_PARAMETERS);	                 //超出最大step	
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



 

 




