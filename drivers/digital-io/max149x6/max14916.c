/***************************************************************************//**
 *   @file   max14916.c
 *   @brief  Source file of MAX14916 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "max14916.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Read the high-side state of a channel's switch.
 * @param desc - device descriptor for the MAX14916.
 * @param ch - channel index (0 based).
 * @param val - binary value representing a channel's high-side switc state.
 * @return 0 in case of success, negative error code otherwise.
 */
int max14916_ch_get(struct max149x6_desc *desc, uint32_t ch, uint32_t *val)
{
	int ret;

	if (ch >= MAX14916_CHANNELS)
		return -EINVAL;

	ret = max149x6_reg_read(desc, MAX14916_SETOUT_REG, val);
	if (ret)
		return ret;

	*val = no_os_field_get(MAX14916_SETOUT_MASK(ch), *val);

	return 0;
}

/**
 * @brief Write the (logic) state of a channel (on/off).
 * @param desc - device descriptor for the MAX14916.
 * @param ch - channel index (0 based).
 * @param val - binary value representing a channel's voltage level (0 or 1).
 * @return 0 in case of success, negative error code otherwise.
 */
int max14916_ch_set(struct max149x6_desc *desc, uint32_t ch, uint32_t val)
{
	if (ch >= MAX14916_CHANNELS)
		return -EINVAL;

	if (val > 1)
		return -EINVAL;

	return max149x6_reg_update(desc, MAX14916_SETOUT_REG,
				   MAX14916_SETOUT_MASK(ch), (val) ?
				   MAX14916_SETOUT_MASK(ch) : 0);
}

/**
 * @brief Read an output channel's current limit.
 * @param desc - device descriptor for the MAX14916.
 * @param ch - channel index (0 based).
 * @param state - status led state.
 * @return 0 in case of success, negative error code otherwise.
 */
int max14916_sled_set(struct max149x6_desc *desc, uint32_t ch,
		      enum max14916_sled_state state)
{
	int ret;

	if (ch >= MAX14916_CHANNELS)
		return -EINVAL;

	/** SLEDSet bit needs to be set to 1 in config1 register in order
	    to change the SLED state. */
	ret = max149x6_reg_update(desc, MAX14916_CONFIG1_REG,
				  MAX14916_SLED_MASK,
				  no_os_field_prep(MAX14916_SLED_MASK, 1));
	if (ret)
		return ret;

	return max149x6_reg_update(desc, MAX14916_SET_SLED_REG,
				   MAX14916_SLED_CH_MASK(ch),
				   no_os_field_prep(MAX14916_SLED_CH_MASK(ch),
						   state));
}

