/***************************************************************************//**
 *   @file   ad5761r.h
 *   @brief  Header file of AD5761R Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifndef AD5761R_H_
#define AD5761R_H_

typedef unsigned char  Bool;  
#define false 0  
#define true  1 
 
#include "sys.h"

//#define AD5761R_RCC_GPIOClockCmd()    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA,ENABLE);RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC,ENABLE)

#define AD5761R_SCLK_PORT       GPIOA
#define AD5761R_SCLK_PIN        GPIO_PIN_8

#define AD5761R_MOSI_PORT       GPIOC
#define AD5761R_MOSI_PIN        GPIO_PIN_8

#define AD5761R_SYNC_PORT       GPIOC
#define AD5761R_SYNC_PIN         GPIO_PIN_9

#define AD5761R_MISO_PORT      GPIOC
#define AD5761R_MISO_PIN       GPIO_PIN_6

#define AD5761R_LDAC_PORT       GPIOC
#define AD5761R_LDAC_PIN        GPIO_PIN_7

#define AD5761R_CLEAR_PORT       GPIOD
#define AD5761R_CLEAR_PIN        GPIO_PIN_11

#define AD5761R_RESET_PORT      GPIOD
#define AD5761R_RESET_PIN       GPIO_PIN_14


#define AD5761R_SCLK_H         HAL_GPIO_WritePin(AD5761R_SCLK_PORT,AD5761R_SCLK_PIN,GPIO_PIN_SET)
#define AD5761R_SCLK_L         HAL_GPIO_WritePin(AD5761R_SCLK_PORT,AD5761R_SCLK_PIN,GPIO_PIN_RESET)

#define AD5761R_MOSI_H         HAL_GPIO_WritePin(AD5761R_MOSI_PORT,AD5761R_MOSI_PIN,GPIO_PIN_SET)
#define AD5761R_MOSI_L         HAL_GPIO_WritePin(AD5761R_MOSI_PORT,AD5761R_MOSI_PIN,GPIO_PIN_RESET)

//#define AD5761R_LDAC_H         HAL_GPIO_WritePin(AD5761R_LDAC_PORT,AD5761R_LDAC_PIN,GPIO_PIN_SET)
//#define AD5761R_LDAC_L         HAL_GPIO_WritePin(AD5761R_LDAC_PORT,AD5761R_LDAC_PIN,GPIO_PIN_RESET)

#define AD5761R_SYNC_H         HAL_GPIO_WritePin(AD5761R_SYNC_PORT,AD5761R_SYNC_PIN,GPIO_PIN_SET)
#define AD5761R_SYNC_L         HAL_GPIO_WritePin(AD5761R_SYNC_PORT,AD5761R_SYNC_PIN,GPIO_PIN_RESET)

//#define AD5761R_CLEAR_H         HAL_GPIO_WritePin(AD5761R_CLEAR_PORT,AD5761R_CLEAR_PIN,GPIO_PIN_SET)
//#define AD5761R_CLEAR_L         HAL_GPIO_WritePin(AD5761R_CLEAR_PORT,AD5761R_CLEAR_PIN,GPIO_PIN_RESET)

//#define AD5761R_RESET_H         HAL_GPIO_WritePin(AD5761R_RESET_PORT,AD5761R_RESET_PIN,GPIO_PIN_SET)
//#define AD5761R_RESET_L         HAL_GPIO_WritePin(AD5761R_RESET_PORT,AD5761R_RESET_PIN,GPIO_PIN_RESET)

#define  Read_MISO              HAL_GPIO_ReadPin(AD5761R_MISO_PORT,AD5761R_MISO_PIN)





/* Input Shift Register Format */
#define AD5761R_INPUT_ZERO_BIT			(1 << 20)
#define AD5761R_INPUT_ADDR_CMD(x)		(((x) & 0xF) << 16)
#define AD5761R_INPUT_DATA(x)			(((x) & 0xFFFF) << 0)

#define AD5761R_DATA(x)					(((x) & 0xFFFF) << 0)
#define AD5721R_DATA(x)					(((x) & 0xFFF) << 4)

