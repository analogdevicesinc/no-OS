/**************************************************************************//**
*   @file   ad5629r.c
*   @brief  Implementation of ad5629r Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad5629r.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
static const struct ad5629r_chip_info chip_info[] = {
	[ID_AD5629R] = {
		.resolution = 12,
		.communication = com_i2c,
	},
	[ID_AD5669R] = {
		.resolution = 16,
		.communication = com_i2c,
	},
	[ID_AD5668] = {
		.resolution = 16,
		.communication = com_spi,
	},
	[ID_AD5648] = {
		.resolution = 14,
		.communication = com_spi,
	},
	[ID_AD5628] = {
		.resolution = 12,
		.communication = com_spi,
	}
};

/******************************************************************************/
/************************** Functions Definitions *****************************/
/******************************************************************************/


/***************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param device -  AD5629R device structure.
 *
 * @param init_param - Device initial parameters.
 *                  Example: AD5629R, AD5669R, AD5668, AD5648, AD5628.
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
int8_t ad5629r_init(struct ad5629r_dev **device,
		    struct ad5629r_init_param init_param)
{
	struct ad5629r_dev *dev;
	int8_t status;

	dev = (struct ad5629r_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->act_device = init_param.act_device;

	if (chip_info[dev->act_device].communication == com_spi) {
		status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	} else {
		status = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
	}

	status |= no_os_gpio_get(&dev->gpio_ldac, &init_param.gpio_ldac);
	status |= no_os_gpio_get(&dev->gpio_clr, &init_param.gpio_clr);

	AD5629R_LDAC_OUT;
	AD5629R_LDAC_LOW;
	AD5629R_CLR_OUT;
	AD5629R_CLR_HIGH;

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD5629R_Init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad5629r_remove(struct ad5629r_dev *dev)
{
	int32_t ret;

	if (chip_info[dev->act_device].communication == com_spi)
		ret = no_os_spi_remove(dev->spi_desc);
	else
		ret = no_os_i2c_remove(dev->i2c_desc);

	if (dev->gpio_ldac)
		ret |= no_os_gpio_remove(dev->gpio_ldac);

	if (dev->gpio_clr)
		ret |= no_os_gpio_remove(dev->gpio_clr);

	no_os_free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Write to input register and read from output register via SPI.
 *
 * @param   dev      - The device structure.
 * @param   function - command control bits;
 * @param   dac_n    - address of selected DAC;
 * @param   data     - data to be written in register.
 *
 * @return  readBack - value read from register.
******************************************************************************/
void ad5629r_set_ctrl(struct ad5629r_dev *dev,
		      uint8_t function,
		      uint8_t dac_n,
		      uint32_t data)
{
	uint8_t data_buff [ 4 ]   = {0, 0, 0, 0};

	if(chip_info[dev->act_device].communication == com_spi) {
		data = data & 0xFFFFF;

		data_buff[0] = function;
		data_buff[1] = (dac_n << 4) | ((0xF0000 & data) >> 12);
		data_buff[2] = (0xFF00 & data) >> 8;
		data_buff[3] = (0xFF & data);

		no_os_spi_write_and_read(dev->spi_desc,
					 data_buff,
					 4);
	} else {
		if (chip_info[dev->act_device].communication == com_i2c) {
			data_buff[0] = (function << 4) | dac_n;
			data_buff[1] = (data & 0xFF00) >> 8;
			data_buff[2] = (data & 0x00FF) >> 0;

			no_os_i2c_write(dev->i2c_desc,
					data_buff,
					3,
					1);
		}
	}
}

/**************************************************************************//**
 * @brief Write to input register and read from output register via SPI.
 *
 * @param   dev      - The device structure.
 * @param   function - command control bits.
 * @param   dac_n    - address of selected DAC;
 * @param   dac_value - data to be written in input register.
 *
 * @return  readBack - value read from register.
******************************************************************************/
void ad5629r_set_input_reg(struct ad5629r_dev *dev,
			   uint8_t function,
			   uint8_t dac_n,
			   uint16_t dac_value)
{
	uint8_t data_buff [ 4 ]   = {0, 0, 0, 0};

	dac_value = dac_value << (MAX_RESOLUTION -
				  chip_info[dev->act_device].resolution);

	if(chip_info[dev->act_device].communication == com_spi) {
		dac_value = dac_value & 0xFFFF;

		data_buff[0] = function;
		data_buff[1] = (dac_n << 4) | ((0xF000 & dac_value) >> 12);
		data_buff[2] = (0xFF0 & dac_value) >> 4;
		data_buff[3] = (0xF & dac_value) << 4;

		no_os_spi_write_and_read(dev->spi_desc,
					 data_buff,
					 4);
	} else {
		data_buff[0] = (function << 4) | dac_n;
		data_buff[1] = (dac_value & 0xFF00) >> 8;
		data_buff[2] = (dac_value & 0x00FF) >> 0;

		no_os_i2c_write(dev->i2c_desc,
				data_buff,
				3,
				1);
	}
}

/***************************************************************************//**
 * @brief Writes a value to Input Register N of selected DAC channel.
 *
 * @param dev      - The device structure.
 * @param dac_value - Value to be written in register;
 * @param dac_n     - Address of selected DAC.
 *
 * @return none.
*******************************************************************************/
void ad5629r_write_reg_n(struct ad5629r_dev *dev,
			 uint8_t dac_n,
			 uint16_t dac_value)
{
	ad5629r_set_input_reg(dev,
			      AD5629R_WRITE_N,
			      dac_n,
			      dac_value);
}

