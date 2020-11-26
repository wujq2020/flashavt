#include "qspi.h"
#include "delay.h"	 
#include "usart.h"	


QSPI_HandleTypeDef QSPI_Handler;    //QSPI句柄
    
//QSPI初始化
u8 QSPI_Init(void)
{
    QSPI_Handler.Instance=QUADSPI;                          //QSPI
    QSPI_Handler.Init.ClockPrescaler=19;//10M               //QPSI分频比，一般最大频率为104M，
                                                            //此处为1，则QSPI频率就为200/(1+1)=100MHZ，为8则200/(8+1)=22MHZ
    QSPI_Handler.Init.FifoThreshold=1;                      //FIFO阈值为4个字节
    QSPI_Handler.Init.SampleShifting=QSPI_SAMPLE_SHIFTING_HALFCYCLE;//采样移位半个周期(DDR模式下,必须设置为0)
    QSPI_Handler.Init.FlashSize= 20 + ((SPI.Type>>3)&0x0F) - 1;
   	QSPI_Handler.Init.ChipSelectHighTime=QSPI_CS_HIGH_TIME_5_CYCLE;//片选高电平时间为5个时钟(10*5=55ns),即手册里面的tSHSL参数
//    QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_0;          //模式0
	  QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_3;          //模式3
    QSPI_Handler.Init.FlashID=QSPI_FLASH_ID_2;              //第二片flash
    QSPI_Handler.Init.DualFlash=QSPI_DUALFLASH_DISABLE;     //禁止双闪存模式
    if(HAL_QSPI_Init(&QSPI_Handler)==HAL_OK) return 0;      //QSPI初始化成功
    else return 1;
}

u8 QSPI_Init_Customize(u8 speednum)
{
    u32 IOSpeedType = GPIO_SPEED_FREQ_VERY_HIGH;
   	GPIO_InitTypeDef GPIO_Initure;  
	//==============================QSPI配速==============================	
//    QSPI_Handler.Instance=QUADSPI;                         
    QSPI_Handler.Init.ClockPrescaler=speednum;             
//    QSPI_Handler.Init.FifoThreshold=1;                     
//    QSPI_Handler.Init.SampleShifting=QSPI_SAMPLE_SHIFTING_HALFCYCLE;
//    QSPI_Handler.Init.FlashSize= 20 + ((SPI.Type>>3)&0x0F) - 1;
//   	QSPI_Handler.Init.ChipSelectHighTime=QSPI_CS_HIGH_TIME_5_CYCLE;
//    QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_0;          
//    QSPI_Handler.Init.FlashID=QSPI_FLASH_ID_2;             
//    QSPI_Handler.Init.DualFlash=QSPI_DUALFLASH_DISABLE; 

	
    if(HAL_QSPI_Init(&QSPI_Handler)==HAL_OK) //不能重新初始化IO，测试VT时，会吧WP变为输出状态
		{		 
		//==============================IO配速(未测)==============================
//			if(speednum<=4)         //40M - 100M       
//			 {IOSpeedType = GPIO_SPEED_FREQ_VERY_HIGH;}
//			else if(speednum<=24)   // 8M - 40M        
//			 {IOSpeedType = GPIO_SPEED_FREQ_HIGH;}	
//			else if(speednum<=39)   // 5M - 8M         
//			 {IOSpeedType = GPIO_SPEED_FREQ_MEDIUM;}			 
//			else                    // 0M - 5M
//			 {IOSpeedType = GPIO_SPEED_FREQ_LOW;}				 
//			 
//			//PC11                NCS     (Flash2 初始化)
//			GPIO_Initure.Pin=GPIO_PIN_11;
////			GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用PP
////			GPIO_Initure.Pull=GPIO_PULLUP;                 //上拉
//			GPIO_Initure.Speed=IOSpeedType;                
//			GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //复用为QSPI   
//			HAL_GPIO_Init(GPIOC,&GPIO_Initure);   
//			//PE7,8,9,10          IO0        IO1        IO2        IO3             
//			GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
////			GPIO_Initure.Pull=GPIO_PULLUP; 
////			GPIO_Initure.Speed=IOSpeedType;                
//			GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;      //复用为QSPI
//			HAL_GPIO_Init(GPIOE,&GPIO_Initure);
//			//PB2                 CLK    
//			GPIO_Initure.Pin=GPIO_PIN_2;
//			GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用PP
////			GPIO_Initure.Pull=GPIO_NOPULL;                 //浮空
////			GPIO_Initure.Speed=IOSpeedType;   
//			GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //复用为QSPI
//			HAL_GPIO_Init(GPIOB,&GPIO_Initure); 
			return 0;
		}      //QSPI初始化成功
		else 
		return 1;
}


u8 QSPI_Init_Mode(u8 Mode)
{
	  u32 SPIModeType = QSPI_CLOCK_MODE_0;
	  if(Mode==0x00)
		{SPIModeType = QSPI_CLOCK_MODE_0;}
		else
		{SPIModeType = QSPI_CLOCK_MODE_3;}
		
    QSPI_Handler.Init.ClockMode=SPIModeType;    		      
    
    if(HAL_QSPI_Init(&QSPI_Handler)==HAL_OK) 
		{		 	
			return 0; //QSPI初始化成功
		}     
		else 
		return 1;
}

//QSPI底层驱动,引脚配置，时钟使能
//此函数会被HAL_QSPI_Init()调用
//hqspi:QSPI句柄
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_QSPI_CLK_ENABLE();        //使能QSPI时钟
	
	  __HAL_RCC_QSPI_FORCE_RESET();
    __HAL_RCC_QSPI_RELEASE_RESET();    
	
    __HAL_RCC_GPIOB_CLK_ENABLE();       //使能GPIOB时钟
	  __HAL_RCC_GPIOC_CLK_ENABLE();       //使能GPIOC时钟	
    __HAL_RCC_GPIOE_CLK_ENABLE();       //使能GPIOE时钟

	   /* Enable DMA clock */
