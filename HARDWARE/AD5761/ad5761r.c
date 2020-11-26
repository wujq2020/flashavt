/***************************************************************************//**
 *   @file   ad5761r.c
 *   @brief  Implementation of AD5761R Driver.
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
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "ad5761r.h"
#include "delay.h"

  
#define false 0  
#define true  1 

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

//static uint8_t gpio_set_value( uint8_t gpio_pin,uint8_t value);

/*****用于模拟SPI通信*****/
static uint16_t  spi_write_and_read( uint8_t spi_ss, uint8_t *data,uint8_t number)   
{
	 unsigned char i = 0;
	 unsigned char j = 0;
	 uint8_t temp;
	 uint32_t ret;  //返回数据
	
//		AD5761R_LDAC_L;
		AD5761R_SCLK_H;

	 AD5761R_SYNC_L;
//	 delay_us(100);
	 delay_us(10);
	 for( i = 0; i< number;i++)
	 {
		 temp = *(data + i);
		 for( j = 0; j< 8;j++)
		 {		
        ret <<= 1;				 
			  if( (temp & 0x80) == 0x80)
					AD5761R_MOSI_H;
				else
				  AD5761R_MOSI_L;
				
				if(Read_MISO)
					ret |=0X01;
				
//        delay_us(2);
				AD5761R_SCLK_L;
			  delay_us(2);
//				  delay_us(200);
				AD5761R_SCLK_H;
				delay_us(2);
//				delay_us(200);
				temp <<= 1;	
//        ret <<= 1;					
		 }
	 }
	 AD5761R_SYNC_H;
	 return ret;
}

/**
 * SPI write to device.
 * @param dev - The device structure.
 * @param reg_addr_cmd - The input shift register command.
 * 						 Accepted values: CMD_NOP
 * 										  CMD_WR_TO_INPUT_REG
 * 										  CMD_UPDATE_DAC_REG_FROM_INPUT_REG
 * 										  CMD_WR_UPDATE_DAC_REG
 * 										  CMD_WR_CTRL_REG
 * 										  CMD_SW_DATA_RESET
 * 										  CMD_DIS_DAISY_CHAIN
 * 										  CMD_RD_INPUT_REG
 * 										  CMD_RD_DAC_REG
 * 										  CMD_RD_CTRL_REG
 * 										  CMD_SW_FULL_RESET
 * @param reg_data - The transmitted data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_write(ad5761r_dev *dev,
											uint8_t reg_addr_cmd,
											uint16_t reg_data)
{
	uint8_t data[3];
	int32_t ret;

	data[0] = reg_addr_cmd;
	data[1] = (reg_data & 0xFF00) >> 8;
	data[2] = (reg_data & 0x00FF) >> 0;
	ret = spi_write_and_read(dev->spi_ss, data, 3);
  
	return ret;
}

/**
 *  SPI read from device.
 * @param dev - The device structure.
 * @param reg_addr_cmd - The input shift register command.
 * 						 Accepted values: CMD_NOP
 * 										  CMD_WR_TO_INPUT_REG
 * 										  CMD_UPDATE_DAC_REG_FROM_INPUT_REG
 * 										  CMD_WR_UPDATE_DAC_REG
 * 										  CMD_WR_CTRL_REG
 * 										  CMD_SW_DATA_RESET
 * 										  CMD_DIS_DAISY_CHAIN
 * 										  CMD_RD_INPUT_REG
 * 										  CMD_RD_DAC_REG
 * 										  CMD_RD_CTRL_REG
 * 										  CMD_SW_FULL_RESET
 * @param - The received data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_read(ad5761r_dev *dev,
					 uint8_t reg_addr_cmd,
					 uint16_t *reg_data)
{
	uint8_t data[3];
	int32_t ret;

	data[0] = reg_addr_cmd;
	data[1] = 0;
	data[2] = 0;
	ret = spi_write_and_read(dev->spi_ss, data, 3);
	*reg_data = (data[1] << 8) | data[2];

	return ret;
}

/**
 * Readback the register data.
 * Note: Readback operation is not enabled if daisy-chain mode is disabled.
 * @param dev - The device structure.
 * @param reg - The register to be read.
 * 				Accepted values: AD5761R_REG_INPUT
 * 								 AD5761R_REG_DAC
 * 								 AD5761R_REG_CTRL
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_register_readback(ad5761r_dev *dev,
								  ad5761r_reg reg,
								  uint16_t *reg_data)
{
	uint8_t reg_addr;
	int32_t ret;

	if (!dev->daisy_chain_en) {
		
		return -1;
	}

	switch (reg) {
	case AD5761R_REG_INPUT:
		reg_addr = CMD_RD_INPUT_REG;
		break;
	case AD5761R_REG_DAC:
		reg_addr = CMD_RD_DAC_REG;
		break;
	case AD5761R_REG_CTRL:
		reg_addr = CMD_RD_CTRL_REG;
		break;
	default:
		return -1;
	}

	/* During the first command, the last 16 bits are don�t care bits. */
	ret = ad5761r_read(dev, reg_addr, reg_data);

	/* During the next command, the register contents are shifted out of the
	   SDO pin with the MSB shifted out first. */
	ret |= ad5761r_read(dev, reg_addr, reg_data);

	return ret;
}

