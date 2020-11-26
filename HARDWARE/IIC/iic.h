#ifndef _IIC_H
#define _IIC_H
#include "sys.h"
			
void I2C1_Init(void);
//void I2C_Send(u8* buf,u32 datalen);
void I2C_Send(uint16_t DevAddress, uint16_t MemAddress,u8* buf,u32 datalen);
#endif

