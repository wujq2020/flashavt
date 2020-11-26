#ifndef _DATA_H
#define _DATA_H
#include "sys.h"

typedef enum data
{
	PATTERN_CKB = 0X00,
	PATTERN_RANDOM = 0X01,
	PATTERN_CUSTOM = 0X02, //1k数据再循环
	PATTERN_FLASH = 0X03,
}data;

extern u32 Flash_Star_Addr;              
extern u32 Flash_End_Addr;              

extern u8  datatemp[8192];	
extern u8  Data_Buffer_1[8192];	 
extern u8  Data_Buffer_2[8192];
extern u8  Data_Buffer_Random_8K[8192];
extern u8  Data_Buffer_Custom_8K[8192];
extern u8  Data_Buffer_Flash_8K[8192];
extern u8 Data_Back_Buffer_1K[1024];
#endif