/**
 * Configure the part based on the settings stored in the device structure.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_config(ad5761r_dev *dev)
{
	uint16_t reg_data;

	reg_data = AD5761R_CTRL_CV(dev->cv) |
			   (dev->ovr_en ? AD5761R_CTRL_OVR : 0) |
			   (dev->b2c_range_en? AD5761R_CTRL_B2C : 0) |
			   (dev->exc_temp_sd_en ? AD5761R_CTRL_ETS : 0) |
			   (dev->int_ref_en ? 0 : AD5761R_CTRL_IRO) |
			   AD5761R_CTRL_PV(dev->pv) |
			   AD5761R_CTRL_RA(dev->ra);

	return ad5761r_write(dev, CMD_WR_CTRL_REG, reg_data);
}

/**
 * Enable/disable daisy-chain mode.
 * @param dev - The device structure.
 * @param en_dis - Set true in order to enable the daisy-chain mode.
 * 				   Accepted values: true
 * 									false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_set_daisy_chain_en_dis(ad5761r_dev *dev,
									   Bool en_dis)
{
	dev->daisy_chain_en = en_dis;
	
	return ad5761r_write(dev, CMD_DIS_DAISY_CHAIN,
			AD5761R_DIS_DAISY_CHAIN_DDC(!en_dis));
}

/**
 * Get the status of the daisy-chain mode.
 * @param dev - The device structure.
 * @param en_dis - The status of the daisy-chain mode (enabled, disabled).
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_daisy_chain_en_dis(ad5761r_dev *dev,
									   Bool *en_dis)
{
	*en_dis = dev->daisy_chain_en;

	return 0;
}

/**
 * Set the output_range.
 * @param dev - The device structure.
 * @param out_range - The output range.
 * 					  Accepted values: AD5761R_RANGE_M_10V_TO_P_10V,
 * 									   AD5761R_RANGE_0_V_TO_P_10V
 * 									   AD5761R_RANGE_M_5V_TO_P_5V
 * 									   AD5761R_RANGE_0V_TO_P_5V
 * 									   AD5761R_RANGE_M_2V5_TO_P_7V5
 * 									   AD5761R_RANGE_M_3V_TO_P_3V
 * 									   AD5761R_RANGE_0V_TO_P_16V
 * 									   AD5761R_RANGE_0V_TO_P_20V
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_set_output_range(ad5761r_dev *dev,
								 ad5761r_range out_range)
{
	dev->ra = out_range;

	return ad5761r_config(dev);
}

/**
 * Get the output_range.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_output_range(ad5761r_dev *dev,
								 ad5761r_range *out_range)
{
	*out_range = dev->ra;

	return 0;
}

/**
 * Set the power up voltage.
 * @param dev - The device structure.
 * @param pv - The power up voltage.
 * 			   Accepted values: AD5761R_SCALE_ZERO
 *								AD5761R_SCALE_HALF
 *								AD5761R_SCALE_FULL
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_set_power_up_voltage(ad5761r_dev *dev,
									 ad5761r_scale pv)
{
	dev->pv = pv;

	return ad5761r_config(dev);
}

/**
 * Get the power up voltage.
 * @param dev - The device structure.
 * @param pv - The power up voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_power_up_voltage(ad5761r_dev *dev,
									 ad5761r_scale *pv)
{
	*pv = dev->pv;

	return 0;
}

/**
 * Set the clear voltage.
 * @param dev - The device structure.
 * @param cv - The clear voltage.
 * 			   Accepted values: AD5761R_SCALE_ZERO
 *								AD5761R_SCALE_HALF
 *								AD5761R_SCALE_FULL
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_set_clear_voltage(ad5761r_dev *dev,
								  ad5761r_scale cv)
{
	dev->cv = cv;

	return ad5761r_config(dev);
}

/**
 * Get the clear voltage.
 * @param dev - The device structure.
 * @param cv - The clear voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_clear_voltage(ad5761r_dev *dev,
								  ad5761r_scale *cv)
{
	*cv = dev->cv;

	return 0;
}

/**
 * Enable/disable internal reference.
 * @param dev - The device structure.
 * @param en_dis - Set true in order to enable the internal reference.
 * 				   Accepted values: true
 * 									false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_set_internal_reference_en_dis(ad5761r_dev *dev,
											  Bool en_dis)
{
	dev->int_ref_en = en_dis;

	return ad5761r_config(dev);
}

/**
 * Get the status of the internal reference.
 * @param dev - The device structure.
 * @param en_dis - The status of the internal reference (enabled, disabled).
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_internal_reference_en_dis(ad5761r_dev *dev,
											  Bool *en_dis)
{
	*en_dis = dev->int_ref_en;

	return 0;
}

/**
 * Enable/disable ETS (exceed temperature shutdown) function.
 * @param dev - The device structure.
 * @param en_dis - Set true in order to enable the ETS function.
 * 				   Accepted values: true
 * 									false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_set_exceed_temp_shutdown_en_dis(ad5761r_dev *dev,
												Bool en_dis)
{
	dev->exc_temp_sd_en = en_dis;

	return ad5761r_config(dev);
}

/**
 * Get the status of the ETS (exceed temperature shutdown) function.
 * @param dev - The device structure.
 * @param en_dis - The status of the ETS function (enabled, disabled).
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_exceed_temp_shutdown_en_dis(ad5761r_dev *dev,
												Bool *en_dis)
{
	*en_dis = dev->exc_temp_sd_en;

	return 0;
}

/**
 * Enable/disable the twos complement bipolar output range.
 * @param dev - The device structure.
 * @param en_dis - Set true in order to enable the twos complement bipolar
 * 				   output range.
 * 				   Accepted values: true
 * 									false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_set_2c_bipolar_range_en_dis(ad5761r_dev *dev,
											Bool en_dis)
{
	dev->b2c_range_en = en_dis;

	return ad5761r_config(dev);
}

/**
 * Get the status of the twos complement bipolar output range.
 * @param dev - The device structure.
 * @param en_dis - The status of the twos complement bipolar output range
 * 				   (enabled, disabled).
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_2c_bipolar_range_en_dis(ad5761r_dev *dev,
											Bool *en_dis)
{
	*en_dis = dev->b2c_range_en;

	return 0;
}

/**
 * Enable/disable the 5% overrange.
 * @param dev - The device structure.
 * @param en_dis - Set true in order to enable the 5% overrange.
 * 				   Accepted values: true
 * 									false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_set_overrange_en_dis(ad5761r_dev *dev,
									 Bool en_dis)
{
	dev->ovr_en = en_dis;

	return ad5761r_config(dev);
}

/**
 * Get the status of the 5% overrange.
 * @param dev - The device structure.
 * @param en_dis - The status of the twos 5% overrange (enabled, disabled).
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_overrange_en_dis(ad5761r_dev *dev,
									 Bool *en_dis)
{
	*en_dis = dev->ovr_en;

	return 0;
}

/**
 * Get the short-circuit condition.
 * Note: The condition is reset at every control register write.
 * @param dev - The device structure.
 * @param en_dis - The status of the short-circuit condition (detected,
 * 				   not detected).
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_short_circuit_condition(ad5761r_dev *dev,
											Bool *sc)
{
	uint16_t reg_data;
	int32_t ret;

	ret = ad5761r_read(dev, CMD_RD_CTRL_REG, &reg_data);
	*sc = ((reg_data & AD5761R_CTRL_SC) >> 12);

	return ret;
}

/**
 * Get the brownout condition.
 * Note: The condition is reset at every control register write.
 * @param dev - The device structure.
 * @param en_dis - The status of the brownout condition (detected,
 * 				   not detected).
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_get_brownout_condition(ad5761r_dev *dev,
									   Bool *bo)
{
	uint16_t reg_data;
	int32_t ret;

	ret = ad5761r_read(dev, CMD_RD_CTRL_REG, &reg_data);
	*bo = ((reg_data & AD5761R_CTRL_BO) >> 11);

	return ret;
}

/**
 * Set the reset pin value.
 * @param dev - The device structure.
 * @param value - The pin value.
 * 				  Accepted values: GPIO_LOW
 * 				  				   GPIO_HIGH
 * @return 0 in case of success, negative error code otherwise.
 */