//    __HAL_RCC_MDMA_CLK_ENABLE();  
	
    //PC11                NCS     (Flash2 初始化)
    GPIO_Initure.Pin=GPIO_PIN_11;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用
    GPIO_Initure.Pull=GPIO_PULLUP;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //高速
//	  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //复用为QSPI   
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //PE7,8,9,10          IO0        IO1        IO2        IO3             
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用
//    GPIO_Initure.Pull=GPIO_NOPULL;      
    GPIO_Initure.Pull=GPIO_PULLUP; 	
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //高速
		GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;      //复用为QSPI
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);

    //PB2                 CLK    
    GPIO_Initure.Pin=GPIO_PIN_2;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //复用
    GPIO_Initure.Pull=GPIO_NOPULL;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //高速
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //复用为QSPI
    HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	

//    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
//		GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;            
//     
//    GPIO_Initure.Pull=GPIO_PULLUP; 	
//    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //高速
////		GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;      //复用为QSPI
//    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
//		
//		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_SET);	    //PA6   (R)    
//	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET);	    //PA7	 	(G)
//		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_SET);	    //PA6   (R)    
//	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET);
//		
//		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_SET);	    //PA6   (R)    
//	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_RESET);	    //PA7	 	(G)
//		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_SET);	    //PA6   (R)    
//	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_RESET);
//		
//		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_SET);	    //PA6   (R)    
//	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_RESET);	    //PA7	 	(G)
//		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_SET);	    //PA6   (R)    
//	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_RESET);
//		
//		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_SET);	    //PA6   (R)    
//	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_RESET);	    //PA7	 	(G)
//		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_SET);	    //PA6   (R)    
//	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_RESET);
		
}

//QSPI发送命令
//instruction:要发送的指令
//address:发送到的目的地址
//dummyCycles:空指令周期数
//	instructionMode:指令模式;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE  
//	addressMode:地址模式; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
//	addressSize:地址长度;QSPI_ADDRESS_8_BITS,QSPI_ADDRESS_16_BITS,QSPI_ADDRESS_24_BITS,QSPI_ADDRESS_32_BITS
//	dataMode:数据模式; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE

void QSPI_Send_CMD(u32 instruction,u32 address,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode)
{
    QSPI_CommandTypeDef Cmdhandler;
    
    Cmdhandler.Instruction=instruction;                 	//指令
    Cmdhandler.Address=address;                            	//地址
    Cmdhandler.DummyCycles=dummyCycles;                     //设置空指令周期数
    Cmdhandler.InstructionMode=instructionMode;				//指令模式
    Cmdhandler.AddressMode=addressMode;   					//地址模式
    Cmdhandler.AddressSize=addressSize;   					//地址长度
    Cmdhandler.DataMode=dataMode;             				//数据模式
    Cmdhandler.SIOOMode=QSPI_SIOO_INST_EVERY_CMD;       	//每次都发送指令
    Cmdhandler.AlternateByteMode=QSPI_ALTERNATE_BYTES_NONE; //无交替字节
    Cmdhandler.DdrMode=QSPI_DDR_MODE_DISABLE;           	//关闭DDR模式
    Cmdhandler.DdrHoldHalfCycle=QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command(&QSPI_Handler,&Cmdhandler,5000);
	
}

void QSPI_Send_CMD_alternateByte(u32 instruction,u32 address,u32 alternateBytes,u32 alternateByteMode,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode)
{
    QSPI_CommandTypeDef Cmdhandler;
    
    Cmdhandler.Instruction=instruction;                 	  //指令
    Cmdhandler.Address=address;                            	//地址
	
//	  Cmdhandler.AlternateBytes=alternateBytes;               //交替字
	  Cmdhandler.AlternateBytes=0;                            //交替字固定为0
//	  Cmdhandler.AlternateBytesSize=alternateBytesSize;       //交替字字节数
	  Cmdhandler.AlternateBytesSize=QSPI_ALTERNATE_BYTES_8_BITS;       //交替字字节数固定为单字节
	  Cmdhandler.AlternateByteMode=alternateByteMode;         //交替字模式
	
    Cmdhandler.DummyCycles=dummyCycles;                     //设置空指令周期数
    Cmdhandler.InstructionMode=instructionMode;				      //指令模式
    Cmdhandler.AddressMode=addressMode;   					        //地址模式
    Cmdhandler.AddressSize=addressSize;   					        //地址长度
    Cmdhandler.DataMode=dataMode;             				      //数据模式
    Cmdhandler.SIOOMode=QSPI_SIOO_INST_EVERY_CMD;       	  //每次都发送指令
    Cmdhandler.DdrMode=QSPI_DDR_MODE_DISABLE;           	  //关闭DDR模式
    Cmdhandler.DdrHoldHalfCycle=QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command(&QSPI_Handler,&Cmdhandler,5000);
}


//QSPI接收指定长度的数据
//buf:接收数据缓冲区首地址
//datalen:要传输的数据长度
//返回值:0,正常
//    其他,错误代码
u8 QSPI_Receive(u8* buf,u32 datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                           //配置数据长度
    if(HAL_QSPI_Receive(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //接收数据        //此处要改  超时处理
    else return 1;
}

//QSPI发送指定长度的数据
//buf:发送数据缓冲区首地址
//datalen:要传输的数据长度
//返回值:0,正常
//    其他,错误代码
u8 QSPI_Transmit(u8* buf,u32 datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                            //配置数据长度
    if(HAL_QSPI_Transmit(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //发送数据
    else return 1;
}
