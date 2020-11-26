#include "24cxx.h"
#include "delay.h"
#include "led.h"
#include "usart.h" 
#include "P25QXX.h"	
//====IIC����====
//IIC��ʼ��
void pc24IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��  
    //PD12,13��ʼ������
    GPIO_Initure.Pin=GPIO_PIN_12|GPIO_PIN_13;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;    //����
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
	SDA_OUT();     //sda�����
	pc24IIC_SDA(1);	  	  
	pc24IIC_SCL(1);
	EE_Delay();
 	pc24IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	EE_Delay();
	pc24IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
/*********************************************************/
//; Send EE24xx_Stop, defined as low-to-high SDA with EE24xx_SCL high.
//; EE24xx_SCL expected low on entry. Return with SCL, SDA high.
/*********************************************************/
void pc24IIC_Stop(void)
{	
	SDA_OUT();//sda�����
	pc24IIC_SCL(0);
	pc24IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
	EE_Delay();
	pc24IIC_SCL(1); 
	EE_Delay();
	pc24IIC_SDA(1);//����I2C���߽����ź�
	EE_Delay();							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 pc24IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	pc24IIC_SDA(1);    //1us?
	pc24IIC_SCL(1);    //1us?
	EE_Delay();	
	while(READ_SDA)
	{
		ucErrTime++;     
		if(ucErrTime>65530)    ///??????????????65530???????  ��ʱ
		{
			pc24IIC_Stop();
			return 1;
		}
	}
	pc24IIC_SCL(0);//ʱ�����0 	   
	return 0;                           //����Ӧ
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void pc24IIC_Send_Byte(u8 txd)
{ 	
    u8 t;   
	SDA_OUT(); 	    
    pc24IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
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
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 pc24IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        pc24IIC_NAck();//����nACK
    else
        pc24IIC_Ack(); //����ACK   
    return receive;
}



//====EEPROM����====
//��ʼ��
void P24CXX_Init(void)
{	
	pc24IIC_Init();//pc24IIC��ʼ��	
}

//��P24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 P24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  pc24IIC_Start();  
	if(EE_TYPE>P24C16)
	{
		pc24IIC_Send_Byte(0XA0);	        //����д����
		pc24IIC_Wait_Ack();
		pc24IIC_Send_Byte(ReadAddr>>8);   //���͸ߵ�ַ	    
	}else pc24IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	pc24IIC_Wait_Ack(); 
  pc24IIC_Send_Byte(ReadAddr%256);    //���͵͵�ַ
	pc24IIC_Wait_Ack();	    
	pc24IIC_Start();  	 	   
	pc24IIC_Send_Byte(0XA1);            //�������ģʽ			   
	pc24IIC_Wait_Ack();	 
  temp=pc24IIC_Read_Byte(0);		   
  pc24IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��P24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void P24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{		
  pc24IIC_Start();  
	if(EE_TYPE>P24C16)
	{
		pc24IIC_Send_Byte(0XA0);	         //����д����
		pc24IIC_Wait_Ack();
		pc24IIC_Send_Byte(WriteAddr>>8);   //���͸ߵ�ַ	  
	}else pc24IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	pc24IIC_Wait_Ack();	   
  pc24IIC_Send_Byte(WriteAddr%256);    //���͵͵�ַ
	pc24IIC_Wait_Ack(); 	 										  		   
	pc24IIC_Send_Byte(DataToWrite);      //�����ֽ�							   
	pc24IIC_Wait_Ack();  		    	   
  pc24IIC_Stop();//����һ��ֹͣ���� 
  P24CXX_WriteCompletionPolling();
}
//��P24CXX�����ָ����ַ��ʼд�볤��ΪNumByteToWrite������
//�ú�����������д�������ݣ�����ҳдģʽ���ٶȽϿ�.
//WriteAddr      :��ʼд��ĵ�ַ  
//pBuffer        :���������׵�ַ
//NumByteToWrite :д�����ݵĸ���
void P24CXX_WriteLenByte(u16 WriteAddr,u8* pBuffer,u16 NumByteToWrite)
{  	
	u8 pageremain;
	pageremain = EE_PAGE_SIZE - WriteAddr%EE_PAGE_SIZE;	
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;  //������ReadPageSize(default:256)���ֽ�	
	while(1)
	{
	  P24CXX_PageWrite(WriteAddr,pBuffer,pageremain);
		
		if(NumByteToWrite==pageremain)    												 //�ж��Ƿ�д��
		{	  		  	
			 break;//��������
		}
	 	else                                                       //NumByteToWrite>pageremain                    
		{			 					
			WriteAddr += pageremain;	       												 //����ʼ��ַ
			pBuffer   += pageremain;            										 //��buffer			
			NumByteToWrite-=pageremain;		   												 //��ȥ�Ѿ�д����ֽ���
			if(NumByteToWrite>EE_PAGE_SIZE)pageremain=EE_PAGE_SIZE;  //һ��д��EE_PAGE_SIZE(64)��BYTE
			else pageremain=NumByteToWrite;                          //����EE_PAGE_SIZE���ֽ���
		}	
	}									    
}

//��P24CXXָ����ַд��ҳ����
//WriteAddr  		 :��ʼд��ĵ�ַ    
//pBuffer		     :���������׵�ַ
//NumByteToWrite :д�����ݵĸ���
void P24CXX_PageWrite(u16 WriteAddr,u8* pBuffer,u16 NumByteToWrite)
{		
	u16 i;
  pc24IIC_Start();  
	if(EE_TYPE>P24C16)
	{
		pc24IIC_Send_Byte(0XA0);	        //����д����
		pc24IIC_Wait_Ack();
		pc24IIC_Send_Byte(WriteAddr>>8);  //���͸ߵ�ַ	  
	}else pc24IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	pc24IIC_Wait_Ack();	   
  pc24IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	pc24IIC_Wait_Ack(); 	
	for(i=0;i<NumByteToWrite;i++)   //ҳд
	{	
	  pc24IIC_Send_Byte(pBuffer[i]);    //�����ֽ�							   
	  pc24IIC_Wait_Ack();  
  }		
  pc24IIC_Stop();//����һ��ֹͣ���� 
  P24CXX_WriteCompletionPolling();
}


//��P24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
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
//���P24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 P24CXX_Check(void)
{
	u8 temp;
	temp=P24CXX_ReadOneByte(255);//����ÿ�ο�����дP24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
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
//		case RST_ERRINFO:                                         //����־
//			 MCU_RLED_ON;		
//		
//		   startadd = (u16)CHIP1_STATUS_ADD;
//		   endadd   = (u16)STATUS_END_ADD;
//       datasize = endadd - startadd + 1;
//		
//			 for(i=0;i<datasize;i++)   //С��4096  
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

//��P24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void P24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=P24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��P24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
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
		   pc24IIC_Send_Byte(0XA0);	    //����д����
		   AckTest = pc24IIC_Wait_Ack(); 
		   k++;
		   if(k>=50) AckTest=0;	 
	   }while(AckTest);//If write complete,chip will return 0(ACK); 	
}

void 	EE_Delay(void)
{
  delay_us(4);     //IIC�ٶ�����
}

void Merge_data(u8 *P_Buffer,u32 *P_Merge_Buffer)//���
{	
	*P_Merge_Buffer = 0;
	*P_Merge_Buffer = ((u32)(*(P_Buffer))<<24) | ((u32)(*(P_Buffer+1))<<16) | ((u32)(*(P_Buffer+2))<<8) | (u32)(*(P_Buffer+3));
}

void Split_data(u8 *P_Buffer,u32 *P_Split_Buffer)//���
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