/***************************************************************************//**
 * @brief Updates selected DAC register.
 *
 * @param dev  - The device structure.
 * @param dac_n - Address of selected DAC.
 *
 * @return none.
*******************************************************************************/
void ad5629r_update_dac_n(struct ad5629r_dev *dev,
			  uint8_t dac_n)
{
	ad5629r_set_input_reg(dev,
			      AD5629R_UPDATE_N,
			      dac_n,
			      0x0);
}

/***************************************************************************//**
 * @brief Writes a value to Input Register N of selected DAC channel, then
 *        updates all.
 *
 * @param dev      - The device structure.
 * @param dac_value - Value to be written in register;
 * @param dac_n     - Address of selected DAC.
 *
 * @return none.
*******************************************************************************/
void ad5629r_write_reg_nupdate_all(struct ad5629r_dev *dev,
				   uint8_t dac_n,
				   uint16_t dac_value)
{
	ad5629r_set_input_reg(dev,
			      AD5629R_WRITE_N_UPDATE_ALL,
			      dac_n,
			      dac_value);
}

/***************************************************************************//**
 * @brief Writes a value to Input Register N and updates the respective DAC
 *        channel.
 *
 * @param dev      - The device structure.
 * @param dac_value - Value to be written in register;
 * @param dac_n     - Address of selected DAC.
 *
 * @return none.
*******************************************************************************/
void ad5629r_write_reg_nupdate_n(struct ad5629r_dev *dev,
				 uint8_t dac_n,
				 uint16_t dac_value)
{
	ad5629r_set_input_reg(dev,
			      AD5629R_WRITE_N_UPDATE_N,
			      dac_n,
			      dac_value);
}

/***************************************************************************//**
 * @brief Sets the power mode for one or more selected DAC channels.
 *
 * @param dev    - The device structure.
 * @param dac_sel - a byte where each bit is corresponding to a DAC; when a bit is
 *                 set to 1, the corresponding DAC is selected.
 *                 Example: DAC_A_SEL - the selected DAC is DAC A;
 *                          DAC_D_SEL | DAC_F_SEL | DAC_H_SEL - the selected DACs
 *                          are: DAC D, DAC F and DAC H.
 * @param mode   - the desired power mode to be set.
 *                 Example: PWR_NORMAL      - normal operation;
 *                          PWR_1K_TO_GND   - 1KOhm to GND;
 *                          PWR_100K_TO_GND - 100KOhms to GND;
 *                          PWR_3_STATE     - three-state.
 *
 * @return none.
*******************************************************************************/
void ad5629r_set_power_mode(struct ad5629r_dev *dev,
			    uint8_t dac_sel,
			    uint8_t mode)
{
	uint32_t data = 0;

	data = (mode << 8) | dac_sel;

	ad5629r_set_ctrl(dev,
			 AD5629R_POWER,
			 0x0,
			 data);
}

/***************************************************************************//**
 * @brief Loads the Clear Code Register with a certain value.
 *
 * @param dev        - The device structure.
 * @param clear_value - the value to be set in all DAC registers after a clear
 *                     operation.
 *                     Example: CLR_TO_ZEROSCALE - clears to 0x0;
 *                              CLR_TO_MIDSCALE  - clears to 0x8000;
 *                              CLR_TO_FULLSCALE - clears to 0xFFFF;
 *                              CLR_NOOP         - no operation.
 *
 * @return none.
*******************************************************************************/
void ad5629r_load_clear_code_reg(struct ad5629r_dev *dev,
				 uint8_t clear_value)
{
	uint32_t data = 0;

	data = (uint32_t)clear_value;

	ad5629r_set_ctrl(dev,
			 AD5629R_LOAD_CLEAR_REG,
			 0x0,
			 data);
}

/***************************************************************************//**
 * @brief Loads the LDAC register with a certain value.
 *
 * @param dev    - The device structure.
 * @param dac_sel - a byte where each bit is corresponding to a DAC; when a bit is
 *                 set to 1, the corresponding DAC is selected to override LDAC
 *                 pin.
 *                 Example: DAC_A_SEL - the selected DAC to override the LDAC pin
 *                                      is DAC A;
 *                          DAC_D_SEL | DAC_F_SEL | DAC_H_SEL - the selected DACs
 *                          to override the LDAC pin are: DAC D, DAC F and DAC H.
 *
 * @return none.
*******************************************************************************/
void ad5629r_load_ldac_reg(struct ad5629r_dev *dev,
			   uint8_t dac_sel)
{
	uint32_t data = 0;

	data = (uint32_t)dac_sel;

	ad5629r_set_ctrl(dev,
			 AD5629R_LOAD_LDAC_REG,
			 0x0,
			 data);
}

/***************************************************************************//**
 * @brief Makes a power-on reset.
 *
 * @param dev - The device structure.
 *
 * @return none.
*******************************************************************************/
void ad5629r_reset(struct ad5629r_dev *dev)
{
	ad5629r_set_ctrl(dev,
			 AD5629R_RESET,
			 0x0,
			 0x0);
}

/***************************************************************************//**
 * @brief Turns on/off the internal reference.
 *
 * @param dev    - The device structure.
 * @param status - the status of internal reference.
 *                 Example: REF_ON  - the reference is on;
 *                          REF_OFF - the reference is off.
 *
 * @return none.
*******************************************************************************/
void ad5629r_set_ref(struct ad5629r_dev *dev,
		     uint8_t status)
{
	ad5629r_set_ctrl(dev,
			 AD5629R_REFERENCE,
			 0x0,
			 (uint32_t)status);
}


