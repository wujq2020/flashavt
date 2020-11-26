#include "adc.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ADC �����ѹ�ɼ�							  
////////////////////////////////////////////////////////////////////////////////// 	

ADC_HandleTypeDef ADC1_Handler;//ADC���
ADC_HandleTypeDef ADC3_Handler;//ADC���
//��ʼ��ADC
//ch: ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
void ADC1_Init(void)
{ 
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4��Ƶ��ADCCLK=PER_CK/4=64/4=16MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16λģʽ
    ADC1_Handler.Init.ScanConvMode=DISABLE;                    	//��ɨ��ģʽ
    ADC1_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//�ر�EOC�ж�
	  ADC1_Handler.Init.LowPowerAutoWait=DISABLE;									//�Զ��͹��Ĺر�				
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;               //�ر�����ת��
    ADC1_Handler.Init.NbrOfConversion=1;                        //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;            //��ֹ����������ģʽ
    ADC1_Handler.Init.NbrOfDiscConversion=0;                    //����������ͨ����Ϊ0
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //�������
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
	  ADC1_Handler.Init.BoostMode=ENABLE;							            //BOOTģʽ�ر�
	  ADC1_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			    //���µ����ݵ�����ֱ�Ӹ��ǵ�������
	  ADC1_Handler.Init.OversamplingMode=DISABLE;					        //�������ر�
	  ADC1_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //����ͨ�������ݽ���������DR�Ĵ�������
    HAL_ADC_Init(&ADC1_Handler);                                 //��ʼ�� 
	
	  HAL_ADCEx_Calibration_Start(&ADC1_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADCУ׼
}

void ADC3_Init(void)
{ 
    ADC3_Handler.Instance=ADC3;
    ADC3_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4��Ƶ��ADCCLK=PER_CK/4=64/4=16MHZ
    ADC3_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16λģʽ
    ADC3_Handler.Init.ScanConvMode=DISABLE;                    	//��ɨ��ģʽ
    ADC3_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//�ر�EOC�ж�
	  ADC3_Handler.Init.LowPowerAutoWait=DISABLE;									//�Զ��͹��Ĺر�				
    ADC3_Handler.Init.ContinuousConvMode=DISABLE;               //�ر�����ת��
    ADC3_Handler.Init.NbrOfConversion=1;                        //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC3_Handler.Init.DiscontinuousConvMode=DISABLE;            //��ֹ����������ģʽ
    ADC3_Handler.Init.NbrOfDiscConversion=0;                    //����������ͨ����Ϊ0
    ADC3_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //�������
    ADC3_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
	  ADC3_Handler.Init.BoostMode=ENABLE;							            //BOOTģʽ�ر�
	  ADC3_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			    //���µ����ݵ�����ֱ�Ӹ��ǵ�������
	  ADC3_Handler.Init.OversamplingMode=DISABLE;					        //�������ر�
	  ADC3_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //����ͨ�������ݽ���������DR�Ĵ�������
    HAL_ADC_Init(&ADC3_Handler);                                 //��ʼ�� 
	
	  HAL_ADCEx_Calibration_Start(&ADC3_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADCУ׼
}

//ADC�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_ADC_Init()����
//hadc:ADC���
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
	
    __HAL_RCC_ADC12_CLK_ENABLE();           //ʹ��ADC1/2ʱ��
    __HAL_RCC_ADC3_CLK_ENABLE();            //ʹ��ADC3ʱ��	
	  __HAL_RCC_GPIOA_CLK_ENABLE();			      //����GPIOCʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			      //����GPIOCʱ��
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC����ʱ��ѡ��	
	  
	  //V_FLASH_AD-PC4-ADC12_PIN4
    GPIO_Initure.Pin=GPIO_PIN_4;            
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);	
	
	 //V_CS_AD-PA4-ADC12_PIN18
    GPIO_Initure.Pin=GPIO_PIN_4;            
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	
	
	 //V_MCU_AD-PA3-ADC12_PIN15
    GPIO_Initure.Pin=GPIO_PIN_3;            
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	
		
		//V_GND_AD-PA2-ADC12_PIN14
    GPIO_Initure.Pin=GPIO_PIN_2;            
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     
    GPIO_Initure.Pull=GPIO_NOPULL;          
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//���ADCֵ
//ch: ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
//����ֵ:ת�����
u16 Get_Adc1(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                 //ͨ��
    ADC1_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;     //����ʱ��       
		ADC1_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				      //���߲ɼ�          		
		ADC1_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
		ADC1_ChanConf.Offset=0;   
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);      //ͨ������

    HAL_ADC_Start(&ADC1_Handler);                             //����ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);              //��ѯת��
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	              //�������һ��ADC1�������ת�����
}

