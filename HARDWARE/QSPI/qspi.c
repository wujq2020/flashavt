#include "qspi.h"
#include "delay.h"	 
#include "usart.h"	


QSPI_HandleTypeDef QSPI_Handler;    //QSPI���
    
//QSPI��ʼ��
u8 QSPI_Init(void)
{
    QSPI_Handler.Instance=QUADSPI;                          //QSPI
    QSPI_Handler.Init.ClockPrescaler=19;//10M               //QPSI��Ƶ�ȣ�һ�����Ƶ��Ϊ104M��
                                                            //�˴�Ϊ1����QSPIƵ�ʾ�Ϊ200/(1+1)=100MHZ��Ϊ8��200/(8+1)=22MHZ
    QSPI_Handler.Init.FifoThreshold=1;                      //FIFO��ֵΪ4���ֽ�
    QSPI_Handler.Init.SampleShifting=QSPI_SAMPLE_SHIFTING_HALFCYCLE;//������λ�������(DDRģʽ��,��������Ϊ0)
    QSPI_Handler.Init.FlashSize= 20 + ((SPI.Type>>3)&0x0F) - 1;
   	QSPI_Handler.Init.ChipSelectHighTime=QSPI_CS_HIGH_TIME_5_CYCLE;//Ƭѡ�ߵ�ƽʱ��Ϊ5��ʱ��(10*5=55ns),���ֲ������tSHSL����
//    QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_0;          //ģʽ0
	  QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_3;          //ģʽ3
    QSPI_Handler.Init.FlashID=QSPI_FLASH_ID_2;              //�ڶ�Ƭflash
    QSPI_Handler.Init.DualFlash=QSPI_DUALFLASH_DISABLE;     //��ֹ˫����ģʽ
    if(HAL_QSPI_Init(&QSPI_Handler)==HAL_OK) return 0;      //QSPI��ʼ���ɹ�
    else return 1;
}

u8 QSPI_Init_Customize(u8 speednum)
{
    u32 IOSpeedType = GPIO_SPEED_FREQ_VERY_HIGH;
   	GPIO_InitTypeDef GPIO_Initure;  
	//==============================QSPI����==============================	
//    QSPI_Handler.Instance=QUADSPI;                         
    QSPI_Handler.Init.ClockPrescaler=speednum;             
//    QSPI_Handler.Init.FifoThreshold=1;                     
//    QSPI_Handler.Init.SampleShifting=QSPI_SAMPLE_SHIFTING_HALFCYCLE;
//    QSPI_Handler.Init.FlashSize= 20 + ((SPI.Type>>3)&0x0F) - 1;
//   	QSPI_Handler.Init.ChipSelectHighTime=QSPI_CS_HIGH_TIME_5_CYCLE;
//    QSPI_Handler.Init.ClockMode=QSPI_CLOCK_MODE_0;          
//    QSPI_Handler.Init.FlashID=QSPI_FLASH_ID_2;             
//    QSPI_Handler.Init.DualFlash=QSPI_DUALFLASH_DISABLE; 

	
    if(HAL_QSPI_Init(&QSPI_Handler)==HAL_OK) //�������³�ʼ��IO������VTʱ�����WP��Ϊ���״̬
		{		 
		//==============================IO����(δ��)==============================
//			if(speednum<=4)         //40M - 100M       
//			 {IOSpeedType = GPIO_SPEED_FREQ_VERY_HIGH;}
//			else if(speednum<=24)   // 8M - 40M        
//			 {IOSpeedType = GPIO_SPEED_FREQ_HIGH;}	
//			else if(speednum<=39)   // 5M - 8M         
//			 {IOSpeedType = GPIO_SPEED_FREQ_MEDIUM;}			 
//			else                    // 0M - 5M
//			 {IOSpeedType = GPIO_SPEED_FREQ_LOW;}				 
//			 
//			//PC11                NCS     (Flash2 ��ʼ��)
//			GPIO_Initure.Pin=GPIO_PIN_11;
////			GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //����PP
////			GPIO_Initure.Pull=GPIO_PULLUP;                 //����
//			GPIO_Initure.Speed=IOSpeedType;                
//			GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //����ΪQSPI   
//			HAL_GPIO_Init(GPIOC,&GPIO_Initure);   
//			//PE7,8,9,10          IO0        IO1        IO2        IO3             
//			GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
////			GPIO_Initure.Pull=GPIO_PULLUP; 
////			GPIO_Initure.Speed=IOSpeedType;                
//			GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;      //����ΪQSPI
//			HAL_GPIO_Init(GPIOE,&GPIO_Initure);
//			//PB2                 CLK    
//			GPIO_Initure.Pin=GPIO_PIN_2;
//			GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //����PP
////			GPIO_Initure.Pull=GPIO_NOPULL;                 //����
////			GPIO_Initure.Speed=IOSpeedType;   
//			GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //����ΪQSPI
//			HAL_GPIO_Init(GPIOB,&GPIO_Initure); 
			return 0;
		}      //QSPI��ʼ���ɹ�
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
			return 0; //QSPI��ʼ���ɹ�
		}     
		else 
		return 1;
}

//QSPI�ײ�����,�������ã�ʱ��ʹ��
//�˺����ᱻHAL_QSPI_Init()����
//hqspi:QSPI���
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_QSPI_CLK_ENABLE();        //ʹ��QSPIʱ��
	
	  __HAL_RCC_QSPI_FORCE_RESET();
    __HAL_RCC_QSPI_RELEASE_RESET();    
	
    __HAL_RCC_GPIOB_CLK_ENABLE();       //ʹ��GPIOBʱ��
	  __HAL_RCC_GPIOC_CLK_ENABLE();       //ʹ��GPIOCʱ��	
    __HAL_RCC_GPIOE_CLK_ENABLE();       //ʹ��GPIOEʱ��

	   /* Enable DMA clock */