/* Input Shift Register Commands */
#define CMD_NOP							0x0
#define CMD_WR_TO_INPUT_REG				0x1
#define CMD_UPDATE_DAC_REG				0x2
#define CMD_WR_UPDATE_DAC_REG			0x03
#define CMD_WR_CTRL_REG					0x4
#define CMD_NOP_ALT_1					0x5
#define CMD_NOP_ALT_2					0x6
#define CMD_SW_DATA_RESET				0x7
#define CMD_RESERVED					0x8
#define CMD_DIS_DAISY_CHAIN				0x9
#define CMD_RD_INPUT_REG				0xA
#define CMD_RD_DAC_REG					0xB
#define CMD_RD_CTRL_REG					0xC
#define CMD_NOP_ALT_3					0xD
#define CMD_NOP_ALT_4					0xE
#define CMD_SW_FULL_RESET				0xF

/* Control Register Format */
#define AD5761R_CTRL_SC					(1 << 12)			// RO
#define AD5761R_CTRL_BO					(1 << 11)			// RO
#define AD5761R_CTRL_CV(x)				(((x) & 0x3) << 9)	// RW
#define AD5761R_CTRL_OVR				(1 << 8)			// RW
#define AD5761R_CTRL_B2C				(1 << 7)			// RW
#define AD5761R_CTRL_ETS				(1 << 6)			// RW
#define AD5761R_CTRL_IRO				(1 << 5)			// RW
#define AD5761R_CTRL_PV(x)				(((x) & 0x3) << 3)	// RW
#define AD5761R_CTRL_RA(x)				(((x) & 0x7) << 0)	// RW

/* Disable Daisy-Chain Register Format */
#define AD5761R_DIS_DAISY_CHAIN_DDC(x)	(((x) & 0x1) << 0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum {
	AD5761R,
	AD5721R,
} ad5761r_type;

typedef enum {
	AD5761R_REG_INPUT,
	AD5761R_REG_DAC,
	AD5761R_REG_CTRL,
} ad5761r_reg;

typedef enum {
	AD5761R_SCALE_ZERO,
	AD5761R_SCALE_HALF,
	AD5761R_SCALE_FULL,
} ad5761r_scale;

typedef enum {
	AD5761R_RANGE_M_10V_TO_P_10V,
	AD5761R_RANGE_0_V_TO_P_10V,
	AD5761R_RANGE_M_5V_TO_P_5V,
	AD5761R_RANGE_0V_TO_P_5V,
	AD5761R_RANGE_M_2V5_TO_P_7V5,
	AD5761R_RANGE_M_3V_TO_P_3V,
	AD5761R_RANGE_0V_TO_P_16V,
	AD5761R_RANGE_0V_TO_P_20V,
} ad5761r_range;

typedef struct {
	ad5761r_type type;
	uint8_t spi_ss;
	uint8_t gpio_reset;
	uint8_t gpio_reset_value;
	uint8_t gpio_clr;
	uint8_t gpio_clr_value;
	uint8_t gpio_ldac;
	uint8_t gpio_ldac_value;
	ad5761r_range ra;
	ad5761r_scale pv;
	ad5761r_scale cv;
	Bool int_ref_en;
	Bool exc_temp_sd_en;
	Bool b2c_range_en;
	Bool ovr_en;
	Bool daisy_chain_en;
} ad5761r_dev;

typedef struct {
	ad5761r_type type;
	uint8_t spi_ss;
	uint8_t gpio_reset;
	uint8_t gpio_reset_value;
	uint8_t gpio_clr;
	uint8_t gpio_clr_value;
	uint8_t gpio_ldac;
	uint8_t gpio_ldac_value;
	ad5761r_range out_range;
	ad5761r_scale pwr_voltage;
	ad5761r_scale clr_voltage;
	Bool int_ref_en;
	Bool exc_temp_sd_en;
	Bool b2c_range_en;
	Bool ovr_en;
	Bool daisy_chain_en;
} ad5761r_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the device. */
int32_t ad5761r_init(void);
/* SPI write to device. */
int32_t ad5761r_write(ad5761r_dev *dev,
					  uint8_t reg_addr_cmd,
					  uint16_t reg_data);
/* SPI read from device. */
int32_t ad5761r_read(ad5761r_dev *dev,
					 uint8_t reg_addr_cmd,
					 uint16_t *reg_data);
/* Readback the register data. */
int32_t ad5761r_register_readback(ad5761r_dev *dev,
								  ad5761r_reg reg,
								  uint16_t *reg_data);
/* Configure the part based on the settings stored in the device structure. */
int32_t ad5761r_config(ad5761r_dev *dev);
/* Enable/disable daisy-chain mode. */
int32_t ad5761r_set_daisy_chain_en_dis(ad5761r_dev *dev,
									   Bool en_dis);