//int32_t ad5761r_set_reset_pin(ad5761r_dev *dev,
//							  uint8_t value)
//{
//	if (dev->gpio_reset >= 0) {
//		dev->gpio_reset_value = value;
//		return gpio_set_value(dev->gpio_reset, dev->gpio_reset_value);
//	} else
//		return -1;
//}

/**
 * Get the reset pin value.
 * @param dev - The device structure.
 * @param value - The pin value.
 * @return 0 in case of success, negative error code otherwise.
 */
//int32_t ad5761r_get_reset_pin(ad5761r_dev *dev,
//							  uint8_t *value)
//{
//	if (dev->gpio_reset >= 0) {
//		*value = dev->gpio_reset_value;
//		return 0;
//	} else
//		return -1;
//}

/**
 * Set the clr pin value.
 * @param dev - The device structure.
 * @param value - The pin value.
 * 				  Accepted values: GPIO_LOW
 * 				  				   GPIO_HIGH
 * @return 0 in case of success, negative error code otherwise.
 */
//int32_t ad5761r_set_clr_pin(ad5761r_dev *dev,
//							uint8_t value)
//{
//	if (dev->gpio_clr >= 0) {
//		dev->gpio_clr_value = value;
//		return gpio_set_value(dev->gpio_clr, dev->gpio_clr_value);
//	} else
//		return -1;
//}