//    __HAL_RCC_MDMA_CLK_ENABLE();  
	
    //PC11                NCS     (Flash2 ��ʼ��)
    GPIO_Initure.Pin=GPIO_PIN_11;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //����
    GPIO_Initure.Pull=GPIO_PULLUP;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //����
//	  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //����ΪQSPI   
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //PE7,8,9,10          IO0        IO1        IO2        IO3             
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //����
//    GPIO_Initure.Pull=GPIO_NOPULL;      
    GPIO_Initure.Pull=GPIO_PULLUP; 	
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //����
		GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;      //����ΪQSPI
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);

    //PB2                 CLK    
    GPIO_Initure.Pin=GPIO_PIN_2;
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;             //����
    GPIO_Initure.Pull=GPIO_NOPULL;              
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //����
    GPIO_Initure.Alternate=GPIO_AF9_QUADSPI;       //����ΪQSPI
    HAL_GPIO_Init(GPIOB,&GPIO_Initure); 	

//    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
//		GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;            
//     
//    GPIO_Initure.Pull=GPIO_PULLUP; 	
//    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;  //����
////		GPIO_Initure.Alternate=GPIO_AF10_QUADSPI;      //����ΪQSPI
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

//QSPI��������
//instruction:Ҫ���͵�ָ��
//address:���͵���Ŀ�ĵ�ַ
//dummyCycles:��ָ��������
//	instructionMode:ָ��ģʽ;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE  
//	addressMode:��ַģʽ; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
//	addressSize:��ַ����;QSPI_ADDRESS_8_BITS,QSPI_ADDRESS_16_BITS,QSPI_ADDRESS_24_BITS,QSPI_ADDRESS_32_BITS
//	dataMode:����ģʽ; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE

void QSPI_Send_CMD(u32 instruction,u32 address,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode)
{
    QSPI_CommandTypeDef Cmdhandler;
    
    Cmdhandler.Instruction=instruction;                 	//ָ��
    Cmdhandler.Address=address;                            	//��ַ
    Cmdhandler.DummyCycles=dummyCycles;                     //���ÿ�ָ��������
    Cmdhandler.InstructionMode=instructionMode;				//ָ��ģʽ
    Cmdhandler.AddressMode=addressMode;   					//��ַģʽ
    Cmdhandler.AddressSize=addressSize;   					//��ַ����
    Cmdhandler.DataMode=dataMode;             				//����ģʽ
    Cmdhandler.SIOOMode=QSPI_SIOO_INST_EVERY_CMD;       	//ÿ�ζ�����ָ��
    Cmdhandler.AlternateByteMode=QSPI_ALTERNATE_BYTES_NONE; //�޽����ֽ�
    Cmdhandler.DdrMode=QSPI_DDR_MODE_DISABLE;           	//�ر�DDRģʽ
    Cmdhandler.DdrHoldHalfCycle=QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command(&QSPI_Handler,&Cmdhandler,5000);
	
}

void QSPI_Send_CMD_alternateByte(u32 instruction,u32 address,u32 alternateBytes,u32 alternateByteMode,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode)
{
    QSPI_CommandTypeDef Cmdhandler;
    
    Cmdhandler.Instruction=instruction;                 	  //ָ��
    Cmdhandler.Address=address;                            	//��ַ
	
//	  Cmdhandler.AlternateBytes=alternateBytes;               //������
	  Cmdhandler.AlternateBytes=0;                            //�����̶ֹ�Ϊ0
//	  Cmdhandler.AlternateBytesSize=alternateBytesSize;       //�������ֽ���
	  Cmdhandler.AlternateBytesSize=QSPI_ALTERNATE_BYTES_8_BITS;       //�������ֽ����̶�Ϊ���ֽ�
	  Cmdhandler.AlternateByteMode=alternateByteMode;         //������ģʽ
	
    Cmdhandler.DummyCycles=dummyCycles;                     //���ÿ�ָ��������
    Cmdhandler.InstructionMode=instructionMode;				      //ָ��ģʽ
    Cmdhandler.AddressMode=addressMode;   					        //��ַģʽ
    Cmdhandler.AddressSize=addressSize;   					        //��ַ����
    Cmdhandler.DataMode=dataMode;             				      //����ģʽ
    Cmdhandler.SIOOMode=QSPI_SIOO_INST_EVERY_CMD;       	  //ÿ�ζ�����ָ��
    Cmdhandler.DdrMode=QSPI_DDR_MODE_DISABLE;           	  //�ر�DDRģʽ
    Cmdhandler.DdrHoldHalfCycle=QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command(&QSPI_Handler,&Cmdhandler,5000);
}


//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
u8 QSPI_Receive(u8* buf,u32 datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                           //�������ݳ���
    if(HAL_QSPI_Receive(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //��������        //�˴�Ҫ��  ��ʱ����
    else return 1;
}

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
u8 QSPI_Transmit(u8* buf,u32 datalen)
{
    QSPI_Handler.Instance->DLR=datalen-1;                            //�������ݳ���
    if(HAL_QSPI_Transmit(&QSPI_Handler,buf,5000)==HAL_OK) return 0;  //��������
    else return 1;
}
