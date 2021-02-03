/***************************************************************************//**
 *   @file   AD5791.c
 *   @brief  Implementation of AD5791 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
 *
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad5791.h"    // AD5791 definitions.

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
static const struct ad5791_chip_info chip_info[] = {
	[ID_AD5760] = {
		.resolution = 16,
	},
	[ID_AD5780] = {
		.resolution = 18,
	},
	[ID_AD5781] = {
		.resolution = 18,
	},
	[ID_AD5790] = {
		.resolution = 20,
	},
	[ID_AD5791] = {
		.resolution = 20,
	}
};

/***************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
int32_t ad5791_init(struct ad5791_dev **device,
		    struct ad5791_init_param init_param)
{
	struct ad5791_dev *dev;
	int32_t status;

	dev = (struct ad5791_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->act_device = init_param.act_device;

	/* GPIO */
	status = gpio_get(&dev->gpio_reset, &init_param.gpio_reset);
	status |= gpio_get(&dev->gpio_clr, &init_param.gpio_clr);
	status |= gpio_get(&dev->gpio_ldac, &init_param.gpio_ldac);

	AD5791_RESET_OUT;
	AD5791_RESET_HIGH;
	AD5791_LDAC_OUT;
	AD5791_LDAC_HIGH;
	AD5791_CLR_OUT;
	AD5791_CLR_HIGH;

	status |= spi_init(&dev->spi_desc, &init_param.spi_init);

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5791_init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad5791_remove(struct ad5791_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	ret |= gpio_remove(dev->gpio_reset);
	ret |= gpio_remove(dev->gpio_clr);
	ret |= gpio_remove(dev->gpio_ldac);

	free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 *                          Example:
 *                          AD5791_REG_DAC          - DAC register
 *                          AD5791_REG_CTRL         - Control register
 *                          AD5791_REG_CLR_CODE     - Clearcode register
 *                          AD5791_CMD_WR_SOFT_CTRL - Software control register
 * @param register_value   - Value of the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t ad5791_set_register_value(struct ad5791_dev *dev,
				  uint8_t register_address,
				  uint32_t register_value)
{
	uint8_t write_command[3] = {0, 0, 0};
	uint32_t spi_word = 0;
	int8_t status = 0;

	spi_word = AD5791_WRITE |
		   AD5791_ADDR_REG(register_address) |
		   (register_value & 0xFFFFF);
	write_command[0] = (spi_word >> 16) & 0x0000FF;
	write_command[1] = (spi_word >>  8) & 0x0000FF;
	write_command[2] = (spi_word >>  0) & 0x0000FF;
	status = spi_write_and_read(dev->spi_desc,
				    write_command,
				    3);
	if(status != 0) {
		return -1;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 *                          Example:
 *                          AD5791_REG_DAC          - DAC register
 *                          AD5791_REG_CTRL         - Control register
 *                          AD5791_REG_CLR_CODE     - Clearcode register
 *                          AD5791_CMD_WR_SOFT_CTRL - Software control register
 * @param value - Pointer to the register data.
 *
 * @return dataRead        - The register's value or negative error code.
*******************************************************************************/
int32_t ad5791_get_register_value(struct ad5791_dev *dev,
				  uint8_t register_address,
				  uint32_t *value)
{
	uint8_t register_word[3] = {0, 0, 0};
	int8_t status = 0;

	register_word[0] = (AD5791_READ | AD5791_ADDR_REG(register_address)) >> 16;
	status = spi_write_and_read(dev->spi_desc,
				    register_word,
				    3);
	if(status != 0) {
		return -1;
	}
	register_word[0] = 0x00;
	register_word[1] = 0x00;
	register_word[2] = 0x00;
	status = spi_write_and_read(dev->spi_desc,
				    register_word,
				    3);
	if(status != 0) {
		return -1;
	}
	*value = ((int32_t)register_word[0] << 16) |
		 ((int32_t)register_word[1] <<  8) |
		 ((int32_t)register_word[2] <<  0);

	return status;
}

/***************************************************************************//**
 * @brief Sets the DAC output in one of the three states.
 *
 * @param dev   - The device structure.
 * @param state - The output state.
 *                Example:
 *                AD5791_OUT_NORMAL     - normal operation mode
 *                AD5791_OUT_CLAMPED_6K - output is clamped via ~6KOhm to AGND
 *                AD5791_OUT_TRISTATE   - output is in tristate
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad5791_dac_ouput_state(struct ad5791_dev *dev,
			       uint8_t state)
{
	uint32_t old_ctrl = 0;
	uint32_t new_ctrl = 0;
	uint32_t val;
	int32_t status = 0;

	status = ad5791_get_register_value(dev, AD5791_REG_CTRL, &val);
	if(status < 0) {
		return status;
	}
	old_ctrl = val;
	/* Clear DACTRI and OPGND bits. */
	old_ctrl = old_ctrl & ~(AD5791_CTRL_DACTRI | AD5791_CTRL_OPGND);
	/* Sets the new state provided by the user. */
	new_ctrl = old_ctrl |
		   ((state << 2) & (AD5791_CTRL_DACTRI | AD5791_CTRL_OPGND));
	status = ad5791_set_register_value(dev,
					   AD5791_REG_CTRL,
					   new_ctrl);

	return status;
}

/***************************************************************************//**
 * @brief Writes to the DAC register.
 *
 * @param dev   - The device structure.
 * @param value - The value to be written to DAC.
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad5791_set_dac_value(struct ad5791_dev *dev,
			     uint32_t value)
{
	int32_t status = 0;

	value = value << (MAX_RESOLUTION - chip_info[dev->act_device].resolution);
	AD5791_LDAC_LOW;
	status = ad5791_set_register_value(dev,
					   AD5791_REG_DAC,
					   value);
	AD5791_LDAC_HIGH;

	return status;
}

/***************************************************************************//**
 * @brief Asserts RESET, CLR or LDAC in a software manner.
 *
 * @param dev             - The device structure.
 * @param instruction_bit - A Software Control Register bit.
 *                         Example:
 *                         AD5791_SOFT_CTRL_LDAC  - Load DAC
 *                         AD5791_SOFT_CTRL_CLR   - Clear
 *                         AD5791_SOFT_CTRL_RESET - Reset
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad5791_soft_instruction(struct ad5791_dev *dev,
				uint8_t instruction_bit)
{
	int32_t status = 0;

	status = ad5791_set_register_value(dev,
					   AD5791_CMD_WR_SOFT_CTRL,
					   instruction_bit);
	if(status < 0) {
		return status;
	}
	mdelay(1);    // Wait for the instruction to take effect.

	return status;
}

/***************************************************************************//**
 * @brief Configures the output amplifier, DAC coding, SDO state and the
 *        linearity error compensation.
 *
 * @param dev        - The device structure.
 * @param setup_word - Is a 24-bit value that sets or clears the Control Register
 *                    bits : RBUF bit(AD5791_CTRL_RBUF),
 *                           BIN/2sC bit(AD5791_CTRL_BIN2SC),
 *                           SDODIS bit(AD5791_CTRL_SDODIS) and
 *                           LINCOMP bits(AD5791_CTRL_LINCOMP(x)).
 *                    Example: AD5791_CTRL_BIN2SC | AD5791_CTRL_RBUF - sets
 *                             the DAC register to use offset binary coding and
 *                             powers down the internal output amplifier.
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
int32_t ad5791_setup(struct ad5791_dev *dev,
		     uint32_t setup_word)
{
	uint32_t old_ctrl = 0;
	uint32_t new_ctrl = 0;
	uint32_t val;
	int32_t status = 0;

	/* Reads the control register in order to save the options related to the
	   DAC output state that may have been configured previously. */
	status = ad5791_get_register_value(dev, AD5791_REG_CTRL, &val);
	if(status < 0) {
		return status;
	}
	old_ctrl = val;
	/* Clear LINCOMP, SDODIS, BIN2SC and RBUF bits. */
	old_ctrl = old_ctrl & ~(AD5791_CTRL_LINCOMP(-1) |
				AD5791_CTRL_SDODIS |
				AD5791_CTRL_BIN2SC |
				AD5791_CTRL_RBUF);
	/* Sets the new state provided by the user. */
	new_ctrl = old_ctrl | setup_word;
	status = ad5791_set_register_value(dev,
					   AD5791_REG_CTRL,
					   new_ctrl);

	return status;
}
