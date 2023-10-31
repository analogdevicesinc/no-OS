/***************************************************************************//**
 *   @file   max14906.c
 *   @brief  Source file of MAX14906 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "max14906.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Read the (voltage) state of a channel (works for both input or output).
 * @param desc - device descriptor for the MAX14906
 * @param ch - channel index (0 based).
 * @param val - binary value representing a channel's voltage level (0 or 1).
 * @return 0 in case of success, negative error code otherwise
 */
int max14906_ch_get(struct max149x6_desc *desc, uint32_t ch, uint32_t *val)
{
	int ret;

	if (ch >= MAX14906_CHANNELS)
		return -EINVAL;

	ret = max149x6_reg_read(desc, MAX14906_DOILEVEL_REG, val);
	if (ret)
		return ret;

	*val = no_os_field_get(MAX14906_DOI_LEVEL_MASK(ch), *val);

	return 0;
}

/**
 * @brief Write the (logic) state of a channel (only for output channels).
 * @param desc - device descriptor for the MAX14906
 * @param ch - channel index (0 based).
 * @param val - binary value representing a channel's voltage level (0 or 1).
 * @return 0 in case of success, negative error code otherwise
 */
int max14906_ch_set(struct max149x6_desc *desc, uint32_t ch, uint32_t val)
{
	if (ch >= MAX14906_CHANNELS)
		return -EINVAL;

	return max149x6_reg_update(desc, MAX14906_SETOUT_REG,
				   MAX14906_HIGHO_MASK(ch), (val) ?
				   MAX14906_HIGHO_MASK(ch) : 0);
}

/**
 * @brief Configure a channel's function.
 * @param desc - device descriptor for the MAX14906
 * @param ch - channel index (0 based).
 * @param function - channel configuration (input, output or high-z).
 * @return 0 in case of success, negative error code otherwise
 */
int max14906_ch_func(struct max149x6_desc *desc, uint32_t ch,
		     enum max14906_function function)
{
	uint8_t setout_reg_val;
	int ret = 0;

	switch (function) {
	case MAX14906_HIGH_Z:
		setout_reg_val = MAX14906_IN;
		ret = max149x6_reg_update(desc, MAX14906_CONFIG_DO_REG, MAX14906_DO_MASK(ch),
					  no_os_field_prep(MAX14906_DO_MASK(ch),
							  MAX14906_PUSH_PULL));
		break;

	case MAX14906_IN:
		setout_reg_val = MAX14906_IN;
		ret = max149x6_reg_update(desc, MAX14906_CONFIG_DO_REG, MAX14906_DO_MASK(ch),
					  no_os_field_prep(MAX14906_DO_MASK(ch),
							  MAX14906_HIGH_SIDE));
		break;

	case MAX14906_OUT:
		setout_reg_val = MAX14906_OUT;
		break;

	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return max149x6_reg_update(desc, MAX14906_SETOUT_REG, MAX14906_CH_DIR_MASK(ch),
				   no_os_field_prep(MAX14906_CH_DIR_MASK(ch), setout_reg_val));
}

/**
 * @brief Configure the current limit for output channels
 * @param desc - device descriptor for the MAX14906
 * @param ch - channel index (0 based).
 * @param climit - current limit value.
 * @return 0 in case of success, negative error code otherwise
 */
int max14906_climit_set(struct max149x6_desc *desc, uint32_t ch,
			enum max14906_climit climit)
{
	return max149x6_reg_update(desc, MAX14906_CONFIG_CURR_LIM, MAX14906_CL_MASK(ch),
				   no_os_field_prep(MAX14906_CL_MASK(ch), climit));
}

/**
 * @brief Read an output channel's current limit
 * @param desc - device descriptor for the MAX14906
 * @param ch - channel index (0 based).
 * @param climit - current limit value.
 * @return 0 in case of success, negative error code otherwise
 */
int max14906_climit_get(struct max149x6_desc *desc, uint32_t ch,
			enum max14906_climit *climit)
{
	uint32_t reg_val;
	int ret;

	if (!climit)
		return -EINVAL;

	ret = max149x6_reg_read(desc, MAX14906_CONFIG_CURR_LIM, &reg_val);
	if (ret)
		return ret;