/**
 * Get the clr pin value.
 * @param dev - The device structure.
 * @param value - The pin value.
 * @return 0 in case of success, negative error code otherwise.
 */
//int32_t ad5761r_get_clr_pin(ad5761r_dev *dev,
//							uint8_t *value)
//{
//	if (dev->gpio_clr >= 0) {
//		*value = dev->gpio_clr_value;
//		return 0;
//	} else
//		return -1;
//}

/**
 * Set the ldac pin value.
 * @param dev - The device structure.
 * @param value - The pin value.
 * 				  Accepted values: GPIO_LOW
 * 				  				   GPIO_HIGH
 * @return 0 in case of success, negative error code otherwise.
 */
//int32_t ad5761r_set_ldac_pin(ad5761r_dev *dev,
//							 uint8_t value)
//{
//	if (dev->gpio_ldac >= 0) {
//		dev->gpio_ldac_value = value;
//		return gpio_set_value(dev->gpio_ldac, dev->gpio_ldac_value);
//	} else
//		return -1;
//}

/**
 * Get the ldac pin value.
 * @param dev - The device structure.
 * @param value - The pin value.
 * @return 0 in case of success, negative error code otherwise.
 */
//int32_t ad5761r_get_ldac_pin(ad5761r_dev *dev,
//							 uint8_t *value)
//{
//	if (dev->gpio_ldac >= 0) {
//		*value = dev->gpio_ldac_value;
//		return 0;
//	} else
//		return -1;
//}

/**
 * Write to input register.
 * @param dev - The device structure.
 * @param dac_data - The DAC data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_write_input_register(ad5761r_dev *dev,
									 uint16_t dac_data)
{
	uint16_t reg_data;


		reg_data = AD5761R_DATA(dac_data);


	return ad5761r_write(dev, CMD_WR_TO_INPUT_REG, reg_data);
}

/**
 * Update DAC register.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_update_dac_register(ad5761r_dev *dev)
{
	return ad5761r_write(dev, CMD_UPDATE_DAC_REG, 0);
}

/**
 * Write to input register and update DAC register.
 * @param dev - The device structure.
 * @param dac_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_write_update_dac_register(ad5761r_dev *dev,
										  uint16_t dac_data)
{
	uint16_t reg_data;

	reg_data = AD5761R_DATA(dac_data);

	ad5761r_write(dev, CMD_WR_UPDATE_DAC_REG, reg_data);
	
	
	return 0;
}

/**
 * Software data reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_software_data_reset(ad5761r_dev *dev)
{
	return ad5761r_write(dev, CMD_SW_DATA_RESET, 0);
}

/**
 * Software full reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5761r_software_full_reset(ad5761r_dev *dev)
{
	return ad5761r_write(dev, CMD_SW_FULL_RESET, 0);
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return 0 in case of success, negative error code otherwise.
 */

