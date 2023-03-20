/***************************************************************************//**
 *   @file   max31865.c
 *   @brief  Implementation of MAX31865 Driver.
 *   @author JSanBuen (jose.sanbuenaventura@analog.com)
 *   @author MSosa (marcpaolo.sosa@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "max31865.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Device and comm init function
 * @param device - MAX31865 descriptor to be initialized
 * @param init_param - Init parameters for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max31865_init(struct max31865_dev **device,
		  struct max31865_init_param *init_param)
{
	int ret;
	struct max31865_dev *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = (struct max31865_dev *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, &init_param->spi_init);
	if (ret)
		goto err;

	descriptor->t_rc_delay = (int) (init_param->rtd_rc * 10.5 * 1000000);

	if (descriptor->t_rc_delay < 1)
		/* default 2mS/2000uS delay if no RC time constant specified */
		descriptor->t_rc_delay = 2000;
	else
		/* additional 1mS/1000uS delay (1-Shot section)  */
		descriptor->t_rc_delay += 1000;

	*device = descriptor;

	return 0;

err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Remove resources allocated by the init function
 * @param device  - max31865 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int max31865_remove(struct max31865_dev *device)
{
	int ret;

	if (!device)
		return -EINVAL;

	ret = no_os_spi_remove(device->comm_desc);
	if (ret)
		return ret;

	no_os_free(device);

	return 0;
}

