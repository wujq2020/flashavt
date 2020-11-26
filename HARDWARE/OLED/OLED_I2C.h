#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "sys.h"
#define  OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78
		
#define  OLED_SDA_ON  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_SET); 
#define  OLED_SDA_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_RESET); 

#define  OLED_SCL_ON  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET); 
#define  OLED_SCL_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET);

#define  ReadOLED_SDA  HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11);


#define OLED_SDA_IN()  {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=0<<11*2;}	//PB11输入模式
#define OLED_SDA_OUT() {GPIOB->MODER&=~(3<<(11*2));GPIOB->MODER|=1<<11*2;}  //PB11输出模式

#define  OLED_Min_Row 0
#define  OLED_Max_Row 7

#define  OLED_Min_Column 0
#define  OLED_Max_Column 132

#define  CHIP_ADDR_x 3
#define  CHIP_ADDR_y 0

#define  VT_Era_Vol_ADDR_x 3
#define  VT_Era_Vol_ADDR_y 2

#define  VTP_Ver_Vol_ADDR_x 3
#define  VTP_Ver_Vol_ADDR_y 3

#define  VTE_Ver_Vol_ADDR_x 3
#define  VTE_Ver_Vol_ADDR_y 4

//#define  VTP_Result_ADDR_x 10
#define  VTP_Result_ADDR_x 3
#define  VTP_Result_ADDR_y 6

#define  VTPba_Result_ADDR_x 60
#define  VTPba_Result_ADDR_y 6

//#define  VTE_Result_ADDR_x 10
#define  VTE_Result_ADDR_x 3
#define  VTE_Result_ADDR_y 7

#define  VTEba_Result_ADDR_x 60
#define  VTEba_Result_ADDR_y 7


void OLED_GPIO_Configuration(void);
void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_CLS1(unsigned char StartPage,
	             unsigned char EndPage,
							 unsigned char Startcolumn,
							 unsigned char Endcolumn,
							 unsigned char fill_Data);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, char ch[], unsigned char TextSize);
void OLED_ShowStrlen(unsigned char x, unsigned char y, unsigned char ch[], unsigned char Length,unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);



#endif
