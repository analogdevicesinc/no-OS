/***************************************************************************//**
 *   @file   ltc2672.c
 *   @brief  Implementation of ltc2672 Driver.
 *   @author JSanBuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ltc2672.h"
#include "no_os_spi.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Device and comm init function
 * @param device - ltc2672 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2672_init(struct ltc2672_dev **device,
		  struct ltc2672_init_param *init_param)
{
	int ret;
	struct ltc2672_dev *descriptor;

	descriptor = (struct ltc2672_dev *)calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, &init_param->spi_init);

	if (ret) {
		free(descriptor);
		return ret;
	}

	*device = descriptor;

	return ret;
}

/**
 * @brief Remove resources allocated by the init function
 * @param device  - ltc2672 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2672_remove(struct ltc2672_dev *device)
{
	int ret;

	if (!device)
		return -ENODEV;

	ret = no_os_spi_remove(device->comm_desc);
	if (ret)
		return -EINVAL;

	free(device);

	return ret;
}

/**
 * @brief Read raw register value
 * @param device - ltc2672 descriptor
 * @param reg_addr - register value
 * @param *reg_val - pointer for register value read
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_read(struct ltc2672_dev *device, uint8_t reg_addr,
		  uint8_t *reg_data)
{
	int ret;
	uint8_t raw_array[1];

	raw_array[0] = reg_addr;

	if( (reg_addr & ltc2672_READ_MASK) != reg_addr)
		reg_addr &= ltc2672_READ_MASK;

	if(reg_addr < ltc2672_CONFIG_REG || reg_addr > ltc2672_FAULTSTAT_REG)
		return -ENXIO;

	ret = no_os_spi_write_and_read(device->comm_desc, raw_array, 1);

	if (ret)
		return ret;

	*reg_data = raw_array[0];

	return ret;
}

/**
 * @brief Write raw register value
 * @param device - ltc2672 descriptor
 * @param reg_addr - register value
 * @param reg_val - data to write in register
 * @return 0 in case of success, negative error code otherwise
 */
int ltc2672_write(struct ltc2672_dev *device, uint8_t reg_addr,
		   uint8_t reg_data)
{
	int ret;
	uint8_t raw_array[2];

	if((reg_addr & ltc2672_WRITE_MASK) != ltc2672_WRITE_MASK)
		reg_addr |= ltc2672_WRITE_MASK;

	if(reg_addr < ltc2672_CONFIG_REG || reg_addr > ltc2672_FAULTSTAT_REG)
		return -ENXIO;

	raw_array[0] = reg_addr;
	raw_array[1] = reg_data;

	ret = no_os_spi_write_and_read(device->comm_desc, raw_array, 2);
	if (!ret)
		return ret;

	return ret;
}

/**
 * @brief Read the raw 8-bit FAULTSTAT register
 * @param device ltc2672 descriptor
 * @return The raw unsigned 8-bit FAULT status register
 */
int ltc2672_read_fault(struct ltc2672_dev *device, uint8_t *fault)
{
	uint8_t reg_data;
	int ret;

	*fault = reg_data;

	ret = ltc2672_read(device, ltc2672_FAULTSTAT_REG, &reg_data);

	return ret;
}

/**
 * @brief Clear all faults in FAULTSTAT
 * @param device ltc2672 descriptor
 */
int ltc2672_clear_fault(struct ltc2672_dev *device)
{
	uint8_t reg_data;
	int ret;

	ret = ltc2672_read(device, ltc2672_CONFIG_REG, &reg_data);
	if(ret)
		return ret;

	reg_data &= ~0x2C;
	reg_data |= ltc2672_CONFIG_FAULTSTAT;

	ret = ltc2672_write(device, ltc2672_CONFIG_REG, reg_data);

	return ret;
}

/**
 * @brief Enable the bias voltage on the RTD sensor
 * @param device ltc2672 descriptor
 * @param b If true bias is enabled, else disabled
 */
int ltc2672_enable_bias(struct ltc2672_dev *device, bool bias_en)
{
	uint8_t reg_data;
	int ret;

	ret = ltc2672_read(device, ltc2672_CONFIG_REG, &reg_data);
	if(ret)
		return ret;

	if (bias_en)
		reg_data |= ltc2672_CONFIG_BIAS;
	else
		reg_data &= ~ltc2672_CONFIG_BIAS;

	ret = ltc2672_write(device, ltc2672_CONFIG_REG, reg_data);

	return ret;
}

/**
 * @brief Option for continuous conversions between 50/60 Hz
 * @param device ltc2672 descriptor
 * @param b If true, auto conversion is enabled
 */
int ltc2672_auto_convert(struct ltc2672_dev *device, bool auto_conv_en)
{
	uint8_t reg_data;
	int ret;

	ret = ltc2672_read(device, ltc2672_CONFIG_REG, &reg_data);

	if(ret)
		return ret;

	if (auto_conv_en)
		reg_data |= ltc2672_CONFIG_MODEAUTO;
	else
		reg_data &= ~ltc2672_CONFIG_MODEAUTO;

	ret = ltc2672_write(device, ltc2672_CONFIG_REG, reg_data);

	return ret;
}

/**
 * @brief Option for 50Hz or 60Hz noise filters
 * @param device ltc2672 descriptor
 * @param b If true, 50Hz noise is filtered, else 60Hz(default)
 */