/**
 * @brief Set FLEDs minimum on-time to requested value.
 * @param desc - device descriptor for the MAX14916.
 * @param fled_time - minimum on-time for the FLEDs to be set.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max14916_fled_time_set(struct max149x6_desc *desc,
			   enum max14916_fled_time fled_time)
{
	int ret;

	/* FLEDs need to be controlled internally which requires FLEDSet bit
	   in the config1 register to be set to 0. */
	ret = max149x6_reg_update(desc, MAX14916_CONFIG1_REG,
				  MAX14916_FLED_MASK,
				  no_os_field_prep(MAX14916_FLED_MASK, 0));
	if(ret)
		return ret;

	switch(fled_time) {
	case MAX14916_FLED_TIME_DISABLED:
	case MAX14916_FLED_TIME_1S:
	case MAX14916_FLED_TIME_2S:
	case MAX14916_FLED_TIME_3S:
		return max149x6_reg_update(desc, MAX14916_CONFIG1_REG,
					   MAX14916_FLED_STRETCH_MASK,
					   no_os_field_prep
					   (MAX14916_FLED_STRETCH_MASK,
					    fled_time));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Set watchdog timeout in the config2 register.
 * @param desc - device descriptor for the MAX14916.
 * @param wd - watchdog timer to be selected.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max14916_set_wd(struct max149x6_desc *desc, enum max14916_wd wd)
{
	if (wd > MAX14916_WD_1200MS)
		return -EINVAL;

	return max149x6_reg_update(desc, MAX14916_CONFIG2_REG, MAX14916_WD_MASK,
				   no_os_field_prep(MAX14916_WD_MASK, wd));
}

/**
 * @brief Set current magnitude for the ow off in the config2 register.
 * @param desc - device descriptor for the MAX14916.
 * @param ow_off_cs - current magnitude to be set.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max14916_set_ow_off(struct max149x6_desc *desc,
			enum max14916_ow_off_cs ow_off_cs)
{
	if (ow_off_cs > MAX14916_OW_OFF_CS_600UA)
		return -EINVAL;

	return max149x6_reg_update(desc, MAX14916_CONFIG2_REG,
				   MAX14916_OW_OFF_CS_MASK,
				   no_os_field_prep(MAX14916_OW_OFF_CS_MASK,
						   ow_off_cs));
}

/**
 * @brief Set short to vdd threshold in the config2 register.
 * @param desc - device descriptor for the MAX14916.
 * @param sht_vdd_thr - threshold to be set.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max14916_set_sht_vdd_thr(struct max149x6_desc *desc,
			     enum max14916_sht_vdd_thr sht_vdd_thr)
{
	if (sht_vdd_thr > MAX14916_SHT_VDD_THR_14V)
		return -EINVAL;

	return max149x6_reg_update(desc, MAX14916_CONFIG2_REG,
				   MAX14916_SHT_VDD_THR_MASK,
				   no_os_field_prep(MAX14916_SHT_VDD_THR_MASK,
						   sht_vdd_thr));
}

/**
 * @brief Initialize and configure the MAX14916 device.
 * @param desc - device descriptor for the MAX14916 that will be initialized.
 * @param param - initialization parameter for the device.
 * @return 0 in case of success, negative error code otherwise.
 */
int max14916_init(struct max149x6_desc **desc,
		  struct max149x6_init_param *param)
{
	struct max149x6_desc *descriptor;
	uint32_t reg_val;
	int ret, i;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, param->comm_param);
	if (ret)
		goto err;

	descriptor->crc_en = param->crc_en;

	ret = no_os_gpio_get_optional(&descriptor->en_gpio,
				      param->en_gpio_param);
	if (ret)
		goto spi_err;

	if (descriptor->en_gpio) {
		ret = no_os_gpio_set_value(descriptor->en_gpio,
					   NO_OS_GPIO_HIGH);
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
	ret = max149x6_reg_read(descriptor, MAX14916_OVR_LD_REG, &reg_val);
	if (ret)
		goto synch_gpio_err;

	ret = max149x6_reg_read(descriptor, MAX14916_OW_OFF_FAULT_REG,
				&reg_val);
	if (ret)
		goto synch_gpio_err;

	ret = max149x6_reg_read(descriptor, MAX14916_SHD_VDD_FAULT_REG,
				&reg_val);
	if (ret)
		goto synch_gpio_err;

	ret = max149x6_reg_read(descriptor, MAX14916_GLOB_ERR_REG, &reg_val);
	if (ret)
		goto synch_gpio_err;

	for(i = 0; i < MAX14916_CHANNELS; i++) {
		ret = max14916_ch_set(descriptor, i, 0);
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
 * @brief Free the resources allocated during init.
 * @param desc - device descriptor for the MAX14916 that will be initialized.
 * @return 0 in case of success, negative error code otherwise.
 */
int max14916_remove(struct max149x6_desc *desc)
{
	if (!desc)
		return -ENODEV;

	no_os_spi_remove(desc->comm_desc);

	no_os_gpio_remove(desc->en_gpio);
	no_os_gpio_remove(desc->fault_gpio);
	no_os_gpio_remove(desc->ready_gpio);
	no_os_gpio_remove(desc->synch_gpio);

	no_os_free(desc);

	return 0;
}
