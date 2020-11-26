#ifndef __QSPI_H
#define __QSPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
/************************************************
 PUYA  20190619 
************************************************/						  
////////////////////////////////////////////////////////////////////////////////// 	
extern QSPI_HandleTypeDef QSPI_Handler;    //QSPI句柄

u8 QSPI_Init(void);												//初始化QSPI
void QSPI_Send_CMD(u32 instruction,u32 address,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode);			//QSPI发送命令
void QSPI_Send_CMD_alternateByte(u32 instruction,u32 address,u32 alternateBytes,u32 alternateByteMode,u32 dummyCycles,u32 instructionMode,u32 addressMode,u32 addressSize,u32 dataMode);
u8 QSPI_Receive(u8* buf,u32 datalen);							//QSPI接收数据
u8 QSPI_Transmit(u8* buf,u32 datalen);							//QSPI发送数据

u8 QSPI_Init_Customize(u8 speednum);
u8 QSPI_Init_Mode(u8 Mode);

#endif
