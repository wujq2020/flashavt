#ifndef __P25QXX_H
#define __P25QXX_H
#include "sys.h"

//High 8bit ManufactureID  , Low 8bit DeviceID 
#define P25Q05  0X8509
#define P25Q10  0X8510
#define P25Q20  0X8511
#define P25Q40 	0X8512 
#define P25Q80 	0X8513 	
#define P25Q16 	0X8514
#define P25Q32 	0X8515
#define P25Q64 	0X8516
#define P25Q128	0X8517
#define P25Q256 0X8518
#define P25Q512 0X8519
#define P25Q1G  0X851A
#define P25Q2G  0X851B

//通用参数
extern u16 P25QXX_TYPE;	             //默认是P25Q32
extern u8  P25QXX_QPI_MODE;		       //QPI模式标志:0,SPI模式;1,QPI模式.
extern u32 FlashSize;                //容量 单位BYTE
extern u32 PageSize;
extern u8  Flag_4Byte_3Byte_Address; // 0:3-Byte Address    1:4-Byte Address
extern u32 QSPI_ADDRESS_XX_BITS;     // 读写的ADDRESS长度 Q128及以下:XX=24    256及以上:XX=32
extern u8  CheckMode;

////////////////////////////////////////////////////////////////////////////////// 
//指令表  P25Q64U
//read
#define P25X_FastReadData	  	0x0B          //Read Array (fast) 
#define P25X_ReadData			    0x03          //Read Array (low power) 
#define P25X_FastReadDual	  	0x3B          //Read Dual Output
#define P25X_FastRead2IO	  	0xBB          //Read 2IO Output
#define P25X_FastReadQuad	    0x6B          //Read Quad Output
#define P25X_FastRead4IO		  0xEB          //Read 4IO Output
#define P25X_Word4IO		      0xE7          
#define P25X_OctalWord4IO	    0xE3          

//program and erase
#define P25X_PageErase		  	0x81          //Page Erase 
#define P25X_SectorErase		  0x20          //Sector Erase (4K bytes) 
#define P25X_BlockErase32			0x52          //Block Erase (32K bytes)
#define P25X_BlockErase64			0xD8          //Block Erase (64K bytes) 
#define P25X_ChipErase_C7h	  0xC7          //Chip Erase C7h
#define P25X_ChipErase_60h    0x60          //Chip Erase 60h
#define P25X_PageProgram		  0x02          //Page Program
#define P25X_DPageProgram		  0xA2          //Dual-IN Page Program
#define P25X_QPageProgram		  0x32          //Quad Page Program
#define P25X_PES_75h     		  0x75        
#define P25X_PES_b0h		      0xb0
#define P25X_PER_7Ah		      0x7A          
#define P25X_PER_30h		      0x30          

//protection
#define P25X_WriteEnable		  0x06          //Write Enable
#define P25X_WriteDisable		  0x04          //Write Disable
#define P25X_VWREN      		  0x50
#define P25X_SBLK             0x36
#define P25X_SULUK            0x39
#define P25X_RDBLK_3Ch        0x3C
#define P25X_RDBLK_3Dh        0x3D
#define P25X_GBLK             0x7E
#define P25X_GBULK            0x98

//security
#define P25X_ERSCUR           0x44
#define P25X_PRSCUR           0x42
#define P25X_RDSCUR           0x48

//status register
#define P25X_ReadStatusReg1		0x05          //Read Status Register 1
#define P25X_ReadStatusReg2		0x35          //Read Status Register 2  
#define P25X_ReadStatusReg3		0x15          //Read Configure Register 3    15h/45h
#define P25X_ASI          		0x25
#define P25X_WriteStatusReg1  0x01          //Write Status Register 1
#define P25X_WriteStatusReg2  0x31          //Write Status Register 2      0x31 or NULL  8M/32M不一样，不用了
#define P25X_WriteStatusReg3  0x11     		  //Write Configure Register 3   0x11 or 0x31

//other commands
#define P25X_ResetEnable      0x66
#define P25X_Reset            0x99  
#define P25X_EnterQPIMode     0x38          //Enable QPI  
#define P25X_JedecDeviceID		0x9F          //Read Manufacturer/device ID 
#define P25X_ManufactDeviceID	0x90          //Read Manufacture ID  
#define P25X_DualManufactDeviceID	0x92
#define P25X_QuadManufactDeviceID	0x94
#define P25X_PowerDown			  0xB9          //Deep Power-down
#define P25X_ReleasePowerDown	0xAB          //Release Deep Power-down/Read Electronic ID
#define P25X_SetBurstlength   0x77
#define P25X_ReadSFDP         0x5a
#define P25X_ExitQPIMode      0xFF          //Disable QPI 
#define P25X_ReadUniqueID     0x4B          //Read unique ID  

//未用
#define P25X_Enable4ByteAddr  0xB7          //Enable 4-Byte Address Mode
#define P25X_Exit4ByteAddr    0xE9          //Exit   4-Byte Address Mode
#define P25X_SetReadParam		  0xC0          //Set Read Parameters 