/* Get the status of the daisy-chain mode. */
int32_t ad5761r_get_daisy_chain_en_dis(ad5761r_dev *dev,
									   Bool *en_dis);
/* Set the output_range. */
int32_t ad5761r_set_output_range(ad5761r_dev *dev,
								 ad5761r_range out_range);
/* Get the output_range. */
int32_t ad5761r_get_output_range(ad5761r_dev *dev,
								 ad5761r_range *out_range);
/* Set the power up voltage. */
int32_t ad5761r_set_power_up_voltage(ad5761r_dev *dev,
									 ad5761r_scale pv);
/* Get the power up voltage. */
int32_t ad5761r_get_power_up_voltage(ad5761r_dev *dev,
									 ad5761r_scale *pv);
/* Set the clear voltage. */
int32_t ad5761r_set_clear_voltage(ad5761r_dev *dev,
								  ad5761r_scale cv);
/* Get the clear voltage. */
int32_t ad5761r_get_clear_voltage(ad5761r_dev *dev,
								  ad5761r_scale *cv);
/* Enable/disable internal reference. */
int32_t ad5761r_set_internal_reference_en_dis(ad5761r_dev *dev,
											  Bool en_dis);
/* Get the status of the internal reference. */
int32_t ad5761r_get_internal_reference_en_dis(ad5761r_dev *dev,
											  Bool *en_dis);
/* Enable/disable ETS (exceed temperature shutdown) function. */
int32_t ad5761r_set_exceed_temp_shutdown_en_dis(ad5761r_dev *dev,
												Bool en_dis);
/* Get the status of the ETS (exceed temperature shutdown) function. */
int32_t ad5761r_get_exceed_temp_shutdown_en_dis(ad5761r_dev *dev,
												Bool *en_dis);
/* Enable/disable the twos complement bipolar output range. */
int32_t ad5761r_set_2c_bipolar_range_en_dis(ad5761r_dev *dev,
											Bool en_dis);
/* Get the status of the twos complement bipolar output range. */
int32_t ad5761r_get_2c_bipolar_range_en_dis(ad5761r_dev *dev,
											Bool *en_dis);
/* Enable/disable the 5% overrange. */
int32_t ad5761r_set_overrange_en_dis(ad5761r_dev *dev,
									 Bool en_dis);
/* Get the status of the 5% overrange. */
int32_t ad5761r_get_overrange_en_dis(ad5761r_dev *dev,
									 Bool *en_dis);
/* Get the short-circuit condition. */
int32_t ad5761r_get_short_circuit_condition(ad5761r_dev *dev,
											Bool *sc);
/* Get the brownout condition. */
int32_t ad5761r_get_brownout_condition(ad5761r_dev *dev,
									   Bool *bo);
///* Set the reset pin value. */
//int32_t ad5761r_set_reset_pin(ad5761r_dev *dev,
//							  uint8_t value);
///* Get the reset pin value. */
//int32_t ad5761r_get_reset_pin(ad5761r_dev *dev,
//							  uint8_t *value);
///* Set the clr pin value. */
//int32_t ad5761r_set_clr_pin(ad5761r_dev *dev,
//							uint8_t value);
///* Get the clr pin value. */
//int32_t ad5761r_get_clr_pin(ad5761r_dev *dev,
//							uint8_t *value);
///* Set the ldac pin value. */
//int32_t ad5761r_set_ldac_pin(ad5761r_dev *dev,
//							 uint8_t value);
///* Get the ldac pin value. */
//int32_t ad5761r_get_ldac_pin(ad5761r_dev *dev,
//							 uint8_t *value);
/* Write to input register. */
int32_t ad5761r_write_input_register(ad5761r_dev *dev,
									 uint16_t reg_data);
/* Update DAC register. */
int32_t ad5761r_update_dac_register(ad5761r_dev *dev);
/* Write to input register and update DAC register. */
int32_t ad5761r_write_update_dac_register(ad5761r_dev *dev,
										  uint16_t reg_data);
/*Software data reset. */
int32_t ad5761r_software_data_reset(ad5761r_dev *dev);
/* Software full reset. */
int32_t ad5761r_software_full_reset(ad5761r_dev *dev);


static uint16_t  spi_write_and_read( uint8_t spi_ss, uint8_t *data,uint8_t number);
void AD5761R_Init(void);
void AD5761R_SetVoltage(unsigned int value);
void AD5761R_SetVoltage_DIRECT(float value);



#endif // AD5761R_H_