int ltc2672_enable_50Hz(struct ltc2672_dev *device, bool filt_en)
{
	uint8_t reg_data;
	int ret;

	ret = ltc2672_read(device, ltc2672_CONFIG_REG, &reg_data);
	if(ret)
		return ret;

	if (filt_en)
		reg_data |= ltc2672_CONFIG_FILT50HZ;
	else
		reg_data &= ~ltc2672_CONFIG_FILT50HZ;

	ret = ltc2672_write(device, ltc2672_CONFIG_REG, reg_data);

	return ret;
}

/**
 * @brief Write the lower and upper values into the threshold fault
    register to values as returned by readRTD()
 * @param device ltc2672 descriptor
 * @param lower raw lower threshold
 * @param upper raw upper threshold
 */
int ltc2672_set_threshold(struct ltc2672_dev *device, uint16_t lower,
			   uint16_t upper)
{
	uint16_t lsb;
	uint16_t msb;
	int ret;

	lsb = no_os_field_get(0x00FF, upper);
	msb = no_os_field_get(0xFF00, upper);

	ret = ltc2672_write(device, ltc2672_HFAULTMSB_REG, (uint8_t)lsb);
	if(ret)
		return ret;

	ret = ltc2672_write(device, ltc2672_HFAULTLSB_REG, (uint8_t)msb);
	if(ret)
		return ret;

	lsb = no_os_field_get(0x00FF, lower);
	msb = no_os_field_get(0xFF00, lower);

	ret = ltc2672_write(device, ltc2672_LFAULTMSB_REG, (uint8_t)lsb);
	if(ret)
		return ret;

	ret = ltc2672_write(device, ltc2672_LFAULTLSB_REG, (uint8_t)msb);

	return ret;
}

/**
 * @brief Read the raw 16-bit lower threshold value
 * @param device ltc2672 descriptor
 * @return The raw unsigned 16-bit value, NOT temperature value
 */
int ltc2672_get_lower_threshold(struct ltc2672_dev *device,
				 uint16_t *low_threshold)
{
	uint8_t low_msb;
	uint8_t low_lsb;
	int ret;

	ret = ltc2672_read(device, ltc2672_LFAULTMSB_REG, &low_msb);
	if(ret)
		return ret;

	ret = ltc2672_read(device, ltc2672_LFAULTLSB_REG, &low_lsb);
	if(ret)
		return ret;

	*low_threshold = (((uint16_t)low_msb << 8) | (uint16_t)low_lsb);

	return ret;
}

/**
 * @brief Read the raw 16-bit upper threshold value
 * @param device ltc2672 descriptor
 * @return The raw unsigned 16-bit value, NOT temperature value
 */
int ltc2672_get_upper_threshold(struct ltc2672_dev *device,
				 uint16_t *up_threshold)
{
	uint8_t high_msb;
	uint8_t high_lsb;
	int ret;

	ret = ltc2672_read(device, ltc2672_HFAULTMSB_REG, &high_msb);
	if(ret)
		return ret;

	ret = ltc2672_read(device, ltc2672_HFAULTLSB_REG, &high_lsb);
	if(ret)
		return ret;

	*up_threshold = (((uint16_t)high_msb << 8) | (uint16_t)high_lsb);

	return ret;
}

/**
 * @brief RTD setup options for ltc2672_2WIRE, ltc2672_3WIRE, ltc2672_4WIRE
 * @param device ltc2672 descriptor
 * @param wires The number of wires in enum format
 */
int ltc2672_set_wires(struct ltc2672_dev *device, bool is_odd_wire)
{
	uint8_t reg_data = 0x00;
	int ret;

	ret = ltc2672_read(device, ltc2672_CONFIG_REG, &reg_data);
	if(ret)
		return ret;

	if (is_odd_wire)
		reg_data |= ltc2672_CONFIG_3WIRE;
	else
		reg_data &= ~ltc2672_CONFIG_3WIRE;

	ret = ltc2672_write(device, ltc2672_CONFIG_REG, reg_data);

	return ret;
}


/**
 * @brief Read the raw 16-bit value from the RTD_REG in one shot mode
 * @param device ltc2672 descriptor
 * @return The raw unsigned 16-bit value, NOT temperature
 */
int ltc2672_read_rtd(struct ltc2672_dev *device, uint16_t *rtd_reg)
{
	uint8_t reg_data;
	int ret;

	ret = ltc2672_clear_fault(device);
	if(ret)
		return ret;

	ret = ltc2672_enable_bias(device, true);
	if(ret)
		return ret;

	ret = ltc2672_read(device, ltc2672_CONFIG_REG, &reg_data);
	if(ret)
		return ret;

	reg_data |= ltc2672_CONFIG_1SHOT;

	ret = ltc2672_write(device, ltc2672_CONFIG_REG, reg_data);
	if(ret)
		return ret;

	ret = ltc2672_read(device, ltc2672_RTDMSB_REG, &reg_data);
	if(ret)
		return ret;

	*rtd_reg = ((uint16_t)reg_data << 8);

	ret = ltc2672_read(device, ltc2672_RTDLSB_REG, &reg_data);
	if(ret)
		return ret;

	*rtd_reg = *rtd_reg | (uint16_t)reg_data;
	*rtd_reg >>= 1;

	ret = ltc2672_enable_bias(device, false);

	return ret;
}