//test mode aommand
#define P25X_TestMode_Set_Reg_Data                   0xD0  
#define P25X_TestMode_Read_Reg_Data                  0xD9 
#define P25X_TestMode_Chip_Program_with_OTP_Memory   0xD1
#define P25X_TestMode_Chip_Erase_with_OTP_Memory     0xD2 
#define P25X_TestMode_Chip_Program                   0xD3
#define P25X_TestMode_Odd_Even_Page_Chip_Program     0xD4
#define P25X_TestMode_OTP_PageErase                  0xD5
#define P25X_TestMode_OTP_PageProgram                0xD6
#define P25X_TestMode_OTP_Read                       0xD7
#define P25X_TestMode_Odd_Even_Page_Plane_Program    0xDA
#define P25X_TestMode_Read_Compare                   0xDB
#define P25X_TestMode_Read_Redundancy                0xDC
//#define P25X_TestMode_Fast_Read                      0x0B
//#define P25X_TestMode_Read_Status                    0x05


//Protocol 4.1
#define Q05                   0x00
#define Q10                   0x01
#define Q20                   0x02
#define Q40                   0x03
#define Q80                   0x04
#define Q16                   0x05
#define Q32                   0x06
#define Q64                   0x07
#define Q128                  0x08
#define Q256                  0x09
#define Q512                  0x0A
#define Q1G                   0x0B
#define Q2G                   0x0C


#define FTMODE_NORMAL   			0x00
#define FTMODE_VREF   			  0x04
#define FTMODE_VTP   			    0x08
#define FTMODE_VTE   			    0x09


//CheckMode  (校验模式定义)
#define CHECK_PATTERN         0x01  //正反棋盘
#define CHECK_FF              0x02
#define READ_ONLY           	0x03
#define CHECK_00              0x04
#define CHECK_CUSTOMER        0x05


u8   P25QXX_QPI_MODE_CHK(void);         //QPI模式检查
//u16  P25QXX_ReadID(void);  	    		    //读取FLASH ID
u16 P25QXX_ReadID(u8* Uid);
//void P25QXX_Check_ID(void);             //验证FLASH ID 
void P25QXX_RDID(u8* pUid);
void SPI_Flash_Customs_Buffer_Init(u32 WriteStaAddr,u32 WriteEndAddr,u8* pBuffer);
void SPI_Flash_Buffer_Init(u8 ckb,u8 ckbba);
void SPI_Flash_Init(void);
void P25QXX_Init(void);					        //初始化P25QXX
void P25QXX_QE_Enable(void);            //使能QE  
void P25QXX_QE_Disable(void);           //清除QE
void P25QXX_Qspi_Enable(void);			    //使能QSPI模式
void P25QXX_Qspi_Disable(void);			    //关闭QSPI模式
u8   P25QXX_ReadSR(u8 regno);           //读取状态寄存器 
void P25QXX_Write_SR(u8 regno,u8 *pSRbuffer);   //写状态寄存器
void P25QXX_Wait_Busy(u32 Inst);
void P25QXX_Write_Enable(void);  																//写使能 
//void P25QXX_Write_Disable(void);																//写保护 (未用)
//void P25QXX_Volatile_SR_Write_Enable(void);
//void P25QXX_Global_Block_Lock(void);
//void P25QXX_Global_Block_Unlock(void);
//void P25QXX_Reset_Enable(void); 
//void P25QXX_Reset(void); 
void P25QXX_Deep_PowerDown(void); 
//void P25QXX_Suspend(void);
//void P25QXX_Resume(void);
void P25QXX_SendCMD1(u32 cmd); 
void P25QXX_Erase_Page_All(u32 Dst_Addr) ;            					//页擦
void P25QXX_Erase_Sector_All(u32 Dst_Addr);           					//扇区擦
void P25QXX_Erase_Block_All(u32 Dst_Addr);            					//Block擦
void P25QXX_Erase_Chip_All(u8 Inst);                  					//Chip擦
void P25QXX_Erase(u32 WriteAddr,u32 NumByteToWrite);  					//擦函数
void P25QXX_Write_Page_All(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite); //PP DPP QPP
void P25QXX_Write(u32 WriteAddr,u32 NumByteToWrite);
//void P25QXX_Write(u8* pBuffer_a,u8* pBuffer_ba,u32 WriteAddr,u32 NumByteToWrite);
void P25QXX_Read_No_Verify(u8* pBuffer,u32 ReadAddr,u32 NumByteToRead); 
//void P25QXX_Read_No_Verify(u8* pBuffer,u8* pVerfBuf,u32 ReadAddr,u32 NumByteToRead); 
unsigned char P25QXX_Read_Verify(u8* pBuffer,u32 ReadAddr,u32 NumByteToRead,u8  Check_Mode);
//unsigned char P25QXX_Read_Verify(u8* pBuffer,u8* pVerfBuf_a,u8* pVerfBuf_ba,u32 ReadAddr,u32 NumByteToRead,u8  Check_Mode);
void P25QXX_4ByteAddr_Enable(void);     //使能4字节地址模式
void P25QXX_4ByteAddr_Disable(void);    //禁止4字节地址模式
void P25QXX_Enter_TestMode(void);
void P25QXX_Exit_TestMode(void);
void P25QXX_Set_FTMOD(u8 m_ftmod);

void cVTINFO_init(void);
u8 P25QXX_VT_Test(u8 test_type,u8 test_mode,signed int point_value) ;
u8 P25QXX_VT_Verify(u8 test_type);
void P25QXX_TestMode_Write_All_chip(u8* pVerfBuf_a,u8* pVerfBuf_b) ;
u8 P25QXX_OTP_VT_Test(u8 test_type,u8 test_mode,signed int point_value) ;
void SetContinueMode(void);

#endif