	*climit = no_os_field_get(MAX14906_CL_MASK(ch), reg_val);

	return 0;
}

/**
 * @brief Initialize and configure the MAX14906 device
 * @param desc - device descriptor for the MAX14906 that will be initialized.
 * @param param - initialization parameter for the device.
 * @return 0 in case of success, negative error code otherwise
 */
int max14906_init(struct max149x6_desc **desc,
		  struct max149x6_init_param *param)
{
	struct max149x6_desc *descriptor;
	uint32_t reg_val;
	int ret;
	int i;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, param->comm_param);
	if (ret)
		goto err;

	descriptor->crc_en = param->crc_en;

	ret = no_os_gpio_get_optional(&descriptor->en_gpio, param->en_gpio_param);
	if (ret)
		goto spi_err;

	if (descriptor->en_gpio) {
		ret = no_os_gpio_set_value(descriptor->en_gpio, NO_OS_GPIO_HIGH);
		if (ret)
			goto gpio_err;
	}

	ret = no_os_gpio_get_optional(&descriptor->fault_gpio,
				      param->fault_gpio_param);
	if (ret)
		goto gpio_err;

	if (descriptor->fault_gpio) {
		ret = no_os_gpio_direction_input(descriptor->fault_gpio);
		if (ret)
			goto fault_gpio_err;
	}

	ret = no_os_gpio_get_optional(&descriptor->ready_gpio,
				      param->ready_gpio_param);
	if (ret)
		goto fault_gpio_err;

	if (descriptor->ready_gpio) {
		ret = no_os_gpio_direction_input(descriptor->ready_gpio);
		if (ret)
			goto ready_gpio_err;
	}

	ret = no_os_gpio_get_optional(&descriptor->synch_gpio,
				      param->synch_gpio_param);
	if (ret)
		goto ready_gpio_err;

	if (descriptor->synch_gpio) {
		ret = no_os_gpio_direction_output(descriptor->synch_gpio,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto synch_gpio_err;
	}

	/* Clear the latched faults generated at power up */
	ret = max149x6_reg_read(descriptor, MAX14906_OVR_LD_REG, &reg_val);
	if (ret)
		goto synch_gpio_err;

	ret = max149x6_reg_read(descriptor, MAX14906_OPN_WIR_FLT_REG, &reg_val);
	if (ret)
		goto synch_gpio_err;

	ret = max149x6_reg_read(descriptor, MAX14906_SHD_VDD_FLT_REG, &reg_val);
	if (ret)
		goto synch_gpio_err;

	ret = max149x6_reg_read(descriptor, MAX14906_GLOBAL_FLT_REG, &reg_val);
	if (ret)
		goto synch_gpio_err;

	for (i = 0; i < MAX14906_CHANNELS; i++) {
		ret = max14906_ch_func(descriptor, i, MAX14906_HIGH_Z);
		if (ret)
			goto synch_gpio_err;
	}

	*desc = descriptor;

	return 0;

synch_gpio_err:
	no_os_gpio_remove(descriptor->synch_gpio);
ready_gpio_err:
	no_os_gpio_remove(descriptor->ready_gpio);
fault_gpio_err:
	no_os_gpio_remove(descriptor->fault_gpio);
gpio_err:
	no_os_gpio_remove(descriptor->en_gpio);
spi_err:
	no_os_spi_remove(descriptor->comm_desc);
err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated during init and place all the channels in high-z.
 * @param desc - device descriptor for the MAX14906 that will be initialized.
 * @return 0 in case of success, negative error code otherwise
 */
int max14906_remove(struct max149x6_desc *desc)
{
	int ret;
	int i;

	if (!desc)
		return -ENODEV;

	for (i = 0; i < MAX14906_CHANNELS; i++) {
		ret = max14906_ch_func(desc, i, MAX14906_HIGH_Z);
		if (ret)
			return ret;
	}

	no_os_spi_remove(desc->comm_desc);
	no_os_gpio_remove(desc->en_gpio);
	no_os_gpio_remove(desc->fault_gpio);
	no_os_gpio_remove(desc->ready_gpio);
	no_os_gpio_remove(desc->synch_gpio);

	no_os_free(desc);

	return 0;
}