u16 Get_Adc3(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC3_ChanConf;
    
    ADC3_ChanConf.Channel=ch;                                 //ͨ��
    ADC3_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
    ADC3_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;     //����ʱ��       
		ADC3_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				      //���߲ɼ�          		
		ADC3_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
		ADC3_ChanConf.Offset=0;   
    HAL_ADC_ConfigChannel(&ADC3_Handler,&ADC3_ChanConf);      //ͨ������

    HAL_ADC_Start(&ADC3_Handler);                             //����ADC
	
    HAL_ADC_PollForConversion(&ADC3_Handler,10);              //��ѯת��
	return (u16)HAL_ADC_GetValue(&ADC3_Handler);	              //�������һ��ADC3�������ת�����
}

//��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc1_Average(u32 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc1(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 

u16 Get_Adc3_Average(u32 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc3(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 

u16  Get_Power_Voltage(void)
{
    u16 PowerVol; 						      //�����ѹ
	  u16 VrefintData;    						//ʵ���ڲ�
	  u16 ADCxData;      						  //�����ѹ�˿ڲ���ֵ
			
		ADC3_Init();        						//�ڲβ���
		VrefintData = Get_Adc3_Average(ADC_CHANNEL_VREFINT,20);		
		ADC1_Init();        						//���Ե����
	  ADCxData=Get_Adc1_Average(ADC_CHANNEL_4,20);
	  HAL_ADC_DeInit(&ADC1_Handler);  //�ر�ADC
		HAL_ADC_DeInit(&ADC3_Handler);	
	
	  PowerVol =  ((ADCxData * VREFINT_CAL) >> 15)*3300/VrefintData;    //(3.3 * VREFINT_CAL * ADCx_DATA)/(VREFINT_DATA * FULL_SCALE)    * 2
	  return PowerVol;
}

u16  Get_Flash_Voltage(void)
{
    u16 PowerVol; 						      //�����ѹ
	  u16 ADCxData;      						  //�����ѹ�˿ڲ���ֵ
					
		ADC1_Init();        						//���Ե����
	  ADCxData=Get_Adc1_Average(ADC_CHANNEL_4,20);
	  HAL_ADC_DeInit(&ADC1_Handler);  //�ر�ADC

		PowerVol =  ADCxData*VREF*2/65536;   
		return PowerVol;
}

u16  Get_CS_Voltage(void)
{
    u16 PowerVol; 						      //�����ѹ
	  u16 ADCxData;      						  //�����ѹ�˿ڲ���ֵ
					
		ADC1_Init();        						//���Ե����
	  ADCxData=Get_Adc1_Average(ADC_CHANNEL_18,20);
	  HAL_ADC_DeInit(&ADC1_Handler);  //�ر�ADC

		PowerVol =  ADCxData*VREF/65536;   
		return PowerVol;
}

u16  Get_MCU_Voltage(void)
{
    u16 PowerVol; 						      //�����ѹ
	  u16 ADCxData;      						  //�����ѹ�˿ڲ���ֵ
					
		ADC1_Init();        						//���Ե����
	  ADCxData=Get_Adc1_Average(ADC_CHANNEL_15,20);
	  HAL_ADC_DeInit(&ADC1_Handler);  //�ر�ADC

		PowerVol =  ADCxData*VREF*2/65536;   
		return PowerVol;
}

u16  Get_GND_Voltage(void)
{
    u16 PowerVol; 						      //�����ѹ
	  u16 ADCxData;      						  //�����ѹ�˿ڲ���ֵ
					
		ADC1_Init();        						//���Ե����
	  ADCxData=Get_Adc1_Average(ADC_CHANNEL_14,20);
	  HAL_ADC_DeInit(&ADC1_Handler);  //�ر�ADC

		PowerVol =  ADCxData*VREF/65536;   
		return PowerVol;
}