static void AD5761R_GPIO_Init(void)
{
		GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_GPIOA_CLK_ENABLE();														//开启GPIOB时钟
		__HAL_RCC_GPIOC_CLK_ENABLE();	
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	
    GPIO_Initure.Pin= AD5761R_SCLK_PIN;							      //PB10，B11
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  							//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         								//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				//高速
    HAL_GPIO_Init(AD5761R_SCLK_PORT,&GPIO_Initure);     							//初始化
		
		
	 	GPIO_Initure.Pin = AD5761R_MOSI_PIN;
		HAL_GPIO_Init(AD5761R_MOSI_PORT,&GPIO_Initure);
		
		GPIO_Initure.Pin = AD5761R_SYNC_PIN;
		HAL_GPIO_Init(AD5761R_SYNC_PORT,&GPIO_Initure);
	
		GPIO_Initure.Pin= AD5761R_MISO_PIN;							
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  							
    GPIO_Initure.Pull=GPIO_PULLUP;         								
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				
    HAL_GPIO_Init(AD5761R_MISO_PORT,&GPIO_Initure);
		
		
		
//		  GPIO_Initure.Pin= AD5761R_SCLK_PIN;							      //PB10，B11
//    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  							//推挽输出
//    GPIO_Initure.Pull=GPIO_PULLUP;         								//上拉
//    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				//高速
//    HAL_GPIO_Init(AD5761R_SCLK_PORT,&GPIO_Initure);
		
		
		GPIO_Initure.Pin = AD5761R_CLEAR_PIN;
		GPIO_Initure.Mode=GPIO_MODE_INPUT;  							//推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         								//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH; 
		HAL_GPIO_Init(AD5761R_CLEAR_PORT,&GPIO_Initure);
		
			GPIO_Initure.Pin = AD5761R_LDAC_PIN;
		HAL_GPIO_Init(AD5761R_LDAC_PORT,&GPIO_Initure);
		
//		GPIO_Initure.Pin = AD5761R_RESET_PIN;
//		HAL_GPIO_Init(AD5761R_RESET_PORT,&GPIO_Initure);
		
		  GPIO_Initure.Pin= AD5761R_RESET_PIN;							      //PB10，B11
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  							//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         								//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  				//高速
    HAL_GPIO_Init(AD5761R_RESET_PORT,&GPIO_Initure);
		
//		GPIO_Initure.Pin = AD5761R_LDAC_PIN;
//		HAL_GPIO_Init(AD5761R_LDAC_PORT,&GPIO_Initure);
		
		HAL_GPIO_WritePin(AD5761R_RESET_PORT,AD5761R_RESET_PIN,GPIO_PIN_SET);
    AD5761R_SYNC_H;
}



//static uint8_t gpio_set_value( uint8_t gpio_pin,uint8_t value)   //没有用到
//{
//	 return 0;
//}

/***********AD5761R的初始化*******************/
void AD5761R_Init(void)
{
	ad5761r_dev dev;
	AD5761R_GPIO_Init();
	ad5761r_software_full_reset(&dev);

	dev.ra  = AD5761R_RANGE_M_3V_TO_P_3V;    //设置电压输出范围+-3V
	dev.int_ref_en = 1;                      //内部参考电压关闭
//		dev.int_ref_en = 0;                      //内部参考电压打开
//	dev.cv = AD5761R_SCALE_ZERO;             //clear电压选择
	dev.cv = AD5761R_SCALE_HALF;             //clear电压选择
	dev.b2c_range_en = 0;                    //直接2进制编码
//	dev.pv = AD5761R_SCALE_ZERO;             //上电电压0
	dev.pv = AD5761R_SCALE_HALF;             //上电电压中间电平
	dev.ovr_en = 0;                          //禁用5%超范围
	dev.daisy_chain_en = 0;
	dev.exc_temp_sd_en = 0;
  
	ad5761r_config(&dev);
	
	ad5761r_software_data_reset(&dev);
//	ad5761r_set_daisy_chain_en_dis(0,0);
	
//	AD5761R_CLEAR_H;
//	ad5761r_write_update_dac_register(0,0);
}


/*************设置AD5761R输出的电压 ************************/
void AD5761R_SetVoltage(unsigned int value)
{
	ad5761r_write_update_dac_register(0,value);
}

void AD5761R_SetVoltage_DIRECT(float value)
{ 
	float Temp_value;
	Temp_value = value/2491+1.2f;
//	Temp_value = value/2500+1.2f;
	Temp_value = Temp_value/2.4f *65536;
	ad5761r_write_update_dac_register(0,(unsigned int)Temp_value);
}