/**
 * @brief update register contents
 * @param device - MAX31865 descriptor
 * @param reg_addr - register value to read from
 * @param reg_update - mask to update the value of selected register
 * @param or_mask - set to true for OR, false for AND masking
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_reg_update(struct max31865_dev *device, uint8_t reg_addr,
			uint8_t reg_update, bool or_mask)
{
	uint8_t temp;
	int ret;

	ret = max31865_read(device, reg_addr, &temp);
	if (ret)
		return ret;

	if (or_mask)
		temp |= reg_update;
	else
		temp &= reg_update;

	return max31865_write(device, reg_addr, temp);
}

/**
 * @brief Read raw register value
 * @param device - MAX31865 descriptor
 * @param reg_addr - register value to read from
 * @param reg_data - pointer for register value read
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_read(struct max31865_dev *device, uint8_t reg_addr,
		  uint8_t *reg_data)
{
	uint8_t raw_array[2];
	int ret;
	struct no_os_spi_msg temp_xfer = {
		.rx_buff = raw_array,
		.tx_buff = raw_array,
		.bytes_number = 2,
		.cs_change = 1,
	};

	raw_array[0] = reg_addr;
	raw_array[1] = reg_addr;

	if ((reg_addr & MAX31865_READ_MASK) != reg_addr)
		reg_addr &= MAX31865_READ_MASK;

	if (reg_addr < MAX31865_CONFIG_REG || reg_addr > MAX31865_FAULTSTAT_REG)
		return -EINVAL;

	ret = no_os_spi_transfer(device->comm_desc, &temp_xfer, 1);
	if (ret)
		return ret;

	*reg_data = raw_array[1];

	return 0;
}

/**
 * @brief Write raw register value
 * @param device - MAX31865 descriptor
 * @param reg_addr - register value to write to
 * @param reg_data - data to write in register
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_write(struct max31865_dev *device, uint8_t reg_addr,
		   uint8_t reg_data)
{
	int ret;
	uint8_t raw_array[2];
	struct no_os_spi_msg temp_xfer = {
		.rx_buff = raw_array,
		.tx_buff = raw_array,
		.bytes_number = 2,
		.cs_change = 1,
	};

	if (reg_addr < MAX31865_CONFIG_REG || reg_addr >= MAX31865_FAULTSTAT_REG)
		return -EINVAL;

	if (reg_addr == MAX31865_RTDMSB_REG || reg_addr == MAX31865_RTDLSB_REG)
		return -EINVAL;

	reg_addr |= MAX31865_WRITE_MASK;

	raw_array[0] = reg_addr;
	raw_array[1] = reg_data;

	return no_os_spi_transfer(device->comm_desc, &temp_xfer, 1);
}

/**
 * @brief Clear all faults in FAULTSTAT
 * @param device MAX31865 descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_clear_fault(struct max31865_dev *device)
{
	int ret;

	ret = max31865_reg_update(device, MAX31865_CONFIG_REG,
				  MAX31865_CONFIG_CLRFAULT_MASK, false);
	if (ret)
		return ret;

	return max31865_reg_update(device, MAX31865_CONFIG_REG,
				   MAX31865_CONFIG_FAULTSTAT, true);
}

/**
 * @brief Enable the bias voltage on the RTD sensor
 * @param device MAX31865 descriptor
 * @param bias_en If true bias is enabled, else disabled
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_enable_bias(struct max31865_dev *device, bool bias_en)
{
	if (bias_en)
		return max31865_reg_update(device, MAX31865_CONFIG_REG, MAX31865_CONFIG_BIAS,
					   true);

	return max31865_reg_update(device, MAX31865_CONFIG_REG, ~MAX31865_CONFIG_BIAS,
				   false);
}

/**
 * @brief Option for continuous conversions between 50/60 Hz
 * @param device MAX31865 descriptor
 * @param auto_conv_en If true, auto conversion is enabled
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_auto_convert(struct max31865_dev *device, bool auto_conv_en)
{
	if (auto_conv_en)
		return max31865_reg_update(device, MAX31865_CONFIG_REG,
					   MAX31865_CONFIG_MODEAUTO, true);

	return max31865_reg_update(device, MAX31865_CONFIG_REG, MAX31865_CONFIG_MODEOFF,
				   false);
}

/**
 * @brief Option for 50Hz or 60Hz noise filters
 * @param device MAX31865 descriptor
 * @param filt_en If true, 50Hz noise is filtered, else 60Hz(default)
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_enable_50Hz(struct max31865_dev *device, bool filt_en)
{
	device->is_filt_50 = filt_en;

	if (filt_en)
		return max31865_reg_update(device, MAX31865_CONFIG_REG,
					   MAX31865_CONFIG_FILT50HZ, true);

	return max31865_reg_update(device, MAX31865_CONFIG_REG,
				   MAX31865_CONFIG_FILT60HZ, false);
}

/**
 * @brief Update the contents of the HIGH and LOW FAULT registers of max31865
 * @param device MAX31865 descriptor
 * @param lower raw lower threshold
 * @param upper raw upper threshold
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_set_threshold(struct max31865_dev *device, uint16_t lower,
			   uint16_t upper)
{
	uint16_t lsb;
	uint16_t msb;
	int ret;

	lsb = no_os_field_get(0x00FF, upper);
	msb = no_os_field_get(0xFF00, upper);

	ret = max31865_write(device, MAX31865_HFAULTMSB_REG, (uint8_t)msb);
	if (ret)
		return ret;

	ret = max31865_write(device, MAX31865_HFAULTLSB_REG, (uint8_t)lsb);
	if (ret)
		return ret;

	lsb = no_os_field_get(0x00FF, lower);
	msb = no_os_field_get(0xFF00, lower);

	ret = max31865_write(device, MAX31865_LFAULTMSB_REG, (uint8_t)msb);
	if (ret)
		return ret;

	return max31865_write(device, MAX31865_LFAULTLSB_REG, (uint8_t)lsb);
}

/**
 * @brief Read the raw 16-bit lower threshold value
 * @param device MAX31865 descriptor
 * @param low_threshold pointer to hold the 16-bit lower threshold value
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_get_lower_threshold(struct max31865_dev *device,
				 uint16_t *low_threshold)
{
	uint8_t low_msb;
	uint8_t low_lsb;
	int ret;

	ret = max31865_read(device, MAX31865_LFAULTMSB_REG, &low_msb);
	if (ret)
		return ret;

	ret = max31865_read(device, MAX31865_LFAULTLSB_REG, &low_lsb);
	if (ret)
		return ret;

	*low_threshold = (low_msb << 8) | low_lsb;

	return 0;
}

/**
 * @brief Read the raw 16-bit upper threshold value
 * @param device MAX31865 descriptor
 * @param up_threshold pointer to hold the 16-bit upper threshold value
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_get_upper_threshold(struct max31865_dev *device,
				 uint16_t *up_threshold)
{
	uint8_t high_msb;
	uint8_t high_lsb;
	int ret;

	ret = max31865_read(device, MAX31865_HFAULTMSB_REG, &high_msb);
	if (ret)
		return ret;

	ret = max31865_read(device, MAX31865_HFAULTLSB_REG, &high_lsb);
	if (ret)
		return ret;

	*up_threshold = (high_msb << 8) | high_lsb;

	return 0;
}

/**
 * @brief N-wire option for RTD measurement setup
 * @param device MAX31865 descriptor
 * @param is_odd_wire If true, uses 3-wire setup. Else, uses 2/4-wire setup.
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_set_wires(struct max31865_dev *device, bool is_odd_wire)
{
	device->is_odd_wire = is_odd_wire;

	if (is_odd_wire)
		return max31865_reg_update(device, MAX31865_CONFIG_REG, MAX31865_CONFIG_3WIRE,
					   true);

	return max31865_reg_update(device, MAX31865_CONFIG_REG, MAX31865_CONFIG_2_4WIRE,
				   false);
}

/**
 * @brief Read the raw 16-bit value from the RTD_REG in one shot mode
 * @param device MAX31865 descriptor
 * @param rtd_reg pointer to hold the 16-bit raw RTD_REG value
 * @return 0 in case of success, negative error code otherwise
 */
int max31865_read_rtd(struct max31865_dev *device, uint16_t *rtd_reg)
{
	uint8_t reg_data;
	int ret;

	ret = max31865_clear_fault(device);
	if (ret)
		return ret;

	ret = max31865_enable_bias(device, true);
	if (ret)
		return ret;

	ret = max31865_reg_update(device, MAX31865_CONFIG_REG, MAX31865_CONFIG_1SHOT,
				  true);
	if (ret)
		return ret;

	if (device->is_filt_50)
		no_os_udelay(62500 + device->t_rc_delay);
	else
		no_os_udelay(52000 + device->t_rc_delay);

	ret = max31865_read(device, MAX31865_RTDMSB_REG, &reg_data);
	if (ret)
		return ret;

	*rtd_reg = reg_data << 8;

	ret = max31865_read(device, MAX31865_RTDLSB_REG, &reg_data);
	if(ret)
		return ret;

	*rtd_reg = *rtd_reg | (uint16_t)reg_data;
	*rtd_reg >>= 1;

	return max31865_enable_bias(device, false);
}
